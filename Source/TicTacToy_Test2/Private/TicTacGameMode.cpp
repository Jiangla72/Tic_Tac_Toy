// Copyright 2025


#include "TicTacGameMode.h"
#include "TicTacGameConfig.h"
#include "TicTacGameState.h"
#include "TicTacPlayerController.h"
#include "TicTacBoard.h"
#include "TicTacCameraPawn.h"
#include "Kismet/GameplayStatics.h"

ATicTacGameMode::ATicTacGameMode()
{
	//基类成员
	GameStateClass = ATicTacGameState::StaticClass();
	PlayerControllerClass = ATicTacPlayerController::StaticClass();
	PrimaryActorTick.bCanEverTick = true;

	CurrentPlayer = EPlayerType::Player1;
    TurnCount = 0;
    bIsGameActive = false;
    GameConfig = nullptr;
}

void ATicTacGameMode::ResetData()
{
	CurrentPlayer = EPlayerType::Player1;
	TurnCount = 0;
	bIsGameActive = false;
}

void ATicTacGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (!GameConfig)
	{
		GameConfig = LoadObject<UTicTacGameConfig>(nullptr, TEXT("/Game/Config/DA_GameConfig_Default.DA_GameConfig_Default"));

		if (GameConfig)
		{
			UE_LOG(LogTemp, Log, TEXT("Load Default GameConfig Success"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Load Default GameConfig Faild!"));
			return;
		}
	}

	if (!GameConfig->IsValidConfig())
	{
		UE_LOG(LogTemp, Error, TEXT("GameConfig IsNotValid"));
		return;
	}

	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (!TicTacGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("cant find game state"));
		return;
	}

	TicTacGameState->SetPlayerName(EPlayerType::Player1, GameConfig->Player1Name);
	TicTacGameState->SetPlayerName(EPlayerType::Player2, GameConfig->Player2Name);

	FTimerHandle StartGameTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		StartGameTimerHandle,
		this,
		&ATicTacGameMode::StartNewGame,
		0.1f,  // 延迟0.1秒
		false  // 不循环
	);

	//if (GameConfig == nullptr)
	//{
	//	//FString ConfigPath = FPaths::ProjectContentDir() + TEXT("");
	//	FString ConfigPath = TEXT("/Game/Data/DA_GameConfig_Default.DA_GameConfig_Default");
	//	GameConfig = Cast<UTicTacGameConfig>(StaticLoadObject(UTicTacGameConfig::StaticClass(), nullptr, *ConfigPath));
	//	if (GameConfig!=nullptr && GameConfig->IsValidConfig())
	//	{
	//		APlayerController* PC = GetWorld()->GetFirstPlayerController();
	//		if (PC)
	//		{
	//			PC->bShowMouseCursor = true;
	//			PC->bEnableClickEvents = true;
	//			PC->bEnableMouseOverEvents = true;

	//			FVector CameraLocation(0.0f, -500.0f, 300.0f);
	//			FRotator CameraRotation(-20.0f, 0.0f, 0.0f);
	//			PC->SetControlRotation(CameraRotation);

	//			if (APawn* Pawn = PC->GetPawn())
	//			{
	//				Pawn->SetActorLocation(CameraLocation);
	//			}
	//		}
	//		StartNewGame();
	//		bIsGameActive = true;
	//	}
	//}
}

void ATicTacGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bIsGameActive && GameConfig && GameConfig->bEnableTurnTimeLimit)
	{
	}

}

void ATicTacGameMode::LoadGameConfig(UTicTacGameConfig* NewConfig)
{
	if (NewConfig && NewConfig->IsValidConfig())
	{
		GameConfig = NewConfig;
		if (bIsGameActive)
		{
			ReStartGame();
		}
	}
}

