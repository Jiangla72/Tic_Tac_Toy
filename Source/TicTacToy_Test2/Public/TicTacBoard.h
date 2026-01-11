// Copyright 2025

#pragma once

#include "CoreMinimal.h"
#include "TicTacGameTypes.h"
#include "GameFramework/Actor.h"
#include "TicTacBoard.generated.h"

class ATicTacGridCell;
class USceneComponent;
class UStaticMeshComponent;
class UTicTacGameConfig;

UCLASS()
class TICTACTOY_TEST2_API ATicTacBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATicTacBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BoardBaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Board Settings", meta = (ClampMin = "3", ClampMax = "10"))
	int32 BoardSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Board Settings", meta = (ClampMin = "50", ClampMax = "300"))
	float CellSpacing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Board Settings", meta = (ClampMin = "0.5", ClampMax = "2.0"))
	float CellScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Board Settings")
	TSubclassOf<ATicTacGridCell> GridCellClass;

	UPROPERTY(BlueprintReadOnly, Category = "Board")
	TArray<ATicTacGridCell*> GridCells;
public:
	UFUNCTION(BlueprintCallable, Category = "Board")
	void GenerateBoard(int32 Size);

	UFUNCTION(BlueprintCallable, Category = "Board")
	void ClearBoard();

	UFUNCTION(BlueprintCallable, Category = "Board")
	void ResetAllCells();

	UFUNCTION(BlueprintPure, Category = "Board")
	ATicTacGridCell* GetCellByIndex(int32 Index) const;

	UFUNCTION(BlueprintPure, Category = "Board")
	ATicTacGridCell* GetCellByRowColumn(int32 Row, int32 Column) const;

	UFUNCTION(BlueprintPure, Category = "Board")
	TArray<ATicTacGridCell*> GetAllCells() const { return GridCells; }

	UFUNCTION(BlueprintPure, Category = "Board")
	int32 GetTotalCellCount() const { return GridCells.Num(); }

	UFUNCTION(BlueprintPure, Category = "Board")
	void IndexToRowColumn(int32 Index, int32& OutRow, int32& OutColumn) const;

	UFUNCTION(BlueprintPure, Category = "Board")
	int32 RowColumnToIndex(int32 Row, int32 Column) const;

	UFUNCTION(BlueprintPure, Category = "Board")
	FVector CalculateCellWorldLocation(int32 Row, int32 Column) const;

	UFUNCTION(BlueprintCallable, Category = "Board")
	bool SetCellOwner(int32 Index, EPlayerType Player);

	UFUNCTION(BlueprintPure, Category = "Board")
	EPlayerType GetCellOwner(int32 Index) const;

	UFUNCTION(BlueprintPure, Category = "Board")
	bool IsCellOccupied(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Board")
	void HighlightWinningCells(const TArray<int32>& WinningCells);

	UFUNCTION(BlueprintCallable, Category = "Board")
	void ClearHighlights();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DrawDebugGrid();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void PrintBoardState() const;

protected:
	ATicTacGridCell* SpawnGridCell(int32 Index, int32 Row, int32 Column, const FVector& Location);

	bool IsValidIndex(int32 Index) const;

	bool IsValidRowColumn(int32 Row, int32 Column) const;

	void LoadConfigFromGameMode();
};
