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
    gLooper.shadows.Emplace().Init();
    if (dd < speed * speed) {
        pos = mp;
    } else {
        pos += d / std::sqrt(dd) * speed;
    }
}

void DragCircle::Draw() {
    border.SetPosition(pos).SetColor(color).Draw();
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
    border.SetPointsArray(vertsForDraw).SetRotate(radians).Draw();
    //border.SetPointsArray(vertsForCalc).Draw();
}

xx::Task<> Poly::MainTask() {
    while (true) {
        radians += cRadiansIncrease;
        auto at = AffineTransform::MakePosScaleRadians(pos, scale, radians);
        for (int i = 0; i < std::size(vertsForCalc); ++i) {
            vertsForCalc[i] = at.Apply(vertsForCalcBak[i]);
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

    if (CheckIntersects::PolyCircle(poly.vertsForCalc, dc.pos.x, dc.pos.y, dc.radius)) {
        dc.color = { 255, 0, 0, 255 };
        log = "intersects";
    } else {
        dc.color = { 255, 255, 0, 255 };
        log = "";
    }
}

xx::Task<> GameLooper::MainTask() {
    ctc72.Init();
    dc.Init({ 40, 0 }, 10, 16);
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
