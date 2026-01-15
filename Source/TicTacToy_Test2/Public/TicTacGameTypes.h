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

UENUM(BlueprintType)
enum class EAIDifficulty : uint8
{
	Easy        UMETA(DisplayName = "Easy"),
	Medium      UMETA(DisplayName = "Medium"),
	Hard        UMETA(DisplayName = "Hard"),
};

UCLASS()
class UMyEnum : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Enum")
	static TArray<FString> GetAllEnumDisplayNames_AIDifficulty()
	{
		
		TArray<FString> DisplayNames;
		UEnum* Enum = StaticEnum<EAIDifficulty>();

		if (Enum)
		{
			for (int32 i = 0; i < Enum->NumEnums(); i++)
			{
				DisplayNames.Add(Enum->GetDisplayNameTextByIndex(i).ToString());
			}
		}
		return DisplayNames;
	};

	UFUNCTION(BlueprintCallable, Category = "Enum")
	static EAIDifficulty GetEnumFromDisplayName_AIDifficulty(const FString& DisplayName)
	{
		UEnum* Enum = StaticEnum<EAIDifficulty>();

		if (Enum)
		{
			for (int32 i = 0; i < Enum->NumEnums(); i++)
			{
				if (Enum->GetDisplayNameTextByIndex(i).ToString() == DisplayName)
				{
					return static_cast<EAIDifficulty>(i);
				}
			}
		}
		return EAIDifficulty::Easy;
	};
};