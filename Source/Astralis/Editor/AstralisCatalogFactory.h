#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AstralisCatalogFactory.generated.h"

UCLASS()
class ASTRALIS_API UAstralisCatalogFactory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ASTRALIS|Editor", meta = (CallInEditor = "true"))
	static void CreateSampleComponents();

	static void RegisterConsoleCommands();

private:
	static FAutoConsoleCommand CreateSampleComponentsCmd;
};