void ATicTacGameMode::StartNewGame()
{
	if (!GameConfig)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("StartNewGame"));
	ResetData();
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (TicTacGameState)
	{
		TicTacGameState->InitialBoard(GameConfig->BoardSize);
		TicTacGameState->SetGameState(EGameState::Playing);
		TicTacGameState->OnPlayerChanged.Broadcast(CurrentPlayer);
	}

	if (!Board && BoardClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		Board = GetWorld()->SpawnActor<ATicTacBoard>(
			BoardClass,
			FVector(0.0f, 0.0f, 0.0f),
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (Board)
		{
			ATicTacPlayerController* PC = Cast<ATicTacPlayerController>(GetWorld()->GetFirstPlayerController());
			if (PC)
			{
				PC->SetBoard(Board);
				ATicTacCameraPawn* CameraPawn = Cast<ATicTacCameraPawn>(PC->GetPawn());
				if (CameraPawn)
				{
					float BoardSizeFloat = static_cast<float>(GameConfig->BoardSize);
					float CalculatedHeight = 400.0f + (BoardSizeFloat - 3.0f) * 150.0f;

					CameraPawn->CameraHeight = CalculatedHeight;
					CameraPawn->ApplyCameraSettings();

					FVector BoardCenter = Board->GetActorLocation();
					CameraPawn->FocusOnLocation(BoardCenter);

					UE_LOG(LogTemp, Log, TEXT("change camera size: %d×%d, height: %.1f"),
						GameConfig->BoardSize,
						GameConfig->BoardSize,
						CalculatedHeight);
				}
			}
		}
	}
	else if (Board)
	{
		// 如果Board已存在，重置它
		Board->ResetAllCells();
	}

	ATicTacPlayerController* PC = Cast<ATicTacPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->ShowGameHUD();
		bIsGameActive = true;
	}
}

void ATicTacGameMode::ReStartGame()
{
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (TicTacGameState)
	{
		TicTacGameState->ClearBoard();
		TicTacGameState->SetGameState(EGameState::Playing);
		TicTacGameState->OnPlayerChanged.Broadcast(CurrentPlayer);
	}

	if (Board)
	{
		Board->ResetAllCells();
		Board->ClearHighlights();
	}

	ResetData();

	ATicTacPlayerController* PC = Cast<ATicTacPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->HideGameOver();
		PC->ShowGameHUD();
		PC->SetInputModeGameAndUI();
		bIsGameActive = true;

	}
}

void ATicTacGameMode::EndGame(EPlayerType Winner, bool bIsDraw)
{
	bIsGameActive = false;

	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (TicTacGameState)
	{
		if (bIsDraw)
		{
			TicTacGameState->SetGameState(EGameState::Draw);
		}
		else
		{
			TicTacGameState->SetGameState(EGameState::GameOver);
			TicTacGameState->AddScore(Winner);
		}

		TicTacGameState->OnGameEnded.Broadcast(Winner, bIsDraw);

		if (Winner == EPlayerType::None)
		{
			UE_LOG(LogTemp, Warning, TEXT("========== GameOver: Draw!=========="));
		}
		else
		{
			FString WinnerName = TicTacGameState->GetPlayerName(Winner);
			UE_LOG(LogTemp, Warning, TEXT("========== GameOver: %s Win!=========="), *WinnerName);
		}

		ATicTacPlayerController* PC = Cast<ATicTacPlayerController>(GetWorld()->GetFirstPlayerController());
		if (PC)
		{
			FTimerHandle ShowGameOverTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				ShowGameOverTimerHandle,
				[PC]()
				{
					PC->ShowGameOver();
				},
				1.0f,  // 延迟1秒
				false
			);
		}
	}
}

void ATicTacGameMode::TogglePause()
{
	bool bIsPaused = UGameplayStatics::IsGamePaused(this);
	UGameplayStatics::SetGamePaused(this, !bIsPaused);
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (TicTacGameState)
	{
		if (bIsPaused)
		{
			TicTacGameState->SetGameState(EGameState::Playing);
			bIsGameActive = true;
		}
		else
		{
			TicTacGameState->SetGameState(EGameState::Paused); 
			bIsGameActive = false;
		}
	}
}

