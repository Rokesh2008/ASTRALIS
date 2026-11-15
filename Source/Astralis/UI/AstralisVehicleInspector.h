#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Astralis/LaunchVehicle/AstralisLaunchVehicleActor.h"
#include "AstralisVehicleInspector.generated.h"

// Shows launch vehicle engineering data and allows component modification.
// This is the core UI for the Rocket Architect workspace.
UCLASS()
class ASTRALIS_API UAstralisVehicleInspector : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void SetLaunchVehicle(AAstralisLaunchVehicleActor* Vehicle);

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void RefreshDisplay();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	TSharedRef<SWidget> MakeHeader(const FString& Title);
	TSharedRef<SWidget> MakeRow(const FString& Label, TSharedPtr<STextBlock>& OutValue);

	TWeakObjectPtr<AAstralisLaunchVehicleActor> TrackedVehicle;

	// Vehicle overview
	TSharedPtr<STextBlock> VehicleNameText;
	TSharedPtr<STextBlock> TotalMassText;
	TSharedPtr<STextBlock> TotalDeltaVText;
	TSharedPtr<STextBlock> PayloadMassText;
	TSharedPtr<STextBlock> StatusText;

	// Stage 1
	TSharedPtr<STextBlock> Stage1NameText;
	TSharedPtr<STextBlock> Stage1MassText;
	TSharedPtr<STextBlock> Stage1PropText;
	TSharedPtr<STextBlock> Stage1ThrustText;
	TSharedPtr<STextBlock> Stage1IspText;
	TSharedPtr<STextBlock> Stage1TWRText;
	TSharedPtr<STextBlock> Stage1DVText;

	// Stage 2
	TSharedPtr<STextBlock> Stage2NameText;
	TSharedPtr<STextBlock> Stage2MassText;
	TSharedPtr<STextBlock> Stage2PropText;
	TSharedPtr<STextBlock> Stage2ThrustText;
	TSharedPtr<STextBlock> Stage2IspText;
	TSharedPtr<STextBlock> Stage2TWRText;
	TSharedPtr<STextBlock> Stage2DVText;

	// Validation
	TSharedPtr<STextBlock> ValidationText;

	// Helpers
	void UpdateAllValues();
};
