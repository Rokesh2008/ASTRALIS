#include "AstralisAssemblyPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AAstralisAssemblyPawn::AAstralisAssemblyPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootScene);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootScene);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
}

void AAstralisAssemblyPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

	InInputComponent->BindKey(EKeys::MouseScrollUp, IE_Pressed, this, &AAstralisAssemblyPawn::OnZoomIn);
	InInputComponent->BindKey(EKeys::MouseScrollDown, IE_Pressed, this, &AAstralisAssemblyPawn::OnZoomOut);

}

void AAstralisAssemblyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	// Orbit: hold right mouse button and move mouse
	bool bRMBDown = PC->IsInputKeyDown(EKeys::RightMouseButton);

	if (bRMBDown)
	{
		float MouseX, MouseY;
		PC->GetInputMouseDelta(MouseX, MouseY);

		FRotator CurrentRot = CameraBoom->GetRelativeRotation();
		CurrentRot.Yaw += MouseX * OrbitSpeed;
		CurrentRot.Pitch = FMath::Clamp(CurrentRot.Pitch + MouseY * OrbitSpeed, -89.0f, 89.0f);
		CameraBoom->SetRelativeRotation(CurrentRot);
	}

	// Pan: hold middle mouse button
	bool bMMBDown = PC->IsInputKeyDown(EKeys::MiddleMouseButton);
	if (bMMBDown)
	{
		float MouseX, MouseY;
		PC->GetInputMouseDelta(MouseX, MouseY);

		FVector Right = CameraBoom->GetRightVector() * MouseX * PanSpeed;
		FVector Up = CameraBoom->GetUpVector() * -MouseY * PanSpeed;
		AddActorWorldOffset(Right + Up);
	}

	// WASD pan
	float Forward = PC->GetInputAnalogKeyState(EKeys::W) - PC->GetInputAnalogKeyState(EKeys::S);
	float Right = PC->GetInputAnalogKeyState(EKeys::D) - PC->GetInputAnalogKeyState(EKeys::A);

	if (FMath::Abs(Forward) > 0.01f || FMath::Abs(Right) > 0.01f)
	{
		FVector ForwardDir = CameraBoom->GetForwardVector();
		ForwardDir.Z = 0.0f;
		ForwardDir.Normalize();
		FVector RightDir = CameraBoom->GetRightVector();
		RightDir.Z = 0.0f;
		RightDir.Normalize();

		FVector Move = (ForwardDir * Forward + RightDir * Right) * PanSpeed * 100.0f * DeltaTime;
		AddActorWorldOffset(Move);
	}
}

void AAstralisAssemblyPawn::OnZoomIn()
{
	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = FMath::Max(
			MinZoomDistance, CameraBoom->TargetArmLength - ZoomSpeed);
	}
}

void AAstralisAssemblyPawn::OnZoomOut()
{
	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = FMath::Min(
			MaxZoomDistance, CameraBoom->TargetArmLength + ZoomSpeed);
	}
}