void ATicTacGameMode::SwitchPlayer()
{
	CurrentPlayer = (CurrentPlayer == EPlayerType::Player1)
		? EPlayerType::Player2
		: EPlayerType::Player1;
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (TicTacGameState)
	{
		TicTacGameState->OnPlayerChanged.Broadcast(CurrentPlayer);
	}
}

bool ATicTacGameMode::ProcessPlayerMove(int32 CellIndex)
{
	if (!bIsGameActive)
	{
		return false;
	}

	int32 BoardSize = GetBoardSize();
	if (CellIndex < 0 || CellIndex >= BoardSize * BoardSize)
	{
		return false;
	}

	ATicTacGameState* TicTacGameState = GetTicTacGameState();

	if (TicTacGameState->IsCellOccupied(CellIndex))
	{
		return false;
	}

	TicTacGameState->SetCellOwner(CellIndex, CurrentPlayer);
	if (Board)
	{
		Board->SetCellOwner(CellIndex, CurrentPlayer);
	}
	TurnCount++;

	FGameResult Result = CheckGameResult();
	if (Result.bHasWinner)
	{
		TicTacGameState->SetWinningCells(Result.WinningCells);
		if (Board)
		{
			Board->HighlightWinningCells(Result.WinningCells);
		}
		EndGame(Result.Winner, Result.bIsDraw);
	}
	else if (Result.bIsDraw)
	{
		EndGame(EPlayerType::None, Result.bIsDraw);
	}
	else
	{
		SwitchPlayer();
	}
	return true;
}

FGameResult ATicTacGameMode::CheckGameResult()
{
	FGameResult Result;
	TArray<int32> WinningCells;

	if (CheckWinCondition(CurrentPlayer, WinningCells))
	{
		Result.bHasWinner = true;
		Result.Winner = CurrentPlayer;
		Result.WinningCells = WinningCells;
		Result.bIsDraw = false;
		return Result;
	}

	if (CheckDrawCondition())
	{
		Result.bHasWinner = false;
		Result.Winner = EPlayerType::None;
		Result.bIsDraw = true;
		return Result;
	}

	return Result;
}

bool ATicTacGameMode::CheckWinCondition(EPlayerType Player, TArray<int32>& OutWinningCells)
{
	OutWinningCells.Empty();

	if (CheckHorizontalWin(Player, OutWinningCells))
		return true;
	if (CheckVerticalWin(Player, OutWinningCells))
		return true;
	if (CheckDiagonalWin(Player, OutWinningCells)) 
		return true;
	if (CheckAntiDiagonalWin(Player, OutWinningCells)) 
		return true;


	return false;
}

bool ATicTacGameMode::CheckDrawCondition() const
{
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (!TicTacGameState) return false;

	int32 TotalCells = GetBoardSize() * GetBoardSize();
	for (int32 i = 0; i < TotalCells; i++)
	{
		if (!TicTacGameState->IsCellOccupied(i))
		{
			return false;
		}
	}

	return true;
}

ATicTacGameState* ATicTacGameMode::GetTicTacGameState() const
{
	return GetGameState<ATicTacGameState>();
}

int32 ATicTacGameMode::GetBoardSize() const
{
	if (GameConfig)
	{
		return GameConfig->BoardSize;
	}
	return 3;
}

int32 ATicTacGameMode::GetWinConditionCount() const
{
	if (GameConfig)
	{
		return GameConfig->WinConditionCount;
	}
	return 3;
}

