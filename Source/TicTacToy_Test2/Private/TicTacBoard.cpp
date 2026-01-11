
// Copyright 2025


#include "TicTacBoard.h"
#include "TicTacGridCell.h"
#include "TicTacGameMode.h"
#include "TicTacGameConfig.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATicTacBoard::ATicTacBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootComp;

	BoardBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardBaseMesh"));
	BoardBaseMesh->SetupAttachment(RootComponent);
	BoardBaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 使用引擎自带的平
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));
	//if (PlaneMesh.Succeeded())
	//{
	//	BoardBaseMesh->SetStaticMesh(PlaneMesh.Object);
	//	BoardBaseMesh->SetRelativeScale3D(FVector(3.0f, 3.0f, 0.1f));
	//	BoardBaseMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -20.0f));
	//}

	BoardSize = 3;
	CellSpacing = 120.0f;
	CellScale = 1.0f;
	GridCellClass = nullptr;

	Tags.Add(TEXT("Board"));
}

// Called when the game starts or when spawned
void ATicTacBoard::BeginPlay()
{
	Super::BeginPlay();

	LoadConfigFromGameMode();
	GenerateBoard(BoardSize);
}

// Called every frame
void ATicTacBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if WITH_EDITOR
	if (GEngine && GEngine->bEnableOnScreenDebugMessages)
	{
		// DrawDebugGrid();  // 取消注释以启用
	}
#endif
}

void ATicTacBoard::GenerateBoard(int32 Size)
{
	if (Size < 3 || Size > 10)
	{
		return;
	}

	if (!GridCellClass)
	{
		return;
	}

	ClearBoard();

	BoardSize = Size;
	int32 TotalCells = Size * Size;

	float BoardWidth = (Size - 1) * CellSpacing; // 第一个到最后一个格子的中心距离
	FVector CenterOffset = FVector(-BoardWidth / 2.0f, -BoardWidth / 2.0f, 0.0f);

	for (int32 Row = 0; Row < Size; Row++)
	{
		for (int32 Column = 0; Column < Size; Column++)
		{
			int32 Index = RowColumnToIndex(Row, Column);

			FVector CellLocation = CalculateCellWorldLocation(Row, Column) + CenterOffset;//向左移动半个棋盘

			ATicTacGridCell* NewCell = SpawnGridCell(Index, Row, Column, CellLocation);

			if (NewCell)
			{
				GridCells.Add(NewCell);
			}
		}
	}

	if (BoardBaseMesh)
	{
		float BaseScale = (BoardWidth + CellSpacing) / 100.0f; // Plane的默认大小
		BoardBaseMesh->SetRelativeScale3D(FVector(BaseScale, BaseScale, 0.1f));
	}
}

void ATicTacBoard::ClearBoard()
{
	for (ATicTacGridCell* Cell : GridCells)
	{
		if (Cell)
		{
			Cell->Destroy();
		}
	}

	GridCells.Empty();
}

void ATicTacBoard::ResetAllCells()
{
	for (ATicTacGridCell* Cell : GridCells)
	{
		if (Cell)
		{
			Cell->ResetCell();
		}
	}
}

ATicTacGridCell* ATicTacBoard::GetCellByIndex(int32 Index) const
{
	if (IsValidIndex(Index))
	{
		return GridCells[Index];
	}

	return nullptr;
}

ATicTacGridCell* ATicTacBoard::GetCellByRowColumn(int32 Row, int32 Column) const
{
	if (IsValidRowColumn(Row, Column))
	{
		int32 Index = RowColumnToIndex(Row, Column);
		return GridCells[Index];
	}

	return nullptr;
}

void ATicTacBoard::IndexToRowColumn(int32 Index, int32& OutRow, int32& OutColumn) const
{
	OutRow = Index / BoardSize;
	OutColumn = Index % BoardSize;
}

int32 ATicTacBoard::RowColumnToIndex(int32 Row, int32 Column) const
{
	return Row * BoardSize + Column;
}

FVector ATicTacBoard::CalculateCellWorldLocation(int32 Row, int32 Column) const
{
	// 计算相对于棋盘根组件的位置
	float X = Row * CellSpacing;
	float Y = Column * CellSpacing;
	float Z = 0.0f;

	// 转换为世界坐标
	FVector LocalLocation(X, Y, Z);
	return GetActorLocation() + GetActorRotation().RotateVector(LocalLocation);
}

bool ATicTacBoard::SetCellOwner(int32 Index, EPlayerType Player)
{
	ATicTacGridCell* Cell = GetCellByIndex(Index);
	if (!Cell)
	{
		return false;
	}

	Cell->SetOwningPlayer(Player);
	return true;
}

EPlayerType ATicTacBoard::GetCellOwner(int32 Index) const
{
	ATicTacGridCell* Cell = GetCellByIndex(Index);
	if (Cell)
	{
		return Cell->GetOwningPlayer();
	}

	return EPlayerType::None;
}

