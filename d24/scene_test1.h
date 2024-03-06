#pragma once
#include <game_looper.h>
#include <game_effect_number.h>

// todo: very fast Save + Load for time rollback

// weak ptr likely. need switch case type + iv check item
struct Pointer {
	int32_t typeId{};
	LDLIV iv;

	// return nullptr mean item is disposed
	template<typename T>
	T* Get();

	bool Exists() const;
};

struct PointerInt : Pointer {
	int value{};
};

namespace xx {
	template<>
	struct IsPod<Pointer, void> : std::true_type {};
	template<>
	struct IsPod<PointerInt, void> : std::true_type {};
}

struct SceneItem {
	int32_t typeId{};		// fill by XxxxxInit
	int32_t lineNumber{};
	LDLIV iv;				// fill by XxxxxInit
	XY pos{}, scale{};
	float radius{}, radians{}, frameIndex{}, speed{};
};

struct Bullet : SceneItem {
	static constexpr int32_t cTypeId{ 1 };

	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cScale{ 1.f };
	static constexpr float cRadius{ 16.f * cScale };
	static constexpr xx::FromTo cSpeed{ 5.f, 20.f };
	static constexpr float cTimeSpan{ 2 };
	static constexpr xx::FromTo cMaxHitCount{ 1, 3 };
	static constexpr int cHitDelayFrames{ int(gDesign.fps / 10) };

	// ...pod fields here
	float e{};
	int i{};
	XY inc;
	int maxHitCount{};
	float tailRatio{};
	xx::Listi32<PointerInt> hitBlackList;	// offset == 0x38	( sync with memory layout )

	void Init();
	void Init(float radians_);
	int UpdateCore();
	bool Update();			// return true mean dispose
	void Draw();
	void Load(xx::Data& d);	// placement new
	void Save(xx::Data& d);
};

struct BigMonster : SceneItem {
	static constexpr int cTypeId{ 2 };

	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cScale{ 10.f };
	static constexpr float cRadius{ 16.f * cScale };

	void Init(XY const& pos_);
	bool Update();			// return true mean dispose
	void Draw();
	void Load(xx::Data& d);	// placement new
	void Save(xx::Data& d);
};

struct SceneTest1 : Scene {
	inline static SceneTest1* instance{};			// init by Init()

	xx::Shared<Node> rootNode;

	// memcpy begin
	Camera camera;
	int numBulletGenerateByEveryFrame{ 1 };
	int frameNumber{};						// does not use gLooper's frameNumber. for save load
	Rnd rnd;
	// memcpy end

	LDL<Bullet> bullets;
	LDL<BigMonster> bigMonsters;
	EffectNumberManager enm;
	// ...

	Bullet& MakeBullet();
	BigMonster& MakeBigMonster();
	// ...

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;

	//xx::Queue<xx::Data> memLog;		// for time rollback
	xx::Data savedData;
	void Save();
	void Load();
};

template<typename T>
T* Pointer::Get() {
	if constexpr (std::is_same_v<T, Bullet>) {
		assert(Bullet::cTypeId == typeId);
		auto& os = SceneTest1::instance->bullets;
		if (os.Exists(iv)) return &os.At(iv);
	}
	// ... if constexpr ( .....

	return nullptr;
}

inline bool Pointer::Exists() const {
	switch (typeId) {
	case BigMonster::cTypeId:
		return SceneTest1::instance->bigMonsters.Exists(iv);
	}
	assert(false);
	return false;
}
