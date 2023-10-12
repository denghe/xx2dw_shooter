#include "pch.h"

int32_t main() {
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
	}, nullptr);
}
GameLooper gLooper;											// global var for easy use


void DragCircle::Init(XY const& pos_, float radius_, int32_t segments) {
    pos = pos_;
    radius = radius_;

    border.FillCirclePoints({ 0,0 }, radius, {}, segments)
        .SetPosition(pos);
}

bool DragCircle::OnMouseDown() {
    offset = gLooper.mouse.pos - pos;
    return offset.x * offset.x + offset.y * offset.y <= radius * radius;
}

void DragCircle::OnMouseMove() {
    auto mp = gLooper.mouse.pos - offset;
    auto d = mp - pos;
    auto dd = d.x * d.x + d.y * d.y;
    if (dd < std::numeric_limits<float>::epsilon()) return;

    border.SetPosition(pos);
    gLooper.shadows.Emplace().Init(border);

    if (dd < speed * speed) {
        pos = mp;
    } else {
        pos += d / std::sqrt(dd) * speed;
    }
}

void DragCircle::Draw() {
    border.SetPosition(pos).SetColor(color).Draw();
}



void Poly::Init() {
    vertsForDraw[0] = { 0, 0 };
    vertsForDraw[1] = { 60, 0 };
    vertsForDraw[2] = { 60, -10 };
    vertsForDraw[3] = { 30, -10 };
    vertsForDraw[4] = { 20, 0 };

    vertsForCalc = vertsForDraw;
    vertsForCalc[0] = { 20, 0 };

    vertsForCalcBak = vertsForCalc;
}

void Poly::Draw() {
    printf("%f\n", scale.y);
    border.SetPointsArray(vertsForDraw).SetRotate(radians).SetScale(scale).Draw();
    //border.SetPointsArray(vertsForCalc).SetScale(scale).Draw();
}

xx::Task<> Poly::MainTask() {
    constexpr float r1 = -M_PI / 2, r2 = M_PI / 2;
    while (true) {
        for (radians = r1; radians < r2; radians += cRadiansIncrease) {
            auto at = AffineTransform::MakePosScaleRadians(pos, scale, radians);
            for (int i = 0; i < std::size(vertsForCalc); ++i) {
                vertsForCalc[i] = at(vertsForCalcBak[i]);
            }
            border.SetPosition(pos).SetRotate(radians);
            gLooper.shadows.Emplace().Init(border);
            co_yield 0;
        }
        scale.y = -scale.y;
        for (radians = r2; radians > r1; radians -= cRadiansIncrease) {
            auto trans = AffineTransform::MakePosScaleRadians(pos, scale, radians);
            for (int i = 0; i < std::size(vertsForCalc); ++i) {
                vertsForCalc[i] = trans(vertsForCalcBak[i]);
            }
            border.SetPosition(pos).SetRotate(radians);
            gLooper.shadows.Emplace().Init(border);
            co_yield 0;
        }
        scale.y = -scale.y;
    }
}


void Shadow::Init(LineStrip const& border_) {
    border = border_;
}

xx::Task<> Shadow::MainTask() {
    while (alpha > 0) {
        alpha -= cAlphaDecrease;
        co_yield 0;
    }
}

void Shadow::Draw() {
    border.SetColorAf(alpha).Draw();
}



EM_BOOL GameLooper::OnMouseDown(EmscriptenMouseEvent const& e) {
    mouse.btnStates[e.button] = true;	// mouse left btn == 0, right btn == 2
    if (dc.OnMouseDown()) {
        mouseFocus = &dc;
    }
    return EM_TRUE;
}

EM_BOOL GameLooper::OnMouseMove(EmscriptenMouseEvent const& e) {
    mouse.pos = { (float)e.targetX - this->windowSize_2.x, this->windowSize.y - (float)e.targetY - this->windowSize_2.y };
    if (mouseFocus) {
        mouseFocus->OnMouseMove();
    }
    return EM_TRUE;
}

EM_BOOL GameLooper::OnMouseUp(EmscriptenMouseEvent const& e) {
    mouse.btnStates[e.button] = false;
    mouseFocus = {};
    return EM_TRUE;
}



void GameLooper::Update() {
    poly.mainTask();

    if (mouseFocus) {
        mouseFocus->OnMouseMove();
    }

    shadows.Foreach([](Shadow& o)->bool {
        return o.mainTask.Resume();
    });

    if (Calc::Intersects::PolyCircle(poly.vertsForCalc, dc.pos.x, dc.pos.y, dc.radius)) {
        dc.color = { 255, 0, 0, 255 };
    } else {
        dc.color = { 255, 255, 0, 255 };
    }
}

xx::Task<> GameLooper::MainTask() {
    dc.Init({ 40, 0 }, 10, 16);
    poly.Init();

    //// algorithm compare: 1 circle faster than 4 poly 500 times
    //{
    //    int counter = 0;
    //    auto secs = xx::NowSteadyEpochSeconds();
    //    for (int i = 0; i < 10000000; ++i) {
    //        if (Calc::Intersects::PolyCircle(poly.vertsForCalc, dc.pos.x, dc.pos.y, dc.radius)) counter++;
    //    }
    //    log = "PolyCircle secs = "s + std::to_string(xx::NowSteadyEpochSeconds(secs))
    //        + " counter = " + std::to_string(counter);

    //    counter = 0;
    //    for (int i = 0; i < 10000000; ++i) {
    //        bool cross{};
    //        for (auto& pos : poly.vertsForCalc) {
    //            auto d = dc.pos - pos;
    //            if (d.x * d.x + d.y * d.y < (dc.radius + dc.radius) * (dc.radius + dc.radius)) {
    //                cross = true;
    //                break;
    //            }
    //        }
    //        if (cross) counter++;
    //    }
    //    log += " 5 circle secs = "s + std::to_string(xx::NowSteadyEpochSeconds(secs))
    //        + " counter = " + std::to_string(counter);
    //}

	co_return;
}

void GameLooper::Draw() {
    xx_assert(shadows.cap <= shadowsCap);   // avoid reserve

    poly.Draw();

    shadows.Foreach([](Shadow& o)->void {
        o.Draw();
    });

    dc.Draw();

    if (!log.empty()) {
        ctcDefault.Draw({ -gEngine->windowSize_2.x, gEngine->windowSize_2.y - ctcDefault.canvasHeight_2 }, log);
    }
}
