#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Astralis/Core/AstralisAssemblyGameMode.h"
#include "AstralisAssemblyHUD.generated.h"

class UAstralisComponentPalette;
class UAstralisComponentData;
class UAstralisStatusPanel;
class UAstralisOrbitTelemetryPanel;

UCLASS()
class ASTRALIS_API AAstralisAssemblyHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void OnPhaseChanged(EAstralisGamePhase NewPhase);

	UPROPERTY()
	TObjectPtr<UAstralisComponentPalette> ComponentPalette;

	UPROPERTY()
	TObjectPtr<UAstralisStatusPanel> StatusPanel;

	UPROPERTY()
	TObjectPtr<UAstralisOrbitTelemetryPanel> OrbitTelemetry;

protected:
	UFUNCTION()
	void OnComponentChosen(UAstralisComponentData* Data);
};
