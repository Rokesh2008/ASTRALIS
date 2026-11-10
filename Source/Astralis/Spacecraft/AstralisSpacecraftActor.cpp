#include "AstralisSpacecraftActor.h"

AAstralisSpacecraftActor::AAstralisSpacecraftActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SpacecraftRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SpacecraftRoot"));
	SetRootComponent(SpacecraftRoot);
}

void AAstralisSpacecraftActor::RecalculateMassProperties()
{
	MassState.TotalMass_kg = FAstralisMassCalculator::TotalMass(ComponentEntries);
	MassState.DryMass_kg = MassState.TotalMass_kg;
	MassState.CenterOfMass_m = FAstralisMassCalculator::CenterOfMass(ComponentEntries);
	FAstralisMassCalculator::InertiaTensor(
		ComponentEntries, MassState.CenterOfMass_m, MassState.InertiaTensor);
}
