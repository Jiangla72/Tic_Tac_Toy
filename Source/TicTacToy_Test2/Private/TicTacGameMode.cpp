// Copyright 2025


#include "TicTacGameMode.h"

#include "TicTacAIController.h"
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

	CurrentPlayer = EPlayerType::None;
	LastFirstPlayer = EPlayerType::None;
    TurnCount = 0;
    bIsGameActive = false;
    GameConfig = nullptr;
}

void ATicTacGameMode::ResetData()
{
	CurrentPlayer = EPlayerType::None;
	TurnCount = 0;
	bIsGameActive = false;
}

void ATicTacGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (!GameConfig)
	{
		///Game/文件夹路径/资源包名.资源名 资源包名==资源名
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

	if (GameConfig->bEnableAI)
	{
		InitializeAI(GameConfig->AIPlayerType, GameConfig->Difficulty);
	}

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
		if (Board)
		{
			Board->ReLoadBoard();
		}
		ATicTacGameState* TicTacGameState = GetTicTacGameState();
		if (!TicTacGameState)
		{
			return;
		}
		TicTacGameState->SetPlayerName(EPlayerType::Player1, GameConfig->Player1Name);
		TicTacGameState->SetPlayerName(EPlayerType::Player2, GameConfig->Player2Name);
		if (GameConfig->bEnableAI)
		{
			InitializeAI(GameConfig->AIPlayerType, GameConfig->Difficulty);
		}
		else
		{
			bAIEnabled = false;
			AIPlayerType = EPlayerType::None;
		}
	}
}

UTicTacGameConfig* ATicTacGameMode::GetGameConfig()
{
	return GameConfig; 
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
		TicTacGameState->ResetScores();
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
			}
		}
	}
	else if (Board)
	{
		Board->ResetAllCells();
	}

	UpdateCameraForBoard();

	ATicTacPlayerController* PC = Cast<ATicTacPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->HideAllUI();
		PC->ShowGameHUD();
		bIsGameActive = true;
	}
	LastFirstPlayer = GameConfig->FirstPlayer;
	SetCurPlayer(GameConfig->FirstPlayer);
}

void ATicTacGameMode::ReStartGame()
{
	//ResetData();
	UE_LOG(LogTemp, Warning, TEXT("========== ReStartGame=========="));

	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (TicTacGameState)
	{
		TicTacGameState->ClearBoard();
		TicTacGameState->SetGameState(EGameState::Playing);
	}

	if (Board)
	{
		Board->ResetAllCells();
		Board->ClearHighlights();
	}

	UpdateCameraForBoard();

	ATicTacPlayerController* PC = Cast<ATicTacPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->HideGameOver();
		PC->ShowGameHUD();
		bIsGameActive = true;
	}

	// Switch first player for this round
	EPlayerType NewFirstPlayer = (LastFirstPlayer == EPlayerType::Player1)
		? EPlayerType::Player2
		: EPlayerType::Player1;
	LastFirstPlayer = NewFirstPlayer;
	TurnCount = 0;
	SetCurPlayer(NewFirstPlayer);
}

void ATicTacGameMode::StartNextGame()
{
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
	}
}

void ATicTacGameMode::TogglePause()
{
	UE_LOG(LogTemp, Warning, TEXT("==========TogglePause=========="));
	bool bIsPaused = UGameplayStatics::IsGamePaused(this);
	UGameplayStatics::SetGamePaused(this, !bIsPaused);
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	ATicTacPlayerController* PC = Cast<ATicTacPlayerController>(GetWorld()->GetFirstPlayerController());
	PC->TogglePause();
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
	SetCurPlayer(CurrentPlayer);
}

void ATicTacGameMode::SetCurPlayer(EPlayerType Player)
{
	CurrentPlayer = Player;
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	TicTacGameState->SetCurPlayer(Player);
	ATicTacPlayerController* PC = Cast<ATicTacPlayerController>(GetWorld()->GetFirstPlayerController());

	UE_LOG(LogTemp, Log, TEXT("SetCurPlayer: %d"), (int32)Player);

	if (bAIEnabled && CurrentPlayer == AIPlayerType)
	{
		UE_LOG(LogTemp, Log, TEXT("AI Turn"));
		PC->SetInputModeUIOnly();
		AIExecuteMove();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Player Turn"));
		PC->SetInputModeGameAndUI();
	}
}

bool ATicTacGameMode::ProcessPlayerMove(int32 CellIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("==========ProcessPlayerMove=========="));
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
	UE_LOG(LogTemp, Warning, TEXT("==========CheckGameResult=========="));
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
	bool bAllowDiagonal = true;
	if (GameConfig)
	{
		bAllowDiagonal = GameConfig->bAllowDiagonalWins;
	}
	if (bAllowDiagonal)
	{
		if (CheckDiagonalWin(Player, OutWinningCells))
			return true;
		if (CheckAntiDiagonalWin(Player, OutWinningCells))
			return true;
	}


	return false;
}

bool ATicTacGameMode::CheckDrawCondition() const
{
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (!TicTacGameState) return false;
	if (GameConfig && !GameConfig->bEnableDrawCondition) 
		return false;

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

void ATicTacGameMode::InitializeAI(EPlayerType InAIPlayer, EAIDifficulty Difficulty)
{
	if (!AIController)
	{
		AIController = NewObject<UTicTacAIController>(this);
	}

	if (AIController)
	{
		AIController->Initialize(this, InAIPlayer);
		AIController->SetDifficulty(Difficulty);

		bAIEnabled = true;
		AIPlayerType = InAIPlayer;

		UE_LOG(LogTemp, Warning, TEXT("InitializeAI，Player: %d, defficulty: %d"), AIPlayerType, (int32)Difficulty);
	}
}

void ATicTacGameMode::AIExecuteMove()
{
	if (!AIController || !bAIEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI UnEnable"));
		return;
	}

	//int32 BestMove = AIController->MakeMove();

	FTimerHandle AIThinkTimer;
	GetWorld()->GetTimerManager().SetTimer(
		AIThinkTimer,
		[this]()
		{
			int32 BestMove = AIController->MakeMove();
			if (BestMove >= 0)
			{
				ProcessPlayerMove(BestMove);
			}
		},
		0.1f, 
		false
	);
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
		for (int32 Col = 0; Col < BoardSize; ++Col)
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

	for (int32 Col = 0; Col < BoardSize; ++Col)
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

void ATicTacGameMode::UpdateCameraForBoard()
{
	if (!Board || !GameConfig)
	{
		return;
	}

	ATicTacPlayerController* PC = Cast<ATicTacPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC)
	{
		return;
	}

	ATicTacCameraPawn* CameraPawn = Cast<ATicTacCameraPawn>(PC->GetPawn());
	if (!CameraPawn)
	{
		return;
	}

	float BoardSizeFloat = static_cast<float>(GameConfig->BoardSize);
	float CalculatedHeight = 400.0f + (BoardSizeFloat - 3.0f) * 150.0f;

	CameraPawn->CameraHeight = CalculatedHeight;
	CameraPawn->ApplyCameraSettings();

	FVector BoardCenter = Board->GetActorLocation();
	CameraPawn->FocusOnLocation(BoardCenter);
}
