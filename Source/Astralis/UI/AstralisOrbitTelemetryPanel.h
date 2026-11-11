#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AstralisOrbitTelemetryPanel.generated.h"

class AAstralisOrbitSceneActor;

UCLASS()
class ASTRALIS_API UAstralisOrbitTelemetryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetOrbitScene(AAstralisOrbitSceneActor* InScene);
	void RefreshDisplay();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	TSharedRef<SWidget> MakeRow(const FString& Label, TSharedPtr<STextBlock>& OutValue);

	TWeakObjectPtr<AAstralisOrbitSceneActor> OrbitScene;

	// Orbital
	TSharedPtr<STextBlock> AltitudeText;
	TSharedPtr<STextBlock> VelocityText;
	TSharedPtr<STextBlock> PeriodText;
	TSharedPtr<STextBlock> EclipseText;

	// Power
	TSharedPtr<STextBlock> SolarGenText;
	TSharedPtr<STextBlock> PowerDrawText;
	TSharedPtr<STextBlock> BatteryText;

	// Comms
	TSharedPtr<STextBlock> LinkMarginText;
	TSharedPtr<STextBlock> DataRateText;
	TSharedPtr<STextBlock> GroundVisText;

	// Time
	TSharedPtr<STextBlock> METText;
	TSharedPtr<STextBlock> TimeWarpText;
};
