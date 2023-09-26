#include "pch.h"

int32_t main() {
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
	}, nullptr);
}
GameLooper gLooper;											// global var for easy use

void GameLooper::Init() {
    windowWidth = gDesign.width;
    windowHeight = gDesign.height;
}

/*
todo: polygon circle
docs here:

https://www.sevenson.com.au/programming/sat/
https://github.com/sevdanski/SAT_JS/blob/main/src/js/sat.js
https://www.codeproject.com/Articles/15573/2D-Polygon-Collision-Detection
http://www.jeffreythompson.org/collision-detection/table_of_contents.php

http://www.jeffreythompson.org/collision-detection/line-circle.php
https://github.com/williamfiset/Algorithms/blob/master/src/main/java/com/williamfiset/algorithms/geometry/LineSegmentCircleIntersection.js
https://stackoverflow.com/questions/4226356/circle-to-circle-segment-collision

*/






float Dist(float x1, float y1, float x2, float y2) {
    return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

// LINE/POINT
bool LinePoint(float x1, float y1, float x2, float y2, float px, float py) {

    // get distance from the point to the two ends of the line
    float d1 = Dist(px, py, x1, y1);
    float d2 = Dist(px, py, x2, y2);

    // get the length of the line
    float lineLen = Dist(x1, y1, x2, y2);

    // since floats are so minutely accurate, add
    // a little buffer zone that will give collision
    float buffer = 0.1;    // higher # = less accurate

    // if the two distances are equal to the line's length, the
    // point is on the line!
    // note we use the buffer here to give a range, rather than one #
    if (d1 + d2 >= lineLen - buffer && d1 + d2 <= lineLen + buffer) {
        return true;
    }
    return false;
}

// POINT/CIRCLE
bool PointCircle(float px, float py, float cx, float cy, float r) {

    // get distance between the point and circle's center
    // using the Pythagorean Theorem
    float distX = px - cx;
    float distY = py - cy;
    float distance = std::sqrt((distX * distX) + (distY * distY));

    // if the distance is less than the circle's 
    // radius the point is inside!
    if (distance <= r) {
        return true;
    }
    return false;
}

// LINE/CIRCLE
bool LineCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r) {

    // is either end INSIDE the circle?
    // if so, return true immediately
    bool inside1 = PointCircle(x1, y1, cx, cy, r);
    bool inside2 = PointCircle(x2, y2, cx, cy, r);
    if (inside1 || inside2) return true;

    // get length of the line
    float distX = x1 - x2;
    float distY = y1 - y2;
    float len = std::sqrt((distX * distX) + (distY * distY));

    // get dot product of the line and circle
    float dot = (((cx - x1) * (x2 - x1)) + ((cy - y1) * (y2 - y1))) / pow(len, 2);

    // find the closest point on the line
    float closestX = x1 + (dot * (x2 - x1));
    float closestY = y1 + (dot * (y2 - y1));

    // is this point actually on the line segment?
    // if so keep going, but if not, return false
    bool onSegment = LinePoint(x1, y1, x2, y2, closestX, closestY);
    if (!onSegment) return false;

    //// optionally, draw a circle at the closest point on the line
    //fill(255, 0, 0);
    //noStroke();
    //ellipse(closestX, closestY, 20, 20);

    // get distance to closest point
    distX = closestX - cx;
    distY = closestY - cy;
    float distance = std::sqrt((distX * distX) + (distY * distY));

    // is the circle on the line?
    if (distance <= r) {
        return true;
    }
    return false;
}


// POLYGON/POINT
// only needed if you're going to check if the circle is INSIDE the polygon
template<typename Vecs>
bool PolygonPoint(Vecs const& vertices, float px, float py) {
    bool collision = false;

    // go through each of the vertices, plus the next vertex in the list
    int next = 0;
    for (int current = 0; current < vertices.size(); current++) {

        // get next vertex in list
        // if we've hit the end, wrap around to 0
        next = current + 1;
        if (next == vertices.size()) next = 0;

        // get the PVectors at our current position
        // this makes our if statement a little cleaner
        auto vc = vertices[current];    // c for "current"
        auto vn = vertices[next];       // n for "next"

        // compare position, flip 'collision' variable back and forth
        if (((vc.y > py && vn.y < py) || (vc.y < py && vn.y > py)) &&
            (px < (vn.x - vc.x) * (py - vc.y) / (vn.y - vc.y) + vc.x)) {
            collision = !collision;
        }
    }
    return collision;
}

