#include "AstralisAssemblyHUD.h"
#include "AstralisComponentPalette.h"
#include "AstralisStatusPanel.h"
#include "AstralisOrbitTelemetryPanel.h"
#include "Astralis/Core/AstralisAssemblyPC.h"
#include "Astralis/Core/AstralisAssemblyGameMode.h"
#include "Astralis/Orbit/AstralisOrbitSceneActor.h"
#include "AstralisData/ComponentCatalog/AstralisComponentTypes.h"

void AAstralisAssemblyHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return;
	}

	// Assembly UI
	ComponentPalette = CreateWidget<UAstralisComponentPalette>(PC);
	if (ComponentPalette)
	{
		ComponentPalette->SetPositionInViewport(FVector2D(10.0f, 10.0f));
		ComponentPalette->AddToViewport(10);
		ComponentPalette->OnComponentSelected.AddDynamic(this, &AAstralisAssemblyHUD::OnComponentChosen);
	}

	StatusPanel = CreateWidget<UAstralisStatusPanel>(PC);
	if (StatusPanel)
	{
		FVector2D ViewportSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}
		else
		{
			ViewportSize = FVector2D(1920.0, 1080.0);
		}
		StatusPanel->SetPositionInViewport(FVector2D(ViewportSize.X - 310.0, 10.0));
		StatusPanel->AddToViewport(10);

		if (AAstralisAssemblyGameMode* GM = Cast<AAstralisAssemblyGameMode>(GetWorld()->GetAuthGameMode()))
		{
			StatusPanel->SetSpacecraft(GM->ActiveSpacecraft);
		}
	}

	// Orbit telemetry (created but hidden until orbit phase)
	OrbitTelemetry = CreateWidget<UAstralisOrbitTelemetryPanel>(PC);
	if (OrbitTelemetry)
	{
		OrbitTelemetry->SetPositionInViewport(FVector2D(10.0f, 10.0f));
		OrbitTelemetry->AddToViewport(10);
		OrbitTelemetry->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AAstralisAssemblyHUD::OnComponentChosen(UAstralisComponentData* Data)
{
	if (AAstralisAssemblyPC* PC = Cast<AAstralisAssemblyPC>(GetOwningPlayerController()))
	{
		PC->SetPlacementData(Data);
	}
}

void AAstralisAssemblyHUD::OnPhaseChanged(EAstralisGamePhase NewPhase)
{
	if (NewPhase == EAstralisGamePhase::Orbit)
	{
		// Hide assembly UI
		if (ComponentPalette)
			ComponentPalette->SetVisibility(ESlateVisibility::Collapsed);
		if (StatusPanel)
			StatusPanel->SetVisibility(ESlateVisibility::Collapsed);

		// Show orbit telemetry
		if (OrbitTelemetry)
		{
			OrbitTelemetry->SetVisibility(ESlateVisibility::Visible);
			if (AAstralisAssemblyGameMode* GM = Cast<AAstralisAssemblyGameMode>(GetWorld()->GetAuthGameMode()))
			{
				OrbitTelemetry->SetOrbitScene(GM->OrbitScene);
			}
		}
	}
	else
	{
		// Show assembly UI
		if (ComponentPalette)
			ComponentPalette->SetVisibility(ESlateVisibility::Visible);
		if (StatusPanel)
			StatusPanel->SetVisibility(ESlateVisibility::Visible);

		// Hide orbit telemetry
		if (OrbitTelemetry)
			OrbitTelemetry->SetVisibility(ESlateVisibility::Collapsed);
	}
}
