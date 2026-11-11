#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AstralisSimulation/SimCore/AstralisSimState.h"
#include "AstralisSimulation/MassProperties/AstralisMassCalculator.h"
#include "AstralisSpacecraftActor.generated.h"

UCLASS()
class ASTRALIS_API AAstralisSpacecraftActor : public AActor
{
	GENERATED_BODY()

public:
	AAstralisSpacecraftActor();

	// Root scene component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SpacecraftRoot;

	// Engineering state (SI, double precision — authoritative)
	FSpacecraftMassState MassState;

	// Component entries for mass calculation
	TArray<FComponentMassEntry> ComponentEntries;

	// Recalculate all mass properties from installed components
	UFUNCTION(BlueprintCallable, Category = "Engineering")
	void RecalculateMassProperties();

	// Get total mass in kg
	UFUNCTION(BlueprintCallable, Category = "Engineering")
	double GetTotalMass() const { return MassState.TotalMass_kg; }

	// Get center of mass in SI metres
	UFUNCTION(BlueprintCallable, Category = "Engineering")
	FVector GetCenterOfMass_SI() const { return FVector(MassState.CenterOfMass_m); }
};
