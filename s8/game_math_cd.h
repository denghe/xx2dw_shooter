#include "pch.h"

/*
polygon circle
ref from 
http://www.jeffreythompson.org/collision-detection/poly-circle.php


other doc:

https://www.sevenson.com.au/programming/sat/
https://github.com/sevdanski/SAT_JS/blob/main/src/js/sat.js
https://www.codeproject.com/Articles/15573/2D-Polygon-Collision-Detection
http://www.jeffreythompson.org/collision-detection/table_of_contents.php

http://www.jeffreythompson.org/collision-detection/line-circle.php
https://github.com/williamfiset/Algorithms/blob/master/src/main/java/com/williamfiset/algorithms/geometry/LineSegmentCircleIntersection.js
https://stackoverflow.com/questions/4226356/circle-to-circle-segment-collision

*/

namespace Mathf {

    inline XX_FORCE_INLINE float Dist(float x1, float y1, float x2, float y2) {
        return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    }

    inline XX_FORCE_INLINE bool LinePoint(float x1, float y1, float x2, float y2, float px, float py) {
        float d1 = Dist(px, py, x1, y1);
        float d2 = Dist(px, py, x2, y2);
        float lineLen = Dist(x1, y1, x2, y2);
        return d1 + d2 >= lineLen - 0.1 && d1 + d2 <= lineLen + 0.1;
    }

    inline XX_FORCE_INLINE bool PointCircle(float px, float py, float cx, float cy, float r) {
        float distX = px - cx;
        float distY = py - cy;
        return (distX * distX) + (distY * distY) <= r * r;
    }

    inline bool LineCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r) {
        if (PointCircle(x1, y1, cx, cy, r) || PointCircle(x2, y2, cx, cy, r)) return true;
        float distX = x1 - x2;
        float distY = y1 - y2;
        float dot = (((cx - x1) * (x2 - x1)) + ((cy - y1) * (y2 - y1))) / ((distX * distX) + (distY * distY));
        float closestX = x1 + (dot * (x2 - x1));
        float closestY = y1 + (dot * (y2 - y1));
        if (!LinePoint(x1, y1, x2, y2, closestX, closestY)) return false;
        distX = closestX - cx;
        distY = closestY - cy;
        return (distX * distX) + (distY * distY) <= r * r;
    }

    template<bool vsEndIsFirst = false, typename Vecs>
    bool PolygonPoint(Vecs const& vs, float px, float py) {
        bool collision = false;
        for (int curr = 0, next = 1, e = vsEndIsFirst ? std::size(vs) - 1 : std::size(vs); curr < e; ++curr, ++next) {
            if constexpr (!vsEndIsFirst) {
                if (next == e) next = 0;
            }
            auto& vc = vs[curr];
            auto& vn = vs[next];
            if (((vc.y > py && vn.y < py) || (vc.y < py && vn.y > py)) &&
                (px < (vn.x - vc.x) * (py - vc.y) / (vn.y - vc.y) + vc.x)) {
                collision = !collision;
            }
        }
        return collision;
    }

    template<bool checkInside = true, bool vsEndIsFirst = false, typename Vecs>
    bool PolyCircle(Vecs const& vs, float cx, float cy, float r) {
        for (int curr = 0, next = 1, e = vsEndIsFirst ? std::size(vs) - 1 : std::size(vs); curr < e; ++curr, ++next) {
            if constexpr (!vsEndIsFirst) {
                if (next == e) next = 0;
            }
            if (LineCircle(vs[curr].x, vs[curr].y, vs[next].x, vs[next].y, cx, cy, r)) return true;
        }
        if constexpr (checkInside) {
            if (PolygonPoint<vsEndIsFirst>(vs, cx, cy)) return true;
        }
        return false;
    }

}
