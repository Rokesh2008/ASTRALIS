#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AstralisAssemblyGameMode.generated.h"

class AAstralisSpacecraftActor;
class AAstralisComponentActor;
class UAstralisComponentData;
class AAstralisAssemblyPC;
class AAstralisAssemblyHUD;
class AAstralisOrbitSceneActor;

UENUM(BlueprintType)
enum class EAstralisGamePhase : uint8
{
	Assembly    UMETA(DisplayName = "Assembly"),
	Orbit       UMETA(DisplayName = "Orbit Simulation")
};

UCLASS()
class ASTRALIS_API AAstralisAssemblyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAstralisAssemblyGameMode();

	virtual void BeginPlay() override;

	void SpawnAssemblyEnvironment();

	UPROPERTY(BlueprintReadOnly, Category = "Assembly")
	TObjectPtr<AAstralisSpacecraftActor> ActiveSpacecraft;

	UPROPERTY(BlueprintReadOnly, Category = "Assembly")
	TArray<TObjectPtr<AAstralisComponentActor>> InstalledComponents;

	// Spawn the spacecraft bus at origin
	UFUNCTION(BlueprintCallable, Category = "Assembly")
	AAstralisSpacecraftActor* SpawnSpacecraft();

	// Add a component at SI position
	UFUNCTION(BlueprintCallable, Category = "Assembly")
	AAstralisComponentActor* AddSpacecraftComponent(UAstralisComponentData* Data, const FVector& Position_SI_m);

	// Remove a placed component
	UFUNCTION(BlueprintCallable, Category = "Assembly")
	bool RemoveSpacecraftComponent(AAstralisComponentActor* Component);

	// Update spacecraft mass properties from all installed components
	UFUNCTION(BlueprintCallable, Category = "Assembly")
	void UpdateMassProperties();

	// Get all available component data assets from the asset registry
	UFUNCTION(BlueprintCallable, Category = "Assembly")
	TArray<UAstralisComponentData*> GetAvailableComponents() const;

	// Phase transition
	UPROPERTY(BlueprintReadOnly, Category = "Mission")
	EAstralisGamePhase CurrentPhase = EAstralisGamePhase::Assembly;

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void LaunchToOrbit();

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void ReturnToAssembly();

	UPROPERTY(BlueprintReadOnly, Category = "Mission")
	TObjectPtr<AAstralisOrbitSceneActor> OrbitScene;
};
