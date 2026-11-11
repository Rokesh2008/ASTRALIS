#include "AstralisProceduralMeshFactory.h"
#include "AstralisCoordinateService.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

void FAstralisProceduralMeshFactory::ApplyComponentVisual(
	UStaticMeshComponent* MeshComp,
	EComponentCategory Category,
	const FVector& Envelope_m)
{
	if (!MeshComp)
	{
		return;
	}

	UStaticMesh* Mesh = GetBaseMesh(Category);
	if (Mesh)
	{
		MeshComp->SetStaticMesh(Mesh);
	}

	UMaterialInterface* Mat = GetMaterial(Category);
	if (Mat)
	{
		MeshComp->SetMaterial(0, Mat);
	}

	FVector Scale = ComputeScale(Category, Envelope_m);
	MeshComp->SetWorldScale3D(Scale);
}

UStaticMesh* FAstralisProceduralMeshFactory::GetBaseMesh(EComponentCategory Category)
{
	// Use distinct engine primitives per category for visual differentiation
	switch (Category)
	{
	case EComponentCategory::Structure:
		return LoadObject<UStaticMesh>(nullptr,
			TEXT("/Engine/BasicShapes/Cube.Cube"));

	case EComponentCategory::Power:
		// Flat panel for solar arrays, small box for batteries
		return LoadObject<UStaticMesh>(nullptr,
			TEXT("/Engine/BasicShapes/Cube.Cube"));

	case EComponentCategory::Propulsion:
		return LoadObject<UStaticMesh>(nullptr,
			TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	case EComponentCategory::Avionics:
		return LoadObject<UStaticMesh>(nullptr,
			TEXT("/Engine/BasicShapes/Cube.Cube"));

	case EComponentCategory::Communications:
		return LoadObject<UStaticMesh>(nullptr,
			TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	case EComponentCategory::Thermal:
		return LoadObject<UStaticMesh>(nullptr,
			TEXT("/Engine/BasicShapes/Cube.Cube"));

	case EComponentCategory::Instrument:
		return LoadObject<UStaticMesh>(nullptr,
			TEXT("/Engine/BasicShapes/Cube.Cube"));

	default:
		return LoadObject<UStaticMesh>(nullptr,
			TEXT("/Engine/BasicShapes/Cube.Cube"));
	}
}

UMaterialInterface* FAstralisProceduralMeshFactory::GetMaterial(EComponentCategory Category)
{
	switch (Category)
	{
	case EComponentCategory::Structure:
		return LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Game/Materials/M_SpacecraftBus.M_SpacecraftBus"));

	case EComponentCategory::Power:
		return LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Game/Materials/M_SolarPanel.M_SolarPanel"));

	case EComponentCategory::Communications:
		return LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Game/Materials/M_Antenna.M_Antenna"));

	case EComponentCategory::Propulsion:
		return LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Game/Materials/M_SpacecraftBus.M_SpacecraftBus"));

	default:
		return LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Game/Materials/M_SpacecraftBus.M_SpacecraftBus"));
	}
}

FVector FAstralisProceduralMeshFactory::ComputeScale(
	EComponentCategory Category,
	const FVector& Envelope_m)
{
	// Engine primitives are 100cm (1m) per side by default.
	// Scale to match the component's physical envelope, converting metres to UE cm.
	// Engine Cube is 100x100x100 UU (1m), Cylinder is 100 diameter x 100 height.

	const double ToCm = FAstralisCoordinateService::MetresToCm;

	switch (Category)
	{
	case EComponentCategory::Structure:
		// Box matching envelope
		return FVector(
			Envelope_m.X * ToCm / 100.0,
			Envelope_m.Y * ToCm / 100.0,
			Envelope_m.Z * ToCm / 100.0);

	case EComponentCategory::Power:
		// Flat panel: thin in Z
		return FVector(
			Envelope_m.X * ToCm / 100.0,
			Envelope_m.Y * ToCm / 100.0,
			FMath::Max(Envelope_m.Z, 0.02) * ToCm / 100.0);

	case EComponentCategory::Propulsion:
		// Cylinder: X is diameter, Z is height
		return FVector(
			Envelope_m.X * ToCm / 100.0,
			Envelope_m.Y * ToCm / 100.0,
			Envelope_m.Z * ToCm / 100.0);

	case EComponentCategory::Communications:
		// Dish/cylinder: wider than tall
		return FVector(
			Envelope_m.X * ToCm / 100.0,
			Envelope_m.Y * ToCm / 100.0,
			Envelope_m.Z * ToCm / 100.0);

	default:
		return FVector(
			Envelope_m.X * ToCm / 100.0,
			Envelope_m.Y * ToCm / 100.0,
			Envelope_m.Z * ToCm / 100.0);
	}
}
