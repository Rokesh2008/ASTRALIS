#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AstralisAssemblyGameMode.generated.h"

class AAstralisSpacecraftActor;
class AAstralisComponentActor;

UCLASS()
class ASTRALIS_API AAstralisAssemblyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAstralisAssemblyGameMode();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Assembly")
	TObjectPtr<AAstralisSpacecraftActor> ActiveSpacecraft;

	UPROPERTY(BlueprintReadOnly, Category = "Assembly")
	TArray<TObjectPtr<AAstralisComponentActor>> InstalledComponents;

	// Spawn the spacecraft bus at origin
	UFUNCTION(BlueprintCallable, Category = "Assembly")
	AAstralisSpacecraftActor* SpawnSpacecraft();

	// Add a component at SI position
	UFUNCTION(BlueprintCallable, Category = "Assembly")
	AAstralisComponentActor* AddComponent(UAstralisComponentData* Data, const FVector& Position_SI_m);

	// Update spacecraft mass properties from all installed components
	UFUNCTION(BlueprintCallable, Category = "Assembly")
	void UpdateMassProperties();
};
