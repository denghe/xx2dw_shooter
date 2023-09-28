#include "pch.h"
#include "game_math_cd.h"

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
        .SetColor({ 255, 255, 0, 255 })
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
    gLooper.shadows.Emplace().Init();
    if (dd < speed * speed) {
        pos = mp;
    } else {
        pos += d / std::sqrt(dd) * speed;
    }
}

void DragCircle::Draw() {
    int hit{};
    //for (int i = 0; i < 1000000; ++i) {
        hit += Mathf::PolyCircle(gLooper.poly.verts, pos.x, pos.y, radius) ? 1 : 0;
    //}
    gLooper.log = std::to_string(hit);
    border.SetPosition(pos).Draw();
}



void DragCircleShadow::Init() {
    border = gLooper.dc.border;
    border.SetPosition(gLooper.dc.pos);
}

xx::Task<> DragCircleShadow::MainTask() {
    while (alpha > 0) {
        alpha -= cAlphaDecrease;
        co_yield 0;
    }
}

void DragCircleShadow::Draw() {
    border.SetColorAf(alpha).Draw();
}



void Poly::Init() {
    bak[0] = { -100, 0 };
    bak[1] = { -75, 75 };
    bak[2] = { 0, 100 };
    bak[3] = { 75, 75 };
    bak[4] = { 100, 0 };
    bak[5] = { 75, -75 };
    bak[6] = { 0, -100 };
    bak[7] = { -75, -75 };
    bak[8] = { -100, 0 };  // loop point
    verts = bak;
}

void Poly::Draw() {
    border.SetPointsArray(verts).Draw();
}

xx::Task<> Poly::MainTask() {
    while (true) {
        radians += 0.001;
        auto at = AffineTransform::MakePosScaleRadians(pos, scale, radians);
        for (int i = 0; i < std::size(verts); ++i) {
            verts[i] = at.Apply(bak[i]);
        }
        co_yield 0;
    }
}



EM_BOOL GameLooper::OnMouseDown(EmscriptenMouseEvent const& e) {
    mouse.btnStates[e.button] = true;	// mouse left btn == 0, right btn == 2
    if (dc.OnMouseDown()) {
        mouseFocus = &dc;
    }
    return EM_TRUE;
}

EM_BOOL GameLooper::OnMouseMove(EmscriptenMouseEvent const& e) {
    mouse.pos = { (float)e.targetX - this->windowWidth_2, this->windowHeight - (float)e.targetY - this->windowHeight_2 };
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



void GameLooper::Init() {
    windowWidth = gDesign.width;
    windowHeight = gDesign.height;
}

void GameLooper::Update() {
    fv.Update();

    poly.mainTask();

    if (mouseFocus) {
        mouseFocus->OnMouseMove();
    }

    shadows.Foreach([](DragCircleShadow& o)->bool {
        return o.mainTask.Resume();
    });
}

xx::Task<> GameLooper::MainTask() {
    ctc72.Init();
    dc.Init({}, 30, 20);
    poly.Init();
	co_return;
}

void GameLooper::Draw() {
    xx_assert(shadows.cap <= shadowsCap);   // avoid reserve

    poly.Draw();

    shadows.Foreach([](DragCircleShadow& o)->void {
        o.Draw();
    });

    dc.Draw();

    if (!log.empty()) {
        ctc72.Draw({ -gEngine->windowWidth_2, gEngine->windowHeight_2 - ctc72.canvasHeight_2 }, log);
    }

    fv.Draw(ctc72);
}
