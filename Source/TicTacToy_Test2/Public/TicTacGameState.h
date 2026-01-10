// Copyright 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TicTacGameTypes.h"
#include "TicTacGameState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBoardUpdatedDelegate, int32, CellIndex, EPlayerType, NewOwner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerChangedDelegate, EPlayerType, NewPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameEndedDelegate, EPlayerType, Winner, bool, bIsDraw);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreUpdatedDelegate, EPlayerType, Player, int32, NewScore);
/**
 * 
 */
UCLASS()
class TICTACTOY_TEST2_API ATicTacGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Board")
    int32 BoardSize;

    UPROPERTY(BlueprintReadOnly, Category = "Board")
    TArray<EPlayerType> BoardData;

    UPROPERTY(BlueprintReadOnly, Category = "Board")
    TArray<int32> WinningCells;

    UPROPERTY(BlueprintReadWrite, Category = "Players")
    FString Player1Name;

    UPROPERTY(BlueprintReadWrite, Category = "Players")
    FString Player2Name;

    UPROPERTY(BlueprintReadOnly, Category = "Players")
    int32 Player1Score;

    UPROPERTY(BlueprintReadOnly, Category = "Players")
    int32 Player2Score;

    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    EGameState CurrentGameState;

    EPlayerType playerTurn;
    UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBoardUpdatedDelegate OnBoardUpdated;
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnPlayerChangedDelegate OnPlayerChanged;
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnGameEndedDelegate OnGameEnded;
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnScoreUpdatedDelegate OnScoreUpdated;
public:
	ATicTacGameState();

    void Init();
    void UnInit();
public:
    void InitialBoard(int32 nSize);
    void ClearBoard();

    bool SetCellOwner(int32 nIndex, EPlayerType Owner);
	EPlayerType GetCellOwner(int32 nIndex) const;

    bool IsCellOccupied(int32 nIndex) const;
    TArray<int32> GetOccupiedCells() const;
    int32 GetOccupiedCellCount() const;

    bool IsWinningCell(int32 CellIndex) const;
    void SetWinningCells(const TArray<int32>& Cells);
	TArray<int32> GetWinningCells() const;

	TArray<int32> GetEmptyCells() const;

    void SetPlayerName(EPlayerType Player, const FString& Name);
    FString GetPlayerName(EPlayerType Player) const;

    void AddScore(EPlayerType Player, int32 Amount = 1);
    int32 GetScore(EPlayerType Player) const;
	void ResetScores();

	void SetGameState(EGameState NewState);
    EGameState GetGameState() const { return CurrentGameState; }

    EPlayerType GetCurrentPlayerTurn() const;
	bool IsBoardFull() const;
    void SwitchPlayerTurn();
protected:
    // 网络复制支持（未来扩展多人模式）
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "TicTacToe")
	EPlayerType CurrentPlayerTurn;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "TicTacToe")
	FGameResult GameResult;

};
