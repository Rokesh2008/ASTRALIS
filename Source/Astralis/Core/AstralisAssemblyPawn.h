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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinZoomDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxZoomDistance = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ZoomSpeed = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float OrbitSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float PanSpeed = 5.0f;

private:
	void OnZoomIn();
	void OnZoomOut();

};
