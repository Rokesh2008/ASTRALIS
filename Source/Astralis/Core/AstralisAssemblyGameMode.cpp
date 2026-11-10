#include "AstralisAssemblyGameMode.h"
#include "Astralis/Spacecraft/AstralisSpacecraftActor.h"
#include "Astralis/Spacecraft/AstralisComponentActor.h"
#include "AstralisSimulation/MassProperties/AstralisMassCalculator.h"

AAstralisAssemblyGameMode::AAstralisAssemblyGameMode()
{
}

void AAstralisAssemblyGameMode::BeginPlay()
{
	Super::BeginPlay();
	SpawnSpacecraft();
}

AAstralisSpacecraftActor* AAstralisAssemblyGameMode::SpawnSpacecraft()
{
	if (ActiveSpacecraft)
	{
		return ActiveSpacecraft;
	}

	FActorSpawnParameters Params;
	Params.Name = FName(TEXT("ASTRALIS_Spacecraft"));
	ActiveSpacecraft = GetWorld()->SpawnActor<AAstralisSpacecraftActor>(
		AAstralisSpacecraftActor::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		Params
	);

	return ActiveSpacecraft;
}

AAstralisComponentActor* AAstralisAssemblyGameMode::AddComponent(
	UAstralisComponentData* Data,
	const FVector& Position_SI_m)
{
	if (!Data || !ActiveSpacecraft)
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	AAstralisComponentActor* CompActor = GetWorld()->SpawnActor<AAstralisComponentActor>(
		AAstralisComponentActor::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		Params
	);

	if (CompActor)
	{
		CompActor->ComponentData = Data;
		CompActor->SetPositionSI(Position_SI_m);
		CompActor->AttachToActor(ActiveSpacecraft, FAttachmentTransformRules::KeepWorldTransform);
		InstalledComponents.Add(CompActor);
		UpdateMassProperties();
	}

	return CompActor;
}

void AAstralisAssemblyGameMode::UpdateMassProperties()
{
	if (!ActiveSpacecraft)
	{
		return;
	}

	ActiveSpacecraft->ComponentEntries.Empty();

	for (const auto& Comp : InstalledComponents)
	{
		if (Comp && Comp->ComponentData)
		{
			FComponentMassEntry Entry;
			Entry.Mass_kg = Comp->ComponentData->Mass_kg;
			Entry.Position_m = FVector3d(Comp->Position_SI_m);
			ActiveSpacecraft->ComponentEntries.Add(Entry);
		}
	}

	ActiveSpacecraft->RecalculateMassProperties();
}
