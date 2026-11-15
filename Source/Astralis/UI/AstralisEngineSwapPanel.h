#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AstralisData/VehicleLibrary/AstralisVehicleTypes.h"
#include "AstralisEngineSwapPanel.generated.h"

class AAstralisLaunchVehicleActor;
class AAstralisStageActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEngineSwapped, int32, StageIndex, FName, NewEngineName);

UCLASS()
class ASTRALIS_API UAstralisEngineSwapPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Engine Swap")
	void SetLaunchVehicle(AAstralisLaunchVehicleActor* Vehicle);

	UFUNCTION(BlueprintCallable, Category = "Engine Swap")
	void SelectStage(int32 StageIndex);

	UPROPERTY(BlueprintAssignable)
	FOnEngineSwapped OnEngineSwapped;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	TSharedRef<SWidget> MakeHeader(const FString& Title);
	TSharedRef<SWidget> MakeEngineButton(int32 EngineIndex);

	void ApplyEngine(int32 EngineIndex);
	void UpdatePreview();

	TWeakObjectPtr<AAstralisLaunchVehicleActor> TrackedVehicle;
	int32 SelectedStageIndex = 0;

	TArray<FEngineSpec_Vehicle> EngineCatalog;

	// UI elements
	TSharedPtr<SVerticalBox> EngineListBox;
	TSharedPtr<STextBlock> StageNameText;
	TSharedPtr<STextBlock> CurrentEngineText;
	TSharedPtr<STextBlock> PreviewThrustText;
	TSharedPtr<STextBlock> PreviewIspText;
	TSharedPtr<STextBlock> PreviewTWRText;
	TSharedPtr<STextBlock> PreviewDVText;
	TSharedPtr<STextBlock> CompatibilityText;
};
