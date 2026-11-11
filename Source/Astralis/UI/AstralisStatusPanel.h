#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AstralisStatusPanel.generated.h"

class AAstralisSpacecraftActor;
class AAstralisAssemblyGameMode;

UCLASS()
class ASTRALIS_API UAstralisStatusPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Assembly")
	void SetSpacecraft(AAstralisSpacecraftActor* Spacecraft);

	UFUNCTION(BlueprintCallable, Category = "Assembly")
	void RefreshDisplay();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	TSharedRef<SWidget> MakeSectionHeader(const FString& Title);
	TSharedRef<SWidget> MakeValueRow(const FString& Label, TSharedPtr<STextBlock>& OutValue);

	TWeakObjectPtr<AAstralisSpacecraftActor> TrackedSpacecraft;

	// Mass budget
	TSharedPtr<STextBlock> ComponentCountText;
	TSharedPtr<STextBlock> TotalMassText;
	TSharedPtr<STextBlock> DryMassText;
	TSharedPtr<STextBlock> CoMText;

	// Power budget
	TSharedPtr<STextBlock> PowerGenerationText;
	TSharedPtr<STextBlock> PowerStandbyText;
	TSharedPtr<STextBlock> PowerAverageText;
	TSharedPtr<STextBlock> PowerPeakText;
	TSharedPtr<STextBlock> PowerMarginText;

	// Thermal
	TSharedPtr<STextBlock> ThermalMinText;
	TSharedPtr<STextBlock> ThermalMaxText;
	TSharedPtr<STextBlock> ThermalDissipationText;

	// Data
	TSharedPtr<STextBlock> DataRateText;
	TSharedPtr<STextBlock> CostText;
};
