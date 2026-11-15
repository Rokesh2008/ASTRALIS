#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Astralis/Core/AstralisAssemblyGameMode.h"
#include "AstralisAssemblyHUD.generated.h"

class UAstralisComponentPalette;
class UAstralisComponentData;
class UAstralisStatusPanel;
class UAstralisOrbitTelemetryPanel;
class UAstralisVehicleInspector;
class UAstralisEngineSwapPanel;
class AAstralisLaunchVehicleActor;

UCLASS()
class ASTRALIS_API AAstralisAssemblyHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	void OnPhaseChanged(EAstralisGamePhase NewPhase);

	EAstralisGamePhase CurrentPhase = EAstralisGamePhase::Assembly;

	UPROPERTY()
	TObjectPtr<UAstralisComponentPalette> ComponentPalette;

	UPROPERTY()
	TObjectPtr<UAstralisStatusPanel> StatusPanel;

	UPROPERTY()
	TObjectPtr<UAstralisOrbitTelemetryPanel> OrbitTelemetry;

	UPROPERTY()
	TObjectPtr<UAstralisVehicleInspector> VehicleInspector;

	UPROPERTY()
	TObjectPtr<UAstralisEngineSwapPanel> EngineSwapPanel;

protected:
	UFUNCTION()
	void OnComponentChosen(UAstralisComponentData* Data);

private:
	void DrawTitleBar(float W, float H);
	void DrawBottomBar(float W, float H);
	void DrawAssemblyHUD(float W, float H, AAstralisAssemblyGameMode* GM);
	void DrawLaunchHUD(float W, float H, AAstralisAssemblyGameMode* GM);
	void DrawOrbitHUD(float W, float H, AAstralisAssemblyGameMode* GM);
	void DrawReentryHUD(float W, float H, AAstralisAssemblyGameMode* GM);

	void DrawPanel(float X, float Y, float PW, float PH, const FString& Title, const FLinearColor& Accent);
	void DrawProgressBar(float X, float Y, float PW, float PH, double Value, double Max, const FLinearColor& Color, const FString& Label);
	void DrawVerticalGauge(float X, float Y, float GW, float GH, double Value, double Min, double Max, const FString& Label, const FString& Unit, const FLinearColor& Color);
	void DrawMissionTimeline(float X, float Y, float TW, float TH, AAstralisLaunchVehicleActor* V);
	void DrawTelemetryGraph(float X, float Y, float GW, float GH, AAstralisLaunchVehicleActor* V);
	void DrawGroundTrackMap(float X, float Y, float MW, float MH, const FVector3d& Position_m, double EarthR);

	float MachCalloutTimer = 0.0f;
	FString MachCalloutText;
	FLinearColor MachCalloutColor;
	double LastMachNumber = 0.0;
};
