#include "AstralisComponentActor.h"
#include "Astralis/Presentation/AstralisCoordinateService.h"

AAstralisComponentActor::AAstralisComponentActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ComponentMesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
}

FVector AAstralisComponentActor::SnapToGrid(const FVector& Position_m) const
{
	if (GridSnap_m <= 0.0)
	{
		return Position_m;
	}
	return FVector(
		FMath::RoundToDouble(Position_m.X / GridSnap_m) * GridSnap_m,
		FMath::RoundToDouble(Position_m.Y / GridSnap_m) * GridSnap_m,
		FMath::RoundToDouble(Position_m.Z / GridSnap_m) * GridSnap_m
	);
}

void AAstralisComponentActor::SetPositionSI(const FVector& NewPosition_m)
{
	Position_SI_m = SnapToGrid(NewPosition_m);
	const FVector UEPos = FAstralisCoordinateService::SIToUE(FVector3d(Position_SI_m));
	SetActorLocation(UEPos);
}

void AAstralisComponentActor::SetHighlighted(bool bHighlight)
{
	bIsSelected = bHighlight;
	if (MeshComponent)
	{
		MeshComponent->SetRenderCustomDepth(bHighlight);
	}
}
