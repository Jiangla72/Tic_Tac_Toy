#pragma once
#include "CoreMinimal.h"
#include "TicTacGameTypes.generated.h"

UENUM(BlueprintType)
enum class EGameState : uint8
{
	NotStarted UMETA(DisplayName = "Not Started"),
	Playing UMETA(DisplayName = "Playing"),
	Paused UMETA(DisplayName = "Paused"),
	GameOver UMETA(DisplayName = "Game Over"),
	Draw UMETA(DisplayName = "Draw")
};

UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	None UMETA(DisplayName = "None"),
	Player1 UMETA(DisplayName = "Player 1"),
	Player2 UMETA(DisplayName = "Player 2")
};

USTRUCT(BlueprintType)
struct FGridCellData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	int32 Index = -1;

	UPROPERTY(BlueprintReadWrite, Category="Grid")
	int32 Row = -1;

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	int32 Column = -1;

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	EPlayerType Owner = EPlayerType::None;

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	bool bIsOccupied = false;

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	bool bIsWinningCell = false;

	FGridCellData()
	{}

	FGridCellData(int32 InIndex, int32 InRow, int32 InColumn)
		:Index(InIndex), Row(InRow), Column(InColumn)
	{}
};

USTRUCT(BlueprintType)
struct FGameResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Game Result")
	bool bHasWinner = false;

	UPROPERTY(BlueprintReadWrite, Category = "Game Result")
	EPlayerType Winner = EPlayerType::None;
	
	UPROPERTY(BlueprintReadWrite, Category = "Game Result")
	TArray<int32> WinningCells;

	UPROPERTY(BlueprintReadWrite, Category = "Game Result")
	bool bIsDraw = false;

	FGameResult()
	{}

};