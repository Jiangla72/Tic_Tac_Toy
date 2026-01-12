// Copyright 2025


#include "TicTacUIManager.h"
#include "TicTacGameState.h"
#include "TicTacPlayerController.h"
#include "TicTacHUDWidget.h"

//UTicTacUIManager* UTicTacUIManager::Instance = nullptr;
//
//UTicTacUIManager* UTicTacUIManager::GetInstance(UWorld* World)
//{
//	if (!Instance && World)
//	{
//		Instance = Cast<UTicTacUIManager>(World->GetGameInstance());
//	}
//	return Instance;
//}

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
	if (!GameState)
		return;

	UpdateScoreDisplay(EPlayerType::Player1, GameState->GetScore(EPlayerType::Player1));
	UpdatePlayerName(EPlayerType::Player1, GameState->GetPlayerName(EPlayerType::Player1));
	UpdateScoreDisplay(EPlayerType::Player2, GameState->GetScore(EPlayerType::Player2));
	UpdatePlayerName(EPlayerType::Player2, GameState->GetPlayerName(EPlayerType::Player2));


	UE_LOG(LogTemp, Verbose, TEXT("UpdateGameHUD"));
}

void UTicTacUIManager::UpdateCurrentPlayerIndicator(EPlayerType CurrentPlayer)
{
	if (!GameState)
		return;
	if (!PlayerController || !PlayerController->GameHUDWidget)
	{
		return;
	}
	FString PlayerName = GameState->GetPlayerName(CurrentPlayer);
	UTicTacHUDWidget* widget = Cast<UTicTacHUDWidget>(PlayerController->GameHUDWidget);
	widget->UpdateTurnText(GameState->GetPlayerName(CurrentPlayer), CurrentPlayer == EPlayerType::Player1 ? true : false);
	UE_LOG(LogTemp, Log, TEXT("CurrentPlayer: %s"), *PlayerName);
}

void UTicTacUIManager::UpdateScoreDisplay(EPlayerType Player, int32 NewScore)
{
	if (!GameState)
		return;
	if (!PlayerController || !PlayerController->GameHUDWidget)
	{
		return;
	}
	FString PlayerName = GameState->GetPlayerName(Player);
	UTicTacHUDWidget* widget = Cast<UTicTacHUDWidget>(PlayerController->GameHUDWidget);
	widget->UpdateScore(GameState->GetScore(EPlayerType::Player1), GameState->GetScore(EPlayerType::Player2));

	UE_LOG(LogTemp, Log, TEXT("%s Score: %d"), *PlayerName, NewScore);
}

void UTicTacUIManager::UpdatePlayerName(EPlayerType Player, FString Name)
{
	if (!GameState)
		return;
	if (!PlayerController || !PlayerController->GameHUDWidget)
	{
		return;
	}
	FString PlayerName = GameState->GetPlayerName(Player);
	UTicTacHUDWidget* widget = Cast<UTicTacHUDWidget>(PlayerController->GameHUDWidget);
	widget->UpdateName(GameState->GetPlayerName(EPlayerType::Player1), GameState->GetPlayerName(EPlayerType::Player2));

	UE_LOG(LogTemp, Log, TEXT("UpdatePlayerName"));
}

void UTicTacUIManager::ShowGameResult(EPlayerType Winner, bool bIsDraw)
{
	if (bIsDraw)
	{
		UE_LOG(LogTemp, Warning, TEXT("========== IsDraw=========="));
	}
	else if (GameState)
	{
		FString WinnerName = GameState->GetPlayerName(Winner);
		UE_LOG(LogTemp, Warning, TEXT("========== %s Win!=========="), *WinnerName);
	}

	// 显示游戏结束界面
	if (PlayerController)
	{
		PlayerController->ShowGameOver();
	}
}
