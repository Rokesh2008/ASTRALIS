#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AstralisData/ComponentCatalog/AstralisComponentTypes.h"
#include "AstralisComponentActor.generated.h"

// Visual representation of one spacecraft component in the assembly lab.
// Bridges a UAstralisComponentData definition to a placed UE mesh.
UCLASS()
class ASTRALIS_API AAstralisComponentActor : public AActor
{
	GENERATED_BODY()

public:
	AAstralisComponentActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	// The data asset defining this component's engineering properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UAstralisComponentData> ComponentData;

	// Position in spacecraft body frame (SI metres)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engineering")
	FVector Position_SI_m = FVector::ZeroVector;

	// Grid snap resolution in metres
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assembly")
	double GridSnap_m = 0.05;

	// Whether this component is currently selected for placement
	UPROPERTY(BlueprintReadWrite, Category = "Assembly")
	bool bIsSelected = false;

	// Snap position to grid
	UFUNCTION(BlueprintCallable, Category = "Assembly")
	FVector SnapToGrid(const FVector& Position_m) const;

	// Set SI position and update UE transform
	UFUNCTION(BlueprintCallable, Category = "Assembly")
	void SetPositionSI(const FVector& NewPosition_m);

	// Highlight for selection
	UFUNCTION(BlueprintCallable, Category = "Assembly")
	void SetHighlighted(bool bHighlight);
};
