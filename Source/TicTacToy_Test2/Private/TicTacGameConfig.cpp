// Copyright 2025


#include "TicTacGameConfig.h"

UTicTacGameConfig::UTicTacGameConfig()
{
	BoardSize = 3;
	WinConditionCount = 3;
	GridSpacing = 120.0f;
	GridSize = 100.0f;
	
	Player1Name = TEXT("Player 1");
	Player2Name = TEXT("Player 2");
	Player1Color = FLinearColor(255,255,255,255);
	Player2Color = FLinearColor(255, 255, 255, 255);
	bEnableAI = true;
	AIPlayerType = EPlayerType::Player2;
	Difficulty = EAIDifficulty::Easy;

	bAllowDiagonalWins = true;
	bAllowUndo = true;
	bEnableTurnTimeLimit = false;
	TurnTimeLimit = 0.0f;
	bEnableSoundEffects = true;
	bEnableBackgroundMusic = true;

	UIScale = 1.0f;
	bShowGridLines = true;
	bHighlightWinningCells = true;
	PieceDropDuration = 0.3f;
	WinLineAnimationDuration = 1.5f;
}

bool UTicTacGameConfig::IsValidConfig() const
{
	if (!(BoardSize >= 3 && WinConditionCount >= 3 && WinConditionCount <= BoardSize && GridSpacing > 0.0f && GridSize > 0.0f))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid TicTacGameConfig: BoardSize=%d, WinConditionCount=%d, GridSpacing=%f, GridSize=%f"), BoardSize, WinConditionCount, GridSpacing, GridSize);
		return false;
	}
	return true;
}

