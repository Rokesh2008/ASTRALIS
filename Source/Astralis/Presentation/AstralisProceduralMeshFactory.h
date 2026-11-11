#pragma once

#include "CoreMinimal.h"
#include "AstralisData/ComponentCatalog/AstralisComponentTypes.h"

class UStaticMeshComponent;
class UMaterialInterface;

// Generates recognizable spacecraft component geometry from engine primitives.
// Each component category gets a distinct visual shape at correct physical scale.
// These are development-quality stand-ins, not final art assets.
class ASTRALIS_API FAstralisProceduralMeshFactory
{
public:
	// Apply an appropriate mesh + material to a static mesh component
	// based on the component's category and envelope dimensions.
	static void ApplyComponentVisual(
		UStaticMeshComponent* MeshComp,
		EComponentCategory Category,
		const FVector& Envelope_m);

private:
	static UStaticMesh* GetBaseMesh(EComponentCategory Category);
	static UMaterialInterface* GetMaterial(EComponentCategory Category);
	static FVector ComputeScale(EComponentCategory Category, const FVector& Envelope_m);
};