bool ATicTacGameMode::CheckHorizontalWin(EPlayerType Player, TArray<int32>& OutCells)
{
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (!TicTacGameState) return false;
	int32 BoardSize = GetBoardSize();
	int32 WinCount = GetWinConditionCount();
	bool bWin = false;
	 
	for (int32 Row = 0; Row < BoardSize; ++Row)
	{
		int32 StartCol = 0;
		TArray<int32> CurrentWinningCells;
		for (int32 Col = 0; Col <= BoardSize; ++Col)
		{
			int32 nIndex = RowColumnToIndex(Row, Col);

			if (TicTacGameState->GetCellOwner(nIndex) != Player)
			{
				StartCol = Col + 1;
				CurrentWinningCells.Empty();
			}
			else
			{
				CurrentWinningCells.Add(nIndex);
				if (Col - StartCol + 1 == WinCount)
				{
					bWin = true;
					break;
				}
			} 
		}
		if (bWin)
		{
			OutCells = CurrentWinningCells;
			break;
		}
	}

	return bWin;
}

bool ATicTacGameMode::CheckVerticalWin(EPlayerType Player, TArray<int32>& OutCells)
{
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (!TicTacGameState) return false;
	int32 BoardSize = GetBoardSize();
	int32 WinCount = GetWinConditionCount();
	bool bWin = false;

	for (int32 Col = 0; Col <= BoardSize; ++Col)
	{
		int32 StartRow = 0;
		TArray<int32> CurrentWinningCells;
		for (int32 Row = 0; Row < BoardSize; ++Row)
		{
			int32 nIndex = RowColumnToIndex(Row, Col);

			if (TicTacGameState->GetCellOwner(nIndex) != Player)
			{
				StartRow = Row + 1;
				CurrentWinningCells.Empty();
			}
			else
			{
				CurrentWinningCells.Add(nIndex);
				if (Row - StartRow + 1 == WinCount)
				{
					bWin = true;
					break;
				}
			}
		}
		if (bWin)
		{
			OutCells = CurrentWinningCells;
			break;
		}
	}

	return bWin;
}

bool ATicTacGameMode::CheckDiagonalWin(EPlayerType Player, TArray<int32>& OutCells)
{
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (!TicTacGameState) return false;
	int32 BoardSize = GetBoardSize();
	int32 WinCount = GetWinConditionCount();

	for (int32 StartRow = 0; StartRow <= BoardSize - WinCount; StartRow++)
	{
		for (int32 StartCol = 0; StartCol <= BoardSize - WinCount; StartCol++)
		{
			TArray<int32> TempCells;
			bool bIsWin = true;

			for (int32 i = 0; i < WinCount; i++)
			{
				int32 Index = RowColumnToIndex(StartRow + i, StartCol + i);
				if (TicTacGameState->GetCellOwner(Index) != Player)
				{
					bIsWin = false;
					break;
				}
				TempCells.Add(Index);
			}

			if (bIsWin)
			{
				OutCells = TempCells;
				return true;
			}
		}
	}

	return false;
}

bool ATicTacGameMode::CheckAntiDiagonalWin(EPlayerType Player, TArray<int32>& OutCells)
{
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (!TicTacGameState) return false;
	int32 BoardSize = GetBoardSize();
	int32 WinCount = GetWinConditionCount();
	
	for (int32 StartRow = 0; StartRow <= BoardSize - WinCount; StartRow++)
	{
		for (int32 StartCol = WinCount - 1; StartCol < BoardSize; StartCol++)
		{
			TArray<int32> TempCells;
			bool bIsWin = true;

			for (int32 i = 0; i < WinCount; i++)
			{
				int32 Index = RowColumnToIndex(StartRow + i, StartCol - i);
				if (TicTacGameState->GetCellOwner(Index) != Player)
				{
					bIsWin = false;
					break;
				}
				TempCells.Add(Index);
			}

			if (bIsWin)
			{
				OutCells = TempCells;
				return true;
			}
		}
	}

	return false;
}

void ATicTacGameMode::IndexToRowColumn(int32 Index, int32& OutRow, int32& OutColumn) const
{
	int32 BoardSize = GetBoardSize();
	OutRow = Index / BoardSize;
	OutColumn = Index % BoardSize;
}

int32 ATicTacGameMode::RowColumnToIndex(int32 Row, int32 Column) const
{
	int32 BoardSize = GetBoardSize();
	return Row * BoardSize + Column;
}
