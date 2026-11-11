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

	AAstralisAssemblyGameMode* GetAssemblyGameMode() const;

public:
	void OnPhaseChanged(EAstralisGamePhase NewPhase);

private:
	void OnLaunchPressed();
	void OnReturnPressed();
	void OnTimeWarpUp();
	void OnTimeWarpDown();
	void OnPauseToggle();
};
