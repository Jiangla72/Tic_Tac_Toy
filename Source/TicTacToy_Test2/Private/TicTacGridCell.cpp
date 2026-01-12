// Copyright 2025


#include "TicTacGridCell.h"

#include "TicTacGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATicTacGridCell::ATicTacGridCell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootComp;

	// 创建底板网格体
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BaseMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	BaseMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// 启用鼠标交互
	BaseMesh->SetGenerateOverlapEvents(true);

	// 创建棋子网格体
	PieceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PieceMesh"));
	PieceMesh->SetupAttachment(BaseMesh);
	PieceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PieceMesh->SetVisibility(false); // 初始隐藏

	//// 加载默认网格体 !改成在编辑器中设置
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
	//if (CubeMesh.Succeeded())
	//{
	//	BaseMesh->SetStaticMesh(CubeMesh.Object);
	//	BaseMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 0.1f)); // 扁平的格子
	//}

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
	//if (SphereMesh.Succeeded())
	//{
	//	PieceMesh->SetStaticMesh(SphereMesh.Object);
	//	PieceMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));
	//	PieceMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 30.0f)); // 悬浮在格子上方
	//}

	// 初始化属性
	GridIndex = -1;
	Row = -1;
	Column = -1;
	Player = EPlayerType::None;
	bIsOccupied = false;
	bIsWinningCell = false;

	// 添加Actor标签（用于射线检测识别）
	Tags.Add(TEXT("GridCell"));
}

// Called when the game starts or when spawned
void ATicTacGridCell::BeginPlay()
{
	Super::BeginPlay();
	if (BaseMesh)
	{
		BaseMesh->OnBeginCursorOver.AddDynamic(this, &ATicTacGridCell::OnMouseBeginHover);
		BaseMesh->OnEndCursorOver.AddDynamic(this, &ATicTacGridCell::OnMouseEndHover);
		BaseMesh->OnClicked.AddDynamic(this, &ATicTacGridCell::OnMouseClick);
	}
	CreateMaterialInstances();

	UpdateCellMaterial();
	UpdatePieceDisplay();
}

// Called every frame
void ATicTacGridCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATicTacGridCell::InitializeCell(int32 InIndex, int32 InRow, int32 InColumn)
{
	GridIndex = InIndex;
	Row = InRow;
	Column = InColumn;
	Player = EPlayerType::None;
	bIsOccupied = false;
	bIsWinningCell = false;

	// 更新Actor名称（方便调试）
	FString NewName = FString::Printf(TEXT("GridCell_%d_%d"), Row, Column);
	SetActorLabel(NewName);
}


void ATicTacGridCell::SetOwningPlayer(EPlayerType NewOwner)
{
	Player = NewOwner;
	bIsOccupied = (NewOwner != EPlayerType::None);
	UpdateCellMaterial();
	UpdatePieceDisplay();
	if (bIsOccupied)
	{
		UE_LOG(LogTemp, Log, TEXT("SetOwningPlayer"));
		PlayPlacementAnimation();
		if (PlacePieceSound)
		{
			PlaySound(PlacePieceSound);
		}
	}
}

void ATicTacGridCell::ResetCell()
{
	Player = EPlayerType::None;
	bIsOccupied = false;
	bIsWinningCell = false;

	UpdateCellMaterial();
	UpdatePieceDisplay();
}

void ATicTacGridCell::SetWinningCell(bool bIsWinning)
{
	bIsWinningCell = bIsWinning;

	if (bIsWinning)
	{
		if (/*BaseMaterialInstance && */WinningCellMaterial)
		{
			BaseMesh->SetMaterial(0, WinningCellMaterial);
		}
		PlayWinAnimation();
	}
	else
	{
		UpdateCellMaterial();
	}
}

void ATicTacGridCell::ShowHoverEffect()
{
	if (bIsOccupied)
	{
		return;
	}
	//UE_LOG(LogTemp, Log, TEXT("ShowHoverEffect"));

	if (HoverMaterial)
	{
		BaseMesh->SetMaterial(0, HoverMaterial);
	}
}

void ATicTacGridCell::HideHoverEffect()
{
	if (bIsOccupied)
	{
		return;
	}
	//UE_LOG(LogTemp, Log, TEXT("HideHoverEffect"));

	UpdateCellMaterial();
}

void ATicTacGridCell::PlayPlacementAnimation()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			if (PieceMesh)
			{
				FVector CurrentScale = PieceMesh->GetRelativeScale3D();
				FVector TargetScale = FVector(0.6f, 0.6f, 0.6f);
				FVector NewScale = FMath::VInterpTo(CurrentScale, TargetScale, GetWorld()->GetDeltaSeconds(), 10.0f);
				PieceMesh->SetRelativeScale3D(NewScale);
			}
		}, 0.01f, true);

	// 0.3秒后停止动画
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			if (PieceMesh)
			{
				PieceMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));
			}
		}, 0.3f, false);
}

void ATicTacGridCell::PlayWinAnimation()
{
	if (!PieceMesh && !BaseMesh)
	{
		return;
	}
}

void ATicTacGridCell::OnMouseBeginHover(UPrimitiveComponent* TouchedComponent)
{
	//UE_LOG(LogTemp, Log, TEXT("OnMouseBeginHover"));

	if (Player == EPlayerType::None)
	{
		ShowHoverEffect();
		if (HoverSound)
		{
			PlaySound(HoverSound, 0.5f);
		}
	}
}

