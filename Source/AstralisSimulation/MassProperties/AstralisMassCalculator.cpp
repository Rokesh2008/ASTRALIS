#include "AstralisMassCalculator.h"

double FAstralisMassCalculator::TotalMass(const TArray<FComponentMassEntry>& Components)
{
	double Total = 0.0;
	for (const auto& C : Components)
	{
		Total += C.Mass_kg;
	}
	return Total;
}

FVector3d FAstralisMassCalculator::CenterOfMass(const TArray<FComponentMassEntry>& Components)
{
	double TotalM = 0.0;
	FVector3d Moment = FVector3d::ZeroVector;
	for (const auto& C : Components)
	{
		TotalM += C.Mass_kg;
		Moment += C.Mass_kg * C.Position_m;
	}
	if (TotalM <= 0.0)
	{
		return FVector3d::ZeroVector;
	}
	return Moment / TotalM;
}

void FAstralisMassCalculator::InertiaTensor(
	const TArray<FComponentMassEntry>& Components,
	const FVector3d& CoM,
	double OutTensor[6])
{
	// Initialize: [Ixx, Iyy, Izz, Ixy, Ixz, Iyz]
	for (int i = 0; i < 6; ++i) OutTensor[i] = 0.0;

	for (const auto& C : Components)
	{
		const FVector3d D = C.Position_m - CoM;
		const double m = C.Mass_kg;
		const double dx = D.X, dy = D.Y, dz = D.Z;
		const double d2 = dx*dx + dy*dy + dz*dz;

		// Parallel-axis theorem: I_total = I_local + m*(d^2*E - d*d^T)
		OutTensor[0] += C.LocalInertia[0] + m * (d2 - dx*dx); // Ixx
		OutTensor[1] += C.LocalInertia[1] + m * (d2 - dy*dy); // Iyy
		OutTensor[2] += C.LocalInertia[2] + m * (d2 - dz*dz); // Izz
		OutTensor[3] += -m * dx * dy;                           // Ixy
		OutTensor[4] += -m * dx * dz;                           // Ixz
		OutTensor[5] += -m * dy * dz;                           // Iyz
	}
}
