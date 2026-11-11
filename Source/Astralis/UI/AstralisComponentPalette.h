#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AstralisComponentPalette.generated.h"

class UAstralisComponentData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComponentSelected, UAstralisComponentData*, Data);

UCLASS()
class ASTRALIS_API UAstralisComponentPalette : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Assembly")
	FOnComponentSelected OnComponentSelected;

	UFUNCTION(BlueprintCallable, Category = "Assembly")
	void RefreshList();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	TSharedPtr<SVerticalBox> ListBox;

	TArray<TObjectPtr<UAstralisComponentData>> CachedComponents;

	void LoadComponents();
	void BuildEntries();
};
