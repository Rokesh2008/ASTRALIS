#include "AstralisCoordinateService.h"

FVector3d FAstralisCoordinateService::RenderingOrigin_m = FVector3d::ZeroVector;

FVector FAstralisCoordinateService::SIToUE(const FVector3d& Position_m)
{
	const FVector3d Relative = Position_m - RenderingOrigin_m;
	return FVector(
		Relative.X * MetresToCm,
		Relative.Y * MetresToCm,
		Relative.Z * MetresToCm
	);
}

FVector3d FAstralisCoordinateService::UEToSI(const FVector& Position_cm)
{
	return FVector3d(
		Position_cm.X * CmToMetres,
		Position_cm.Y * CmToMetres,
		Position_cm.Z * CmToMetres
	) + RenderingOrigin_m;
}

void FAstralisCoordinateService::SetRenderingOrigin(const FVector3d& Origin_m)
{
	RenderingOrigin_m = Origin_m;
}

FVector3d FAstralisCoordinateService::GetRenderingOrigin()
{
	return RenderingOrigin_m;
}
