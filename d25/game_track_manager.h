#pragma once
#include <engine_prims.h>
#include <engine_curvemovepath.h>

struct TrackManager {
	static constexpr float totalWidth{ 32 * 3 };

	static constexpr int trackCount{ 25 };
	static constexpr float pointDistance{ 0.1f };
	static constexpr float trackMargin{ totalWidth / trackCount };

	xx::Listi32<xx::Listi32<XY>> tracks;		// todo: combine to 1 array ??
	xx::Listi32<float> radianss;

	int32_t GetPointCount() const {
		return radianss.len;
	}

	XY GetPoint(int32_t trackIndex, int32_t pointIndex) const {
		return tracks[trackIndex][pointIndex];
	}

	float GetRadians(int32_t pointIndex) const {
		return radianss[pointIndex];
	}

	void Init(std::vector<CurvePoint> const& passingPoints, bool loop = false) {
		// fill path
		MovePath mp;
		mp.FillCurve(loop, passingPoints);

		// path to cache
		MovePathCache mpc;
		mpc.Init(mp, pointDistance);
		auto count = (int)mpc.points.size();
		radianss.Resize(count);

		// create tracks container
		tracks.Resize(trackCount);
		for (auto& track : tracks) {
			track.Resize(count);
		}

		// fill all tracks & radianss
		for (int i = 0; i < count; ++i) {
			auto& p = mpc.points[i];
			radianss[i] = p.radians;
			XY step{
				std::cos(p.radians - gPI / 2) * trackMargin,
				std::sin(p.radians - gPI / 2) * trackMargin
			};
			for (int j = 0; j < trackCount; j++) {
				tracks[j][i] = p.pos + step * (j - trackCount / 2);
			}
		}
	}
};
