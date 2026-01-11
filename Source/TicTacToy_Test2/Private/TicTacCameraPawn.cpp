// Copyright 2025


#include "TicTacCameraPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ATicTacCameraPawn::ATicTacCameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootComp;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bDoCollisionTest = false;  // ¹Ø±ÕÅö×²¼ì²â
	SpringArm->bEnableCameraLag = false;  // ¹Ø±ÕÉãÏñ»úÑÓ³Ù
	SpringArm->bInheritPitch = false;     // ²»¼Ì³ÐPitch
	SpringArm->bInheritYaw = false;       // ²»¼Ì³ÐYaw
	SpringArm->bInheritRoll = false;      // ²»¼Ì³ÐRoll

	// ´´½¨ÉãÏñ»ú
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	CameraHeight = 800.0f;
	CameraPitch = -90.0f;  // ¸©ÊÓ60¶È
	CameraFOV = 90.0f;

	SpringArm->TargetArmLength = CameraHeight;
	SpringArm->SetRelativeRotation(FRotator(CameraPitch, 0.0f, 0.0f));
	Camera->SetFieldOfView(CameraFOV);
}

// Called when the game starts or when spawned
void ATicTacCameraPawn::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("TicTacCameraPawn"));

	ApplyCameraSettings();

	FocusOnLocation(FVector::ZeroVector);
}

void ATicTacCameraPawn::ApplyCameraSettings()
{
	if (SpringArm)
	{
		SpringArm->TargetArmLength = CameraHeight;
		SpringArm->SetRelativeRotation(FRotator(CameraPitch, 0.0f, 0.0f));
	}

	if (Camera)
	{
		Camera->SetFieldOfView(CameraFOV);
	}

	UE_LOG(LogTemp, Log, TEXT("ApplyCameraSettings CameraHeight: %.1f, CameraPitch: %.1f, CameraFOV: %.1f"), CameraHeight, CameraPitch, CameraFOV);
}

void ATicTacCameraPawn::FocusOnLocation(FVector TargetLocation)
{
	SetActorLocation(TargetLocation);

	UE_LOG(LogTemp, Log, TEXT("CameraFocusOn: %s"), *TargetLocation.ToString());
}

