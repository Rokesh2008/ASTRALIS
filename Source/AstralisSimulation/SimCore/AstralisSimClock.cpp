#include "AstralisSimClock.h"

FAstralisSimClock::FAstralisSimClock()
	: SimTime_s(0.0)
	, SimDelta_s(0.0)
	, MissionElapsedTime_s(0.0)
	, TimeScale(1.0)
	, bPaused(false)
{
}

void FAstralisSimClock::SetTimeScale(double InTimeScale)
{
	TimeScale = FMath::Clamp(InTimeScale, 0.0, MaxTimeScale);
}

void FAstralisSimClock::Advance(double RealDeltaSeconds)
{
	if (bPaused)
	{
		SimDelta_s = 0.0;
		return;
	}

	SimDelta_s = RealDeltaSeconds * TimeScale;
	SimTime_s += SimDelta_s;
	MissionElapsedTime_s += SimDelta_s;
}
