#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AstralisSimulation/SimCore/AstralisSimEngine.h"
#include "AstralisOrbitSceneActor.generated.h"

class UStaticMeshComponent;
class USplineComponent;

UCLASS()
class ASTRALIS_API AAstralisOrbitSceneActor : public AActor
{
	GENERATED_BODY()

public:
	AAstralisOrbitSceneActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void InitializeFromAssembly(
		const TArray<FComponentMassEntry>& Components,
		double PropellantMass_kg);

	FAstralisSimEngine& GetSimEngine() { return SimEngine; }

	// Time warp controls
	void IncreaseTimeWarp();
	void DecreaseTimeWarp();
	void TogglePause();

	double GetTimeWarpScale() const;
	bool IsPaused() const;
	FString GetTimeWarpLabel() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> EarthMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> SpacecraftMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	// NASA Asset References (set these in Blueprint or load from Content)
	UPROPERTY(EditDefaultsOnly, Category = "NASA Assets")
	TObjectPtr<UStaticMesh> SpacecraftMeshAsset;

	UPROPERTY(EditDefaultsOnly, Category = "NASA Assets")
	TObjectPtr<UMaterialInterface> EarthMaterialAsset;

private:
	FAstralisSimEngine SimEngine;

	// Orbit trail as a ring of recent positions
	static constexpr int32 TrailMaxPoints = 512;
	TArray<FVector> TrailPoints_UE;
	int32 TrailWriteIndex = 0;
	double TrailSampleTimer = 0.0;

	// Scale: SI metres to UE centimetres for orbit view
	// 1 km = 1 cm → scale = 1e-5 (m to cm via /1000 for km, then * 1 for cm = 1e-3 * 1e-2 = 1e-5)
	// Actually: Position_m * ScaleFactor = UE_cm
	static constexpr double OrbitScale = 1.0e-5;
	static constexpr double EarthRadius_m = 6.371e6;

	FVector SimToUE(const FVector3d& Position_m) const;
	void UpdateOrbitTrail();
	void DrawTrail() const;

	// Time warp steps
	static const TArray<double>& GetTimeWarpSteps();
	int32 TimeWarpIndex = 3; // Start at 1x

	bool bInitialized = false;
};
