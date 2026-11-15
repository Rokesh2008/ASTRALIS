#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AstralisAssemblyPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

// Orbit camera pawn for the assembly lab.
// Rotates around a focal point (spacecraft center).
// Scroll wheel zooms, middle-mouse-drag orbits, WASD pans focus.
UCLASS()
class ASTRALIS_API AAstralisAssemblyPawn : public APawn
{
	GENERATED_BODY()

public:
	AAstralisAssemblyPawn();

	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

	// Snap the orbit camera to look at Target from Distance, at the given yaw/pitch
	void FocusOn(const FVector& Target, float Distance, float Yaw, float Pitch);

	// Move the focus point without touching the player's chosen angle/zoom
	void SetFocusPoint(const FVector& Target);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinZoomDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxZoomDistance = 50000.0f;

	// Fraction of current distance per scroll notch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ZoomStep = 0.12f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float OrbitSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float PanSpeed = 5.0f;

	// Launch vibration
	float ShakeIntensity = 0.0f;
	float ShakeTimer = 0.0f;
	void SetShakeIntensity(float Intensity) { ShakeIntensity = Intensity; }

	// Camera mode
	enum class ECameraMode : uint8 { Tracking, Close, Wide };
	ECameraMode CameraMode = ECameraMode::Tracking;
	void CycleCameraMode();
	FString GetCameraModeString() const;

private:
	void OnZoomIn();
	void OnZoomOut();

};
