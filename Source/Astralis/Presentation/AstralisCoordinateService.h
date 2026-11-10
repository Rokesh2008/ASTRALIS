#pragma once

#include "CoreMinimal.h"

// Converts between SI engineering coordinates (metres, double precision)
// and Unreal Engine visual coordinates (centimetres, single precision).
//
// Engineering frame: right-handed, Z-up, metres, double precision.
// UE frame: left-handed, Z-up, centimetres, single precision.
//
// For orbital-scale distances, an origin-rebasing offset is applied
// so the camera-local region stays within float precision.
class ASTRALIS_API FAstralisCoordinateService
{
public:
	static constexpr double MetresToCm = 100.0;
	static constexpr double CmToMetres = 0.01;

	// Convert SI position (metres) to UE world position (centimetres),
	// applying the current rendering origin offset.
	static FVector SIToUE(const FVector3d& Position_m);

	// Convert UE world position back to SI.
	static FVector3d UEToSI(const FVector& Position_cm);

	// Set the rendering origin in SI coordinates.
	// All SIToUE conversions subtract this before scaling.
	static void SetRenderingOrigin(const FVector3d& Origin_m);
	static FVector3d GetRenderingOrigin();

private:
	static FVector3d RenderingOrigin_m;
};