void ATicTacGridCell::OnMouseEndHover(UPrimitiveComponent* TouchedComponent)
{
	//UE_LOG(LogTemp, Log, TEXT("OnMouseEndHover"));

	HideHoverEffect();
}

void ATicTacGridCell::OnMouseClick(UPrimitiveComponent* ClickedComponent, FKey ButtonPressed)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		// 调用PlayerController的HandleCellClick方法
		 //PC->HandleCellClick(GridIndex);  // 需要转换为正确的类型

		// 临时方案：直接调用GameMode
		//ATicTacGameMode* GameMode = Cast<ATicTacGameMode>(GetWorld()->GetAuthGameMode());
		//if (GameMode)
		//{
		//	GameMode->ProcessPlayerMove(GridIndex);
		//}
	}
}

void ATicTacGridCell::UpdateCellMaterial()
{
	if (!BaseMesh)
	{
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("UpdateCellMaterial"));

	// 根据状态选择材质
	UMaterialInterface* MaterialToUse = DefaultMaterial;

	if (bIsWinningCell && WinningCellMaterial)
	{
		MaterialToUse = WinningCellMaterial;
		UE_LOG(LogTemp, Log, TEXT("WinningCellMaterial"));

	}
	else if (bIsOccupied)
	{
		MaterialToUse = DefaultMaterial;
		UE_LOG(LogTemp, Log, TEXT("DefaultMaterial"));
	}

	if (MaterialToUse)
	{
		BaseMesh->SetMaterial(0, MaterialToUse);
	}
}

void ATicTacGridCell::UpdatePieceDisplay()
{
	if (!PieceMesh)
	{
		return;
	}

	if (bIsOccupied && Player != EPlayerType::None)
	{
		PieceMesh->SetVisibility(true);
		PieceMesh->SetHiddenInGame(false);
		UMaterialInterface* PieceMaterial = nullptr;
		if (Player == EPlayerType::Player1 && Player1Material)
		{
			PieceMaterial = Player1Material;
			UE_LOG(LogTemp, Log, TEXT("Player1Material"));

		}
		else if (Player == EPlayerType::Player2 && Player2Material)
		{
			PieceMaterial = Player2Material;
			UE_LOG(LogTemp, Log, TEXT("Player2Material"));
		}

		if (PieceMaterial)
		{
			PieceMesh->SetMaterial(0, PieceMaterial);
		}
	}
	else
	{
		// 隐藏棋子
		PieceMesh->SetVisibility(false);
	}
}

void ATicTacGridCell::CreateMaterialInstances()
{
	if (BaseMesh && DefaultMaterial)
	{
		BaseMaterialInstance = BaseMesh->CreateDynamicMaterialInstance(0, DefaultMaterial);
	}

	if (PieceMesh && Player1Material)
	{
		PieceMaterialInstance = PieceMesh->CreateDynamicMaterialInstance(0, Player1Material);
	}
}

//void ATicTacGridCell::ApplyHighlightMaterial()
//{
//	if (BaseMesh && HighlightMaterial)
//	{
//		BaseMesh->SetMaterial(0, HighlightMaterial);
//		UE_LOG(LogTemp, Log, TEXT("cell [%d, %d] Apply highlightMaterial"), Row, Column);
//	}
//}

//void ATicTacGridCell::ApplyOwnerMaterial(EPlayerType Player)
//{
//	if (!PieceMesh)
//		return;
//
//	switch (Player)
//	{
//	case EPlayerType::Player1:
//		if (Player1Material)
//		{
//			PieceMesh->SetMaterial(0, Player1Material);
//			CurrentMaterialType = EPlayerType::Player1;
//			UE_LOG(LogTemp, Log, TEXT("cell [%d, %d] Apply 1Material"), Row, Column);
//		}
//		break;
//
//	case EPlayerType::Player2:
//		if (Player2Material)
//		{
//			PieceMesh->SetMaterial(0, Player2Material);
//			CurrentMaterialType = EPlayerType::Player2;
//			UE_LOG(LogTemp, Log, TEXT("cell [%d, %d] Apply 2Material"), Row, Column);
//		}
//		break;
//
//	case EPlayerType::None:
//	default:
//		break;
//	}
//}

void ATicTacGridCell::CreateDynamicMaterialInstance()
{
	if (PieceMesh && DefaultMaterial)
	{
		PieceMaterialInstance = PieceMesh->CreateDynamicMaterialInstance(0, DefaultMaterial);
		if (PieceMaterialInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("cell [%d, %d] CreateDynamicMaterialInstance success"), Row, Column);
		}
	}
}

void ATicTacGridCell::SetMaterialParameter(FName ParameterName, float Value)
{
	if (PieceMaterialInstance)
	{
		PieceMaterialInstance->SetScalarParameterValue(ParameterName, Value);
	}
}

void ATicTacGridCell::SetMaterialColorParameter(FName ParameterName, FLinearColor Color)
{
	if (PieceMaterialInstance)
	{
		PieceMaterialInstance->SetVectorParameterValue(ParameterName, Color);
	}
}

void ATicTacGridCell::PlaySound(USoundBase* Sound, float VolumeMultiplier)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			Sound,
			GetActorLocation(),
			VolumeMultiplier
		);
	}
}


