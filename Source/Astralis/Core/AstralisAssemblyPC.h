#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Astralis/Core/AstralisAssemblyGameMode.h"
#include "AstralisAssemblyPC.generated.h"

class AAstralisComponentActor;
class UAstralisComponentData;
class AAstralisAssemblyGameMode;

UENUM(BlueprintType)
enum class EAssemblyTool : uint8
{
	Select  UMETA(DisplayName = "Select"),
	Place   UMETA(DisplayName = "Place Component"),
	Remove  UMETA(DisplayName = "Remove Component")
};

UCLASS()
class ASTRALIS_API AAstralisAssemblyPC : public APlayerController
{
	GENERATED_BODY()

public:
	AAstralisAssemblyPC();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Category = "Assembly")
	EAssemblyTool ActiveTool = EAssemblyTool::Select;

	UPROPERTY(BlueprintReadWrite, Category = "Assembly")
	TObjectPtr<UAstralisComponentData> SelectedComponentData;

	UPROPERTY(BlueprintReadOnly, Category = "Assembly")
	TObjectPtr<AAstralisComponentActor> HoveredComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Assembly")
	TObjectPtr<AAstralisComponentActor> SelectedComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Assembly")
	TObjectPtr<AAstralisComponentActor> GhostPreview;

	UFUNCTION(BlueprintCallable, Category = "Assembly")
	void SetActiveTool(EAssemblyTool Tool);

	UFUNCTION(BlueprintCallable, Category = "Assembly")
	void SetPlacementData(UAstralisComponentData* Data);

protected:
	void OnLeftClick();
	void OnRightClick();

	bool TraceUnderCursor(FHitResult& OutHit) const;
	FVector CursorWorldPosition_SI() const;

	void UpdateGhostPreview();
	void ClearGhostPreview();

	// Try to snap a placement position to the nearest face of an installed component
	FVector SnapToNearestFace(const FVector& CursorPos_SI) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assembly")
	double SnapRadius_m = 0.3;

public:
	AAstralisAssemblyGameMode* GetAssemblyGameMode() const;

	void OnPhaseChanged(EAstralisGamePhase NewPhase);

private:
	void OnLaunchPressed();
	void OnReturnPressed();
	void OnTimeWarpUp();
	void OnTimeWarpDown();
	void OnPauseToggle();
	void OnEngineSwapToggle();
	void OnSwapStageToggle();
	void OnSwapEngine1();
	void OnSwapEngine2();
	void OnSwapEngine3();
	void OnSwapEngine4();
	void OnSwapEngine5();
	void OnSwapEngine6();
	void OnSwapEngine7();
	void OnSwapEngine8();

	// Vehicle / mission selection
	void OnVehicleNext();
	void OnVehiclePrev();
	void OnMissionNext();
	void OnMissionPrev();

	void OnDeorbitBurn();

	// Orbit maneuvers
	void OnManeuverPrograde();
	void OnManeuverRetrograde();
	void OnManeuverNormal();
	void OnManeuverAntiNormal();
	void OnManeuverRadialIn();
	void OnManeuverRadialOut();

	// Throttle control
	void OnThrottleUp();
	void OnThrottleDown();

	// Launch abort
	void OnAbortLaunch();

	// Camera mode
	void OnCycleCamera();

	// Spacecraft design
	void OnBusNext();
	void OnBusPrev();
	void OnInstrument1();
	void OnInstrument2();
	void OnInstrument3();
	void OnInstrument4();
	void OnInstrument5();
	void OnInstrument6();
	void OnInstrument7();
	void OnInstrument8();
	void OnInstrument9();
	void OnInstrument10();

	float ViewModeFixTimer = 5.0f;
};
