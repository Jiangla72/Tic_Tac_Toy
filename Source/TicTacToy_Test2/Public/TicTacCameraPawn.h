// Copyright 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TicTacCameraPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class TICTACTOY_TEST2_API ATicTacCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATicTacCameraPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings", meta = (ClampMin = "300.0", ClampMax = "2000.0"))
	float CameraHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings", meta = (ClampMin = "-90.0", ClampMax = "0.0"))
	float CameraPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings", meta = (ClampMin = "30.0", ClampMax = "120.0"))
	float CameraFOV;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ApplyCameraSettings();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void FocusOnLocation(FVector TargetLocation);
};
