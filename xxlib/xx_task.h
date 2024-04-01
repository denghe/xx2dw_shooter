#pragma once
#include "xx_time.h"
#include "xx_blocklink.h"
#include "xx_ptr.h"

namespace xx {
    struct YieldType {
        ptrdiff_t v;
        void* p;
        YieldType() : v(0), p(nullptr) {}
        YieldType(ptrdiff_t v) : v(v), p(nullptr) {}
        YieldType(ptrdiff_t v, void* p) : v(v), p(p) {}
        YieldType(YieldType const&) = default;
        YieldType& operator=(YieldType const&) = default;
    };

    template<typename R = void>
    struct Task;

    namespace detail {
        template<typename Derived, typename R>
        struct PromiseBase {
            std::coroutine_handle<> prev, last;
            PromiseBase *root{ this };
            YieldType y;

            struct FinalAwaiter {
                bool await_ready() const noexcept { return false; }
                void await_resume() noexcept {}
                template<typename promise_type>
                std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> curr) noexcept {
                    if (auto &p = curr.promise(); p.prev) return p.prev;
                    else return std::noop_coroutine();
                }
            };
            Task<R> get_return_object() noexcept {
                auto tmp = Task<R>{ std::coroutine_handle<Derived>::from_promise(*(Derived*)this) };
                tmp.coro.promise().last = tmp.coro;
                return tmp;
            }
            std::suspend_always yield_value(YieldType v) {
                root->y = v;
                return {};
            }
            std::suspend_always initial_suspend() { return {}; }
            FinalAwaiter final_suspend() noexcept(true) { return {}; }
            void unhandled_exception() { throw; }
        };

        template<typename R>
        struct Promise final : PromiseBase<Promise<R>, R> {
            std::optional<R> r;

            template<typename T>
            void return_value(T&& v) { r.emplace(std::forward<T>(v)); }
        };

