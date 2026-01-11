// Copyright 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TicTacGameTypes.h"
#include "TicTacGameMode.generated.h"

/**
 * 
 */

class ATicTacBoard;
class UTicTacGameConfig;
class ATicTacGameState;

UCLASS()
class TICTACTOY_TEST2_API ATicTacGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ATicTacGameMode();

	//void Init();
	void ResetData();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Config")
	UTicTacGameConfig* GameConfig;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void LoadGameConfig(UTicTacGameConfig* NewConfig);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void ReStartGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void EndGame(EPlayerType Winner, bool bIsDraw);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void TogglePause();

	EPlayerType GetCurrentPlayer() const {return CurrentPlayer;}

	UFUNCTION(BlueprintCallable, Category= "Game")
	void SwitchPlayer();

	UFUNCTION(BlueprintCallable, Category = "Game")
	bool ProcessPlayerMove(int32 CellIndex);

	UFUNCTION(BlueprintCallable, Category = "Game")
	FGameResult CheckGameResult();

	UFUNCTION(BlueprintPure, Category = "Game")
	bool CheckWinCondition(EPlayerType Player, TArray<int32>& OutWinningCells);

	UFUNCTION(BlueprintPure, Category = "Game")
	bool CheckDrawCondition() const;

	UFUNCTION(BlueprintPure, Category = "Game")
	ATicTacGameState* GetTicTacGameState() const;

	UFUNCTION(BlueprintPure, Category = "Game")
	int32 GetBoardSize() const;

	UFUNCTION(BlueprintPure, Category = "Game")
	int32 GetWinConditionCount() const;

protected:
	bool CheckHorizontalWin(EPlayerType Player, TArray<int32>& OUT OutCells);
	bool CheckVerticalWin(EPlayerType Player, TArray<int32>& OUT OutCells);
	bool CheckDiagonalWin(EPlayerType Player, TArray<int32>& OUT OutCells);
	bool CheckAntiDiagonalWin(EPlayerType Player, TArray<int32>& OUT OutCells);

	void IndexToRowColumn(int32 Index, int32& OutRow, int32& OutColumn) const;
	int32 RowColumnToIndex(int32 Row, int32 Column) const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Game")
	ATicTacBoard* Board;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Config")
	TSubclassOf<APawn> CameraPawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Config")
	TSubclassOf<ATicTacBoard> BoardClass;

	UPROPERTY(BlueprintReadOnly, Category = "Game State")
	EPlayerType CurrentPlayer;

	UPROPERTY(BlueprintReadOnly, Category = "Game State")
	int32 TurnCount;

	UPROPERTY(BlueprintReadOnly, Category = "Game State")
	bool bIsGameActive;
};