bool ATicTacBoard::IsCellOccupied(int32 Index) const
{
	ATicTacGridCell* Cell = GetCellByIndex(Index);
	if (Cell)
	{
		return Cell->IsOccupied();
	}

	return false;
}

void ATicTacBoard::HighlightWinningCells(const TArray<int32>& WinningCells)
{
	for (int32 Index : WinningCells)
	{
		ATicTacGridCell* Cell = GetCellByIndex(Index);
		if (Cell)
		{
			Cell->SetWinningCell(true);
		}
	}
}

void ATicTacBoard::ClearHighlights()
{
	for (ATicTacGridCell* Cell : GridCells)
	{
		if (Cell)
		{
			Cell->SetWinningCell(false);
		}
	}
}

void ATicTacBoard::DrawDebugGrid()
{
	if (!GetWorld())
	{
		return;
	}

	float BoardWidth = (BoardSize - 1) * CellSpacing;
	FVector CenterOffset = FVector(-BoardWidth / 2.0f, -BoardWidth / 2.0f, 0.0f);
	FVector GridOrigin = CalculateCellWorldLocation(0,0) + CenterOffset;

	for (int32 i = 0; i <= BoardSize; i++)
	{
		// 横线
		FVector Start = GridOrigin + FVector(0.0f, i * CellSpacing, 10.0f);
		FVector End = GridOrigin + FVector(BoardWidth, i * CellSpacing, 10.0f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, -1.0f, 0, 2.0f);

		// 竖线
		Start = GridOrigin + FVector(i * CellSpacing, 0.0f, 10.0f);
		End = GridOrigin + FVector(i * CellSpacing, BoardWidth, 10.0f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, -1.0f, 0, 2.0f);
	}

	for (int32 i = 0; i < GridCells.Num(); i++)
	{
		ATicTacGridCell* Cell = GridCells[i];
		if (Cell)
		{
			FVector CellLocation = Cell->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
			FString Label = FString::Printf(TEXT("%d"), i);
			DrawDebugString(GetWorld(), CellLocation, Label, nullptr, FColor::White, 0.0f, true, 1.0f);
		}
	}
}

void ATicTacBoard::PrintBoardState() const
{
	UE_LOG(LogTemp, Log, TEXT("========== BoardState =========="));
	UE_LOG(LogTemp, Log, TEXT("size: %d×%d"), BoardSize, BoardSize);
	UE_LOG(LogTemp, Log, TEXT("grid num: %d"), GridCells.Num());
	UE_LOG(LogTemp, Log, TEXT("grid spacing: %.1f"), CellSpacing);

	// 打印棋盘布局
	for (int32 Row = 0; Row < BoardSize; Row++)
	{
		FString RowStr = TEXT("");

		for (int32 Column = 0; Column < BoardSize; Column++)
		{
			int32 Index = RowColumnToIndex(Row, Column);
			ATicTacGridCell* Cell = GetCellByIndex(Index);

			FString CellStr;
			if (Cell)
			{
				switch (Cell->GetOwningPlayer())
				{
				case EPlayerType::None:
					CellStr = TEXT("·");
					break;
				case EPlayerType::Player1:
					CellStr = TEXT("X");
					break;
				case EPlayerType::Player2:
					CellStr = TEXT("O");
					break;
				}
			}
			else
			{
				CellStr = TEXT("?");
			}

			RowStr += CellStr + TEXT(" ");
		}

		UE_LOG(LogTemp, Log, TEXT("%s"), *RowStr);
	}

	UE_LOG(LogTemp, Log, TEXT("=============================="));
}

ATicTacGridCell* ATicTacBoard::SpawnGridCell(int32 Index, int32 Row, int32 Column, const FVector& Location)
{
	if (!GridCellClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ATicTacGridCell* NewCell = GetWorld()->SpawnActor<ATicTacGridCell>(
		GridCellClass,
		Location, // 世界坐标
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (NewCell)
	{
		NewCell->InitializeCell(Index, Row, Column);
		NewCell->SetActorScale3D(FVector(CellScale));
		NewCell->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}

	return NewCell;
}

bool ATicTacBoard::IsValidIndex(int32 Index) const
{
	return Index >= 0 && Index < GridCells.Num();
}

bool ATicTacBoard::IsValidRowColumn(int32 Row, int32 Column) const
{
	return Row >= 0 && Row < BoardSize && Column >= 0 && Column < BoardSize;
}

void ATicTacBoard::LoadConfigFromGameMode()
{
	ATicTacGameMode* GameMode = Cast<ATicTacGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		UTicTacGameConfig* Config = GameMode->GameConfig;
		if (Config)
		{
			BoardSize = Config->BoardSize;
			CellSpacing = Config->GridSpacing;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("no config"));
		}
	}
}