        template<>
        struct Promise<void> : PromiseBase<Promise<void>, void> {
            void return_void() noexcept {}
        };
    }

    /*************************************************************************************************************************/
    /*************************************************************************************************************************/

    template<typename R>
    struct [[nodiscard]] Task {
        using promise_type = detail::Promise<R>;
        using H = std::coroutine_handle<promise_type>;
        H coro;

        Task() = default;
        Task(H h) { coro = h; }
        Task(Task const &o) = delete;
        Task &operator=(Task const &o) = delete;
        Task(Task &&o) : coro(std::exchange(o.coro, nullptr)) {}
        Task &operator=(Task &&o) {
            std::swap(coro, o.coro);
            return *this;
        }
        ~Task() { if (coro) { coro.destroy(); } }

        struct Awaiter {
            bool await_ready() const noexcept { return false; }
            decltype(auto) await_suspend(std::coroutine_handle<> prev) noexcept {
                auto& cp = curr.promise();
                cp.prev = prev;
                cp.root = ((H&)prev).promise().root;
                cp.root->last = curr;
                return curr;
            }
            decltype(auto) await_resume() {
                assert(curr.done());
                auto& p = curr.promise();
                p.root->last = p.prev;
                if constexpr (std::is_same_v<void, R>) return;
                else return *p.r;
            }
            H curr;
        };
        Awaiter operator co_await() const& noexcept { return {coro}; }

        decltype(auto) Result() const { return *coro.promise().r; }
        YieldType const& YieldValue() const { return coro.promise().y; }
        YieldType& YieldValue() { return coro.promise().y; }

        template<bool runOnce = false>
        XX_FORCE_INLINE void Run() {
            auto& p = coro.promise();
            auto& c = p.last;
            while(c && !c.done()) {
                c.resume();
                if constexpr(runOnce) return;
            }
        }
        operator bool() const { return /*!coro ||*/ coro.done(); }
        bool Resume() { Run<true>(); return coro.done(); }
        bool operator()() { Run<true>();return coro.done(); }
        void RunAll() { Run<true>(); }
    };

    template<typename R>
    struct IsPod<Task<R>> : std::true_type {};

    /*************************************************************************************************************************/
    /*************************************************************************************************************************/

    struct Tasks {
        BlockLink<xx::Task<>, BlockLinkVINPT> tasks;
        void Clear() { tasks.Clear(); }
        int32_t Count() const { return tasks.Count(); }
        bool Empty() const { return !tasks.Count(); }
        void Reserve(int32_t cap) { tasks.Reserve(cap); }

        Tasks(Tasks const &) = delete;
        Tasks &operator=(Tasks const &) = delete;
        Tasks(Tasks &&) noexcept = default;
        Tasks &operator=(Tasks &&) noexcept = default;
        explicit Tasks(int32_t cap = 8) {
            tasks.Reserve(cap);
        }

        // T: Task<> or callable
        template<typename T>
        BlockLinkVI Add(T &&t) {
            if constexpr (std::is_convertible_v<Task<>, T>) {           // ([](...)->xx::Task<>{})(...)
                if (t) return {};
                return (BlockLinkVI)tasks.EmplaceNode(std::forward<T>(t));
            } else {
                return Add([](T t) -> Task<> {
                    if constexpr (std::is_convertible_v<Task<>, FuncR_t<T>>) {
                        co_await t();                                   // [...]()->xx::Task<>{}
                    } else {
                        t();                                            // [...](){}
                        co_return;
                    }
                }(std::forward<T>(t)));
            }
        }

        // resume once
        int32_t operator()() {
            tasks.ForeachLink([&](xx::Task<>& o)->ForeachResult {
                return o.Resume() ? ForeachResult::RemoveAndContinue : ForeachResult::Continue;
            });
            return tasks.Count();
        }
    };

    /*************************************************************************************************************************/
    /*************************************************************************************************************************/

    struct TaskGuard {
        Tasks* ptr;
        BlockLinkVI vi;

        TaskGuard() : ptr(nullptr) {};
        TaskGuard(TaskGuard const&) = delete;
        TaskGuard& operator=(TaskGuard const&) = delete;

        TaskGuard(TaskGuard && o) noexcept {
            ptr = o.ptr;
            vi = o.vi;
            o.ptr = {};
            o.vi = {};
        }
        TaskGuard& operator=(TaskGuard &&o) noexcept {
            std::swap(ptr, o.ptr);
            std::swap(vi, o.vi);
            return *this;
        }

        XX_FORCE_INLINE void Clear() {
            if (ptr) {
                ptr->tasks.Remove(vi);
                ptr = {};
            }
        }

        ~TaskGuard() {
            Clear();
        }

        template<typename T>
        void operator()(Tasks& tasks, T &&t) {
            Clear();
            ptr = &tasks;
            vi = tasks.Add(std::forward<T>(t));
        }

        operator bool() const {
            return ptr && ptr->tasks.TryGet(vi);
        }
    };

    /*************************************************************************************************************************/
    /*************************************************************************************************************************/

    // Cond ( Condition ): Weak<T> / WeakHolder or std::optional<Weak<T> / WeakHolder> / bool func()
    template<typename Cond>
    struct CondTasks {
        BlockLink<std::pair<Cond, Task<>>, BlockLinkVINPT> tasks;
        void Clear() { tasks.Clear(); }
        int32_t Count() const { return tasks.Count(); }
        bool Empty() const { return !tasks.Count(); }
        void Reserve(int32_t cap) { tasks.Reserve(cap); }

        CondTasks(CondTasks const&) = delete;
        CondTasks& operator=(CondTasks const&) = delete;
        CondTasks(CondTasks&&) noexcept = default;
        CondTasks& operator=(CondTasks&&) noexcept = default;
        explicit CondTasks(int32_t cap = 8) {
            tasks.Reserve(cap);
        }

        // C: Condition
        // T: Task<> or callable
        template<typename C, typename T>
        void Add(C&& c, T&& t) {
            if constexpr (std::is_convertible_v<Task<>, T>) {
                if (t) return;
                tasks.Emplace(std::forward<C>(c), std::forward<T>(t));
            } else {
                Add(std::forward<C>(c), [](T t) -> Task<> {
                    if constexpr (std::is_convertible_v<Task<>, FuncR_t<T>>) {
                        co_await t();                                   // [...]()->xx::Task<>{}
                    } else {
                        t();                                            // [...](){}
                        co_return;
                    }
                }(std::forward<T>(t)));
            }
        }

        template<typename T>
        void Add(T&& t) {
            if constexpr (xx::IsFunction_v<Cond>) {
                assert(false);
            }
            Add(Cond{}, std::forward<T>(t));
        }

        // resume once
        int32_t operator()() {
            tasks.ForeachLink([&](std::pair<Cond, Task<>>& o)->ForeachResult {
                if constexpr(IsOptional_v<Cond>) {
                    if (o.first.has_value()) return (!o.first.value() || o.second.Resume()) ? ForeachResult::RemoveAndContinue : ForeachResult::Continue;
                    else return o.second.Resume() ? ForeachResult::RemoveAndContinue : ForeachResult::Continue;
                } else if constexpr (std::is_invocable_v<Cond>) {
                    if (o.first()) {
                        return o.second.Resume() ? ForeachResult::RemoveAndContinue : ForeachResult::Continue;
                    } else return ForeachResult::RemoveAndContinue;
                } else {
                    if (o.first) {
                        return o.second.Resume() ? ForeachResult::RemoveAndContinue : ForeachResult::Continue;
                    } else ForeachResult::RemoveAndContinue;
                }
            });
            return this->tasks.Count();
        }
    };

    using WeakTasks = CondTasks<WeakHolder>;

    using OptWeakTasks = CondTasks<std::optional<WeakHolder>>;

    /*************************************************************************************************************************/
    /*************************************************************************************************************************/

    template<int timeoutSecs = 15>
    struct EventTasks {
        using Tuple = std::tuple<ptrdiff_t, void*, double, Task<>>;
        List<Task<>, int> tasks;
        List<Tuple, int> eventTasks;

        template<typename T>
        void Add(T&& t) {
            if constexpr (std::is_base_of_v<Task<>, T>) {
                xx_assert(!t);
                xx_assert(!t.coro.promise().y.p);
                tasks.Emplace(std::forward<T>(t));
            } else {
                Add([](T t)->Task<> {
                    co_await t();
                }(std::forward<T>(t)));
            }
        }

        // match key & handle args & resume coro
        // void(*Handler)( void* ) = [???](auto p) { *(T*)p = ???; }
        // return 0: miss or success
        template<typename Handler>
        ptrdiff_t operator()(ptrdiff_t v, Handler&& h) {
            for (int i = eventTasks.len - 1; i >= 0; --i) {
                auto& t = eventTasks[i];
                if (v == std::get<0>(t)) {
                    h(std::get<1>(t));
                    return Resume(i, t);
                }
            }
            return 0;
        }

        // handle eventTasks timeout & resume tasks
        // return 0: success
        ptrdiff_t operator()() {
            if (!eventTasks.Empty()) {
                auto now = NowSteadyEpochSeconds();
                for (int i = eventTasks.len - 1; i >= 0; --i) {
                    auto& t = eventTasks[i];
                    if (std::get<2>(t) < now) {
                        if (auto r = Resume(i, t)) return r;
                    }
                }
            }
            if (!tasks.Empty()) {
                for (int i = tasks.len - 1; i >= 0; --i) {
                    if (auto& t = tasks[i]; t.Resume()) {
                        tasks.SwapRemoveAt(i);
                    } else {
                        auto& y = t.coro.promise().y;
                        if (y.p) {
                            eventTasks.Emplace(Tuple{ y.v, y.p, NowSteadyEpochSeconds() + timeoutSecs, std::move(t) });
                            tasks.SwapRemoveAt(i);
                        } else {
                            if (y.v) return y.v;
                        }
                    }
                }
            }
            return 0;
        }

        operator bool() const {
            return eventTasks.len || tasks.len;
        }

    protected:
        XX_FORCE_INLINE ptrdiff_t Resume(int i, Tuple& tuple) {
            auto& task = std::get<3>(tuple);
            if (task.Resume()) {
                eventTasks.SwapRemoveAt(i);  // done
            } else {
                auto& y = task.coro.promise().y;
                if (y.p) {           // renew
                    std::get<0>(tuple) = y.v;
                    std::get<1>(tuple) = y.p;
                    std::get<2>(tuple) = NowSteadyEpochSeconds() + timeoutSecs;
                } else {
                    if (y.v) return y.v;   // yield error number ( != 0 )
                    tasks.Emplace(std::move(task));      // yield 0
                    eventTasks.SwapRemoveAt(i);
                }
            }
            return 0;
        }
    };
}
