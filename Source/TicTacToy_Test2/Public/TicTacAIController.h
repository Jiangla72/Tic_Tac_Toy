// Copyright 2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TicTacGameTypes.h"
#include "TicTacAIController.generated.h"

class ATicTacGameState;
class ATicTacGameMode;

/**
 */
UCLASS()
class TICTACTOY_TEST2_API UTicTacAIController : public UObject
{
    GENERATED_BODY()

public:
    void Initialize(ATicTacGameMode* InGameMode, EPlayerType InAIPlayer);

    UFUNCTION(BlueprintCallable, Category = "AI")
    int32 MakeMove();

    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetDifficulty(EAIDifficulty NewDifficulty) { Difficulty = NewDifficulty; }

    UFUNCTION(BlueprintPure, Category = "AI")
    EAIDifficulty GetDifficulty() const { return Difficulty; }

protected:
    int32 MakeRandomMove();

    int32 MakeMediumMove();

    int32 MakeHardMove();

    int32 Minimax(TArray<EPlayerType>& Board, int32 Depth, bool bIsMaximizing, int32 Alpha, int32 Beta);

    int32 EvaluateBoard(const TArray<EPlayerType>& Board);

    int32 EvaluateLine(
        const TArray<EPlayerType>& Board,
        int32 StartRow,
        int32 StartCol,
        int32 RowStep,
        int32 ColStep,
        int32 Length,
        int32 WinLength,
        EPlayerType Opponent);

    bool CheckWinner(const TArray<EPlayerType>& Board, EPlayerType Player);

    TArray<int32> GetEmptyCells(const TArray<EPlayerType>& Board);

    void SimulateMove(TArray<EPlayerType>& Board, int32 CellIndex, EPlayerType Player);

    void UndoMove(TArray<EPlayerType>& Board, int32 CellIndex);

    uint32 CalculateBoardHash(const TArray<EPlayerType>& Board);
    
    TArray<int32> SortMovesByPriority(const TArray<int32>& Moves, const TArray<EPlayerType>& Board);
    
    int32 CountThreats(const TArray<EPlayerType>& Board, int32 LastMove, EPlayerType Player);
    
protected:
    TMap<uint32, int32> TranspositionTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    EAIDifficulty Difficulty;

    UPROPERTY()
    EPlayerType AIPlayer;

    UPROPERTY()
    ATicTacGameMode* GameMode;

    UPROPERTY()
    ATicTacGameState* GameState;

};