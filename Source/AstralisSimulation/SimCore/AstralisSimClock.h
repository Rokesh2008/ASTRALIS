#pragma once

#include "CoreMinimal.h"

// Manages three independent clocks: rendering, simulation, and mission time.
// Simulation advances at a configurable rate independent of frame rate.
class ASTRALISSIMULATION_API FAstralisSimClock
{
public:
	FAstralisSimClock();

	void SetTimeScale(double InTimeScale);
	double GetTimeScale() const { return TimeScale; }

	void Advance(double RealDeltaSeconds);

	double GetSimTime() const { return SimTime_s; }
	double GetSimDelta() const { return SimDelta_s; }
	double GetMissionElapsedTime() const { return MissionElapsedTime_s; }

	void SetSimTime(double InSimTime) { SimTime_s = InSimTime; }
	void ResetMissionClock() { MissionElapsedTime_s = 0.0; }

	bool IsPaused() const { return bPaused; }
	void SetPaused(bool bInPaused) { bPaused = bInPaused; }

	static constexpr double MaxTimeScale = 10000.0;

private:
	double SimTime_s;
	double SimDelta_s;
	double MissionElapsedTime_s;
	double TimeScale;
	bool bPaused;
};