// POLYGON/CIRCLE
template<typename Vecs, bool enableCenterInsideCheck = true>
bool PolyCircle(Vecs const& vertices, float cx, float cy, float r) {

    // go through each of the vertices, plus the next vertex in the list
    int next = 0;
    for (int current = 0; current < vertices.size(); current++) {

        // get next vertex in list
        // if we've hit the end, wrap around to 0
        next = current + 1;
        if (next == vertices.length) next = 0;

        // get the PVectors at our current position
        // this makes our if statement a little cleaner
        auto vc = vertices[current];    // c for "current"
        auto vn = vertices[next];       // n for "next"

        // check for collision between the circle and a line formed
        // between the two vertices
        bool collision = LineCircle(vc.x, vc.y, vn.x, vn.y, cx, cy, r);
        if (collision) return true;
    }


    // the above algorithm only checks if the circle is touching
    // the edges of the polygon – in most cases this is enough, but
    // you can un-comment the following code to also test if the
    // center of the circle is inside the polygon
    if constexpr (enableCenterInsideCheck) {
        bool centerInside = PolygonPoint(vertices, cx,cy);
        if (centerInside) return true;
    }

    // otherwise, after all that, return false
    return false;
}



//float cx = 0;    // position of the circle
//float cy = 0;
//float r = 30;   // circle's radius
//
//// array of PVectors, one for each vertex in the polygon
//PVector[] vertices = new PVector[4];
//
//
//void setup() {
//    size(600, 400);
//    noStroke();
//
//    // set position of the vertices (here a trapezoid)
//    vertices[0] = new PVector(200, 100);
//    vertices[1] = new PVector(400, 100);
//    vertices[2] = new PVector(350, 300);
//    vertices[3] = new PVector(250, 300);
//}
//
//
//void draw() {
//    background(255);
//
//    // update circle to mouse coordinates
//    cx = mouseX;
//    cy = mouseY;
//
//    // check for collision
//    // if hit, change fill color
//    bool hit = PolyCircle(vertices, cx, cy, r);
//    if (hit) fill(255, 150, 0);
//    else fill(0, 150, 255);
//
//    // draw the polygon using beginShape()
//    noStroke();
//    beginShape();
//    for (PVector v : vertices) {
//        vertex(v.x, v.y);
//    }
//    endShape();
//
//    // draw the circle
//    fill(0, 150);
//    ellipse(cx, cy, r * 2, r * 2);
//}
//












bool DragCircle::HandleMouseDown(LT& L) {
    auto d = pos - L.downPos;
    if (d.x * d.x + d.y * d.y < radiusPow2) {
        dxy = d;
        gLooper.draggingC = this;
        return true;
    }
    return false;
}
int DragCircle::HandleMouseMove(LT& L) {
    return 0;
}
void DragCircle::HandleMouseUp(LT& L) {
    gLooper.draggingC = {};
}

void DragCircle::Init(XY const& pos, float radius, int32_t segments) {
    this->pos = pos;
    this->radius = radius;
    this->radiusPow2 = radius * radius;

    border.FillCirclePoints({ 0,0 }, radius, {}, segments)
        .SetColor({ 255, 255, 0, 255 })
        .SetPosition(pos);
}

void DragCircle::Draw() {
    border.Draw();
}

void Poly::Init() {
    vertices[0] = { 200, 100 };
    vertices[1] = { 400, 100 };
    vertices[2] = { 350, 300 };
    vertices[3] = { 250, 300 };
    border.SetPoints({ vertices[0],vertices[1],vertices[2],vertices[3] });
}

void Poly::Draw() {
    border.Draw();
}

xx::Task<> GameLooper::MainTask() {

	co_return;
}
