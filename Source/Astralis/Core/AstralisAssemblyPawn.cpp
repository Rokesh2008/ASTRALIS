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
	CameraBoom->TargetArmLength = 6000.0f;
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetWorldRotation(FRotator(-12.0f, 45.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = false;
	CameraBoom->bEnableCameraRotationLag = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->FieldOfView = 75.0f;
}

void AAstralisAssemblyPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

	InInputComponent->BindKey(EKeys::MouseScrollUp, IE_Pressed, this, &AAstralisAssemblyPawn::OnZoomIn);
	InInputComponent->BindKey(EKeys::MouseScrollDown, IE_Pressed, this, &AAstralisAssemblyPawn::OnZoomOut);
}

void AAstralisAssemblyPawn::FocusOn(const FVector& Target, float Distance, float Yaw, float Pitch)
{
	SetActorLocation(Target);
	CameraBoom->SetWorldRotation(FRotator(FMath::Clamp(Pitch, -89.0f, 89.0f), Yaw, 0.0f));
	CameraBoom->TargetArmLength = FMath::Clamp(Distance, MinZoomDistance, MaxZoomDistance);
}

void AAstralisAssemblyPawn::SetFocusPoint(const FVector& Target)
{
	SetActorLocation(Target);
}

void AAstralisAssemblyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	// Hold right mouse and drag to orbit around the focus point
	if (PC->IsInputKeyDown(EKeys::RightMouseButton))
	{
		float MouseX, MouseY;
		PC->GetInputMouseDelta(MouseX, MouseY);

		FRotator Rot = CameraBoom->GetComponentRotation();
		Rot.Yaw += MouseX * OrbitSpeed;
		Rot.Pitch = FMath::Clamp(Rot.Pitch + MouseY * OrbitSpeed, -89.0f, 89.0f);
		Rot.Roll = 0.0f;
		CameraBoom->SetWorldRotation(Rot);
	}

	// WASD camera pan relative to boom orientation
	{
		FVector PanDelta = FVector::ZeroVector;
		const FRotator BoomRot = CameraBoom->GetComponentRotation();
		const FVector Forward = FRotationMatrix(BoomRot).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(BoomRot).GetUnitAxis(EAxis::Y);

		const float Speed = PanSpeed * CameraBoom->TargetArmLength * 0.001f * DeltaTime;

		if (PC->IsInputKeyDown(EKeys::W)) PanDelta += Forward * Speed;
		if (PC->IsInputKeyDown(EKeys::S)) PanDelta -= Forward * Speed;
		if (PC->IsInputKeyDown(EKeys::D)) PanDelta += Right * Speed;
		if (PC->IsInputKeyDown(EKeys::A)) PanDelta -= Right * Speed;

		if (!PanDelta.IsNearlyZero())
		{
			AddActorWorldOffset(PanDelta);
		}
	}

	// Launch vibration — multi-frequency Perlin shake
	if (ShakeIntensity > 0.5f && Camera)
	{
		ShakeTimer += DeltaTime;
		FVector Offset;
		Offset.X = ShakeIntensity * FMath::PerlinNoise1D(ShakeTimer * 15.0f);
		Offset.Y = ShakeIntensity * FMath::PerlinNoise1D(ShakeTimer * 19.0f + 100.0f);
		Offset.Z = ShakeIntensity * FMath::PerlinNoise1D(ShakeTimer * 11.0f + 200.0f);
		Camera->SetRelativeLocation(Offset);
	}
	else if (Camera && ShakeTimer > 0.0f)
	{
		Camera->SetRelativeLocation(FVector::ZeroVector);
		ShakeTimer = 0.0f;
	}
}

void AAstralisAssemblyPawn::OnZoomIn()
{
	CameraBoom->TargetArmLength = FMath::Max(
		MinZoomDistance, CameraBoom->TargetArmLength * (1.0f - ZoomStep));
}

void AAstralisAssemblyPawn::OnZoomOut()
{
	CameraBoom->TargetArmLength = FMath::Min(
		MaxZoomDistance, CameraBoom->TargetArmLength * (1.0f + ZoomStep));
}

void AAstralisAssemblyPawn::CycleCameraMode()
{
	switch (CameraMode)
	{
	case ECameraMode::Tracking: CameraMode = ECameraMode::Close; break;
	case ECameraMode::Close:    CameraMode = ECameraMode::Wide; break;
	case ECameraMode::Wide:     CameraMode = ECameraMode::Tracking; break;
	}
}

FString AAstralisAssemblyPawn::GetCameraModeString() const
{
	switch (CameraMode)
	{
	case ECameraMode::Tracking: return TEXT("TRACKING");
	case ECameraMode::Close:    return TEXT("CLOSE-UP");
	case ECameraMode::Wide:     return TEXT("WIDE ANGLE");
	default:                    return TEXT("TRACKING");
	}
}
