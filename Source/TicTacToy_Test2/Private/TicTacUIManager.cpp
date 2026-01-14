// Copyright 2025

#include "TicTacUIManager.h"
#include "TicTacGameState.h"
#include "TicTacPlayerController.h"
#include "TicTacHUDWidget.h"
#include "TicTacGameOverWidget.h"

void UTicTacUIManager::Initialize(ATicTacPlayerController* InPlayerController)
{
	PlayerController = InPlayerController;
	if (PlayerController)
	{
		GameState = PlayerController->GetTicTacGameState();
		UE_LOG(LogTemp, Log, TEXT("UTicTacUIManager Initialize"));
	}
}

void UTicTacUIManager::UpdateGameHUD()
{
	if (!GameState || !PlayerController)
		return;

	UpdatePlayerName(EPlayerType::Player1, GameState->GetPlayerName(EPlayerType::Player1));
	UpdatePlayerName(EPlayerType::Player2, GameState->GetPlayerName(EPlayerType::Player2));
	UpdateScoreDisplay(EPlayerType::Player1, GameState->GetScore(EPlayerType::Player1));
	UpdateScoreDisplay(EPlayerType::Player2, GameState->GetScore(EPlayerType::Player2));
}

void UTicTacUIManager::UpdateCurrentPlayerIndicator(EPlayerType CurrentPlayer)
{
	if (!PlayerController || !PlayerController->GameHUDWidget)
		return;

	UTicTacHUDWidget* Widget = Cast<UTicTacHUDWidget>(PlayerController->GameHUDWidget);
	if (Widget && GameState)
	{
		Widget->UpdateTurnText(GameState->GetPlayerName(CurrentPlayer), CurrentPlayer == EPlayerType::Player1);
	}
}

void UTicTacUIManager::UpdateScoreDisplay(EPlayerType Player, int32 NewScore)
{
	if (!PlayerController || !PlayerController->GameHUDWidget)
		return;

	UTicTacHUDWidget* Widget = Cast<UTicTacHUDWidget>(PlayerController->GameHUDWidget);
	if (Widget && GameState)
	{
		Widget->UpdateScore(GameState->GetScore(EPlayerType::Player1), GameState->GetScore(EPlayerType::Player2));
	}
}

void UTicTacUIManager::UpdatePlayerName(EPlayerType Player, FString Name)
{
	if (!PlayerController || !PlayerController->GameHUDWidget)
		return;

	UTicTacHUDWidget* Widget = Cast<UTicTacHUDWidget>(PlayerController->GameHUDWidget);
	if (Widget && GameState)
	{
		Widget->UpdateName(GameState->GetPlayerName(EPlayerType::Player1), GameState->GetPlayerName(EPlayerType::Player2));
	}
}

void UTicTacUIManager::ShowGameResult(EPlayerType Winner, bool bIsDraw)
{
	if (!PlayerController)
		return;

	FString WinnerName = bIsDraw ? TEXT("Draw!") : (GameState ? GameState->GetPlayerName(Winner) : TEXT(""));

	PlayerController->HideGameHUD();
	PlayerController->ShowGameOver();

	UTicTacGameOverWidget* Widget = Cast<UTicTacGameOverWidget>(PlayerController->GameOverWidget);
	if (Widget && GameState)
	{
		Widget->UpdateWinnerName(WinnerName);
		Widget->UpdateScore(bIsDraw ? 0 : GameState->GetScore(Winner));
	}
}
