#pragma once
#include "engine_quad.h"

namespace xx {

	// code ref from HGE

	struct ParticleItem {
		XY pos, velocity;
		float gravity, radialAccel, tangentialAccel, spin, spinDelta, size, sizeDelta, age, terminalAge;
		RGBA color, colorDelta; // + alpha
	};

	struct ParticleConfig {
		Quad sprite;

		int emission;	// 0 ~ 1000
		float lifetime;	// 0 ~ 10s, -1.0f

		std::pair<float, float> particleLife;	// 0 ~ 5

		float direction;	// 0 ~ 2PI
		float spread;	// 0 ~ 2PI
		int relative;	// 0, 1

		std::pair<float, float> speed;	// -300 ~ 300
		std::pair<float, float> gravity;	// -900 ~ 900
		std::pair<float, float> radialAccel;	// -900 ~ 900
		std::pair<float, float> tangentialAccel;	// -900 ~ 900
		std::pair<float, float> size;	// 1 ~ 100
		float sizeVar;	// 0 ~ 1

		std::pair<float, float> spin;	// -50, 50
		float spinVar;	// 0 ~ 1

		std::pair<RGBA, RGBA> color; // + alpha
		float colorVar, alphaVar;	// 0 ~ 1
	};

	struct Particle {

        xx::Shared<ParticleConfig> cfg;
        std::array<uint32_t, 3> blendFuncs;

        xx::ListLink<ParticleItem, int> particles;

        float age, emissionResidue, scale;
        XY prevPos, pos, rootPos;

		void Init(xx::Shared<ParticleConfig> cfg_, size_t const& cap = 1000, std::array<uint32_t, 3> blendFuncs_ = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD }) {
			particles.Clear<true>();
			particles.Reserve(cap);
			blendFuncs = blendFuncs_;
			cfg = std::move(cfg_);

			pos = prevPos = rootPos = {};
			scale = 1.0f;

			emissionResidue = {};
			age = -2.0;
		}

		void Update(float delta) {
            int i, e;
            float ang;

            if (age >= 0) {
                age += delta;
                if (age >= cfg->lifetime) {
                    age = -2.0f;
                }
            }

            // update all alive particles

            int prev = -1, next{};
            for (auto idx = particles.head; idx != -1;) {
                auto& p = particles[idx];

                p.age += delta;
                if (p.age >= p.terminalAge) {
                    next = particles.Remove(idx, prev);
                } else {
                    next = particles.Next(idx);
                    prev = idx;
                    {
                        XY vecAccel = p.pos - pos;
                        vecAccel.Normalize();
                        XY vecAccel2 = vecAccel;
                        vecAccel *= p.radialAccel;

                        // vecAccel2.Rotate(M_PI_2);
                        // the following is faster
                        ang = vecAccel2.x;
                        vecAccel2.x = -vecAccel2.y;
                        vecAccel2.y = ang;

                        vecAccel2 *= p.tangentialAccel;
                        p.velocity += (vecAccel + vecAccel2) * delta;
                        p.velocity.y += p.gravity * delta;

                        p.pos += p.velocity * delta;

                        p.spin += p.spinDelta * delta;
                        p.size += p.sizeDelta * delta;
                        p.color += p.colorDelta * delta;
                    }
                }
                idx = next;
            }

            // generate new particles

            if (age != -2.0f) {
                const auto particlesNeeded = cfg->emission * delta + emissionResidue;
                int n = (uint32_t)particlesNeeded;
                emissionResidue = particlesNeeded - n;
                auto&& rnd = gEngine->rnd;

                auto left = particles.Left();
                if (n > left) {
                    n = left;
                }
                for (i = 0; i < n; i++) {
                    auto&& p = particles.Emplace();

                    p.age = 0.0f;
                    p.terminalAge = rnd.Next(cfg->particleLife);

                    p.pos = prevPos + (pos - prevPos) * rnd.Next(0.f, 1.f) + XY{ rnd.Next(-2.f, 2.f), rnd.Next(-2.f, 2.f) };

                    ang = cfg->direction - gPI_2 + rnd.Next(0.f, cfg->spread) - cfg->spread / 2.0f;
                    if (cfg->relative) {
                        auto d = prevPos - pos;
                        ang += std::atan2(d.y, d.x) + gPI_2;
                    }
                    p.velocity = XY{ std::cos(ang), std::sin(ang) } *rnd.Next(cfg->speed);

                    p.gravity = rnd.Next(cfg->gravity);
                    p.radialAccel = rnd.Next(cfg->radialAccel);
                    p.tangentialAccel = rnd.Next(cfg->tangentialAccel);

                    p.size = rnd.Next(cfg->size.first, cfg->size.first + (cfg->size.second - cfg->size.first) * cfg->sizeVar);
                    p.sizeDelta = (cfg->size.second - p.size) / p.terminalAge;

                    p.spin = rnd.Next(cfg->spin.first, cfg->spin.first + (cfg->spin.second - cfg->spin.first) * cfg->spinVar);
                    p.spinDelta = (cfg->spin.second - p.spin) / p.terminalAge;

                    p.color.r = rnd.Next2(cfg->color.first.r, cfg->color.first.r + (cfg->color.second.r - cfg->color.first.r) * cfg->colorVar);
                    p.color.g = rnd.Next2(cfg->color.first.g, cfg->color.first.g + (cfg->color.second.g - cfg->color.first.g) * cfg->colorVar);
                    p.color.b = rnd.Next2(cfg->color.first.b, cfg->color.first.b + (cfg->color.second.b - cfg->color.first.b) * cfg->colorVar);
                    p.color.a = rnd.Next2(cfg->color.first.a, cfg->color.first.a + (cfg->color.second.a - cfg->color.first.a) * cfg->alphaVar);

                    p.colorDelta = (cfg->color.second - p.color) / p.terminalAge;
                }
            }

            prevPos = pos;
		}

        void FireAt(XY const& xy) {
            Stop();
            MoveTo(xy);
            Fire();
        }

        void Fire() {
            if (cfg->lifetime == -1.0f) {
                age = -1.0f;
            } else {
                age = 0.0f;
            }
        }

        void MoveTo(XY const& xy, bool moveParticles = false) {
            if (moveParticles) {
                const auto d = xy - pos;

                for (auto idx = particles.head; idx != -1; idx = particles.Next(idx)) {
                    particles[idx].pos += d;
                }

                prevPos += d;
            } else {
                if (age == -2.0) {
                    prevPos = xy;
                } else {
                    prevPos = pos;
                }
            }

            pos = xy;
        }

        void Stop(bool killParticles = false) {
            age = -2.0f;
            if (killParticles) {
                particles.Clear();
            }
        }
        bool Empty() const {
            return particles.Count() == 0;
        }

        void Draw() const {
            auto& e = EngineBase1::Instance();
            e.GLBlendFunc<true>(blendFuncs);

            auto bak = cfg->sprite.color;

            for (auto idx = particles.head; idx != -1; idx = particles.Next(idx)) {
                auto&& par = particles[idx];

                if (cfg->color.first.r < 0) {
                    cfg->sprite.color.a = par.color.a * 255;
                } else {
                    cfg->sprite.color = par.color;
                }
                cfg->sprite.SetPosition(par.pos * scale + rootPos)
                    .SetScale(par.size * scale)
                    .SetRotate(par.spin * par.age)
                    .Draw();
            }

            cfg->sprite.SetColor(bak);

            e.GLBlendFuncRestore();
        }

	};
}
