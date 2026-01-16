// Copyright 2025

#pragma once

#include "CoreMinimal.h"
#include "TicTacGameTypes.h"
#include "Engine/DataAsset.h"
#include "TicTacGameConfig.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class TICTACTOY_TEST2_API UTicTacGameConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UTicTacGameConfig();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Board Settings", meta = (ClampMin = "3", ClampMax = "10"))
	int32 BoardSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Board Settings", meta = (ClampMin = "3", ClampMax = "10"))
	int32 WinConditionCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Board Settings", meta = (ClampMin = "50", ClampMax = "200"))
	float GridSpacing;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Board Settings", meta = (ClampMin = "30", ClampMax = "150"))
	float GridSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
	FString Player1Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
	FString Player2Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
	FLinearColor Player1Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
	FLinearColor Player2Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
	EPlayerType FirstPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bEnableAI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EPlayerType AIPlayerType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EAIDifficulty Difficulty;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	bool bAllowDiagonalWins;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	bool bEnableDrawCondition;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	//bool bEnableTimedTurns;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	//bool bAllowRematches;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	//bool bEnableHints;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	//bool bEnableAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	bool bAllowUndo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	bool bEnableTurnTimeLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules", meta = (ClampMix = "0", ClampMax = "300"))
	float TurnTimeLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	bool bEnableSoundEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	bool bEnableBackgroundMusic;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	//bool bShowMoveHistory;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	//bool bEnableTutorialMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Settings", meta = (Clamp = "0.5", ClampMax = "2.0"))
	float UIScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Settings")
	bool bShowGridLines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Settings")
	bool bHighlightWinningCells;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Settings")
	float PieceDropDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Settings")
	float WinLineAnimationDuration;

	UFUNCTION(BlueprintCallable, Category= "Config")
	bool IsValidConfig() const;

	UFUNCTION(BlueprintPure, Category = "Config")
	int32 GetTotalGridCount() const { return BoardSize * BoardSize; }
};
