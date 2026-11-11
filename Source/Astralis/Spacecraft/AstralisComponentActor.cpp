#include "AstralisComponentActor.h"
#include "Astralis/Presentation/AstralisCoordinateService.h"
#include "Astralis/Presentation/AstralisProceduralMeshFactory.h"

AAstralisComponentActor::AAstralisComponentActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ComponentMesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
}

void AAstralisComponentActor::ApplyComponentData()
{
	if (!ComponentData || !MeshComponent)
	{
		return;
	}

	// Try custom mesh first; fall back to procedural generation
	UStaticMesh* Mesh = ComponentData->Mesh.LoadSynchronous();
	if (Mesh)
	{
		MeshComponent->SetStaticMesh(Mesh);
	}
	else
	{
		FAstralisProceduralMeshFactory::ApplyComponentVisual(
			MeshComponent,
			ComponentData->Category,
			ComponentData->Geometry.Envelope_m);
	}
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

FVector AAstralisComponentActor::GetEnvelope_SI() const
{
	if (ComponentData)
	{
		return ComponentData->Geometry.Envelope_m;
	}
	return FVector(0.1, 0.1, 0.1);
}

TArray<FVector> AAstralisComponentActor::GetAttachmentFacePositions_SI() const
{
	TArray<FVector> Faces;
	const FVector Env = GetEnvelope_SI();
	const FVector Half = Env * 0.5;

	Faces.Add(Position_SI_m + FVector(Half.X, 0.0, 0.0));   // +X
	Faces.Add(Position_SI_m + FVector(-Half.X, 0.0, 0.0));  // -X
	Faces.Add(Position_SI_m + FVector(0.0, Half.Y, 0.0));   // +Y
	Faces.Add(Position_SI_m + FVector(0.0, -Half.Y, 0.0));  // -Y
	Faces.Add(Position_SI_m + FVector(0.0, 0.0, Half.Z));   // +Z
	Faces.Add(Position_SI_m + FVector(0.0, 0.0, -Half.Z));  // -Z

	return Faces;
}
