#pragma once

#include "CoreMinimal.h"

struct ASTRALISSIMULATION_API FComponentMassEntry
{
	double Mass_kg = 0.0;
	FVector3d Position_m = FVector3d::ZeroVector;
	// Inertia about component's own CoM: [Ixx, Iyy, Izz]
	double LocalInertia[3] = {0.0, 0.0, 0.0};
};

// Calculates composite mass properties from an array of components.
// Uses parallel-axis theorem for inertia tensor.
class ASTRALISSIMULATION_API FAstralisMassCalculator
{
public:
	static double TotalMass(const TArray<FComponentMassEntry>& Components);

	static FVector3d CenterOfMass(const TArray<FComponentMassEntry>& Components);

	// Returns symmetric inertia tensor [Ixx, Iyy, Izz, Ixy, Ixz, Iyz]
	// about the composite center of mass.
	static void InertiaTensor(
		const TArray<FComponentMassEntry>& Components,
		const FVector3d& CoM,
		double OutTensor[6]);
};
