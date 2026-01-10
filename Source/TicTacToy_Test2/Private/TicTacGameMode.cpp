// Copyright 2025


#include "TicTacGameMode.h"
#include "TicTacGameConfig.h"
#include "TicTacGameState.h"

ATicTacGameMode::ATicTacGameMode()
{
    CurrentPlayer = EPlayerType::Player1;
    TurnCount = 0;
    bIsGameActive = false;
    GameConfig = nullptr;
	GameStateClass = ATicTacGameState::StaticClass();
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

	if (GameConfig == nullptr)
	{
		//FString ConfigPath = FPaths::ProjectContentDir() + TEXT("");
		FString ConfigPath = TEXT("/Game/Data/DA_GameConfig_Default.DA_GameConfig_Default");
		GameConfig = Cast<UTicTacGameConfig>(StaticLoadObject(UTicTacGameConfig::StaticClass(), nullptr, *ConfigPath));
		if (GameConfig!=nullptr && GameConfig->IsValidConfig())
		{
			
		}
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

	ResetData();
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (TicTacGameState)
	{
		TicTacGameState->InitialBoard(GameConfig->BoardSize);
	}
}

void ATicTacGameMode::ReStartGame()
{
	ATicTacGameState* TicTacGameState = GetTicTacGameState();
	if (TicTacGameState)
	{
		TicTacGameState->ClearBoard();
	}
	ResetData();
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
	}
}

void ATicTacGameMode::TogglePause()
{
	//bool bIsPaused = UGameplayStatics::IsGamePaused(this);
	//UGameplayStatics::SetGamePaused(this, !bIsPaused);
	//ATicTacGameState* TicTacGameState = GetTicTacGameState();
	//if (TicTacGameState)
	//{
	//	TicTacGameState->SetGameState(bIsPaused ? EGameState::Playing : EGameState::Paused);
	//}
}

void ATicTacGameMode::SwitchPlayer()
{
	CurrentPlayer = (CurrentPlayer == EPlayerType::Player1)
		? EPlayerType::Player2
		: EPlayerType::Player1;

	TurnCount++;
}

bool ATicTacGameMode::ProcessPlayerMode(int32 CellIndex)
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

	FGameResult Result = CheckGameResult();
	if (Result.bHasWinner)
	{
		EndGame(Result.Winner, Result.bIsDraw);
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
