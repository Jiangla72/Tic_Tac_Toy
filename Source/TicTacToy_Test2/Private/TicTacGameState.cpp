// Copyright 2025


#include "TicTacGameState.h"

ATicTacGameState::ATicTacGameState()
{
	BoardSize = 3;
	Player1Name = TEXT("1");
	Player1Score = 0;
	Player2Name = TEXT("2");
	Player2Score = 0;
	CurrentGameState = EGameState::NotStarted;

	int32 CellNum = BoardSize * BoardSize;

	BoardData.SetNum(CellNum);
	for (int32 i = 0; i < CellNum; ++i)
	{
		BoardData[i] = EPlayerType::None;
	}
}

void ATicTacGameState::Init()
{
}

void ATicTacGameState::UnInit()
{
}

void ATicTacGameState::InitialBoard(int32 nSize)
{
	BoardSize = nSize;
	int32 nTotalSize = nSize * nSize;
	BoardData.Empty();
	BoardData.SetNum(nTotalSize);
	for (int32 i = 0; i < nTotalSize; ++i)
	{
		BoardData[i] = EPlayerType::None;
	}

	WinningCells.Empty();
	CurrentGameState = EGameState::Playing;
}

void ATicTacGameState::ClearBoard()
{
	BoardData.Empty();
	for (int32 i = 0; i < BoardData.Num(); ++i)
	{
		BoardData[i] = EPlayerType::None;
	}
	WinningCells.Empty();
	CurrentGameState = EGameState::Playing;

}

bool ATicTacGameState::SetCellOwner(int32 nIndex, EPlayerType CellOwner)
{
	if (nIndex < 0 || nIndex >= BoardData.Num())
	{
		return false;
	}

	if (BoardData[nIndex] != EPlayerType::None)
	{
		return false;
	}

	if (CellOwner == EPlayerType::None)
	{
		return false;
	}
	BoardData[nIndex] = CellOwner;

	OnBoardUpdated.Broadcast(nIndex, CellOwner);
	
	return true;
}

EPlayerType ATicTacGameState::GetCellOwner(int32 nIndex) const
{
	EPlayerType CellOwner = EPlayerType::None;
	if (nIndex < 0 || nIndex >= BoardData.Num())
	{
		UE_LOG(LogTemp,Error, TEXT(""), nIndex);
	}
	else
	{
		CellOwner = BoardData[nIndex];
	}
	return CellOwner;
}

bool ATicTacGameState::IsCellOccupied(int32 nIndex) const
{
	bool bOcp = false;
	if (nIndex < 0 || nIndex >= BoardData.Num())
	{
		UE_LOG(LogTemp, Error, TEXT(""), nIndex);
		return false;
	}

	if (BoardData[nIndex] != EPlayerType::None)
	{
		bOcp = true;
	}
	return bOcp;
}

TArray<int32> ATicTacGameState::GetOccupiedCells() const
{
	TArray<int32> Cells;
	for (int32 i = 0; i < BoardData.Num(); i++)
	{
		if (BoardData[i] != EPlayerType::None)
		{
			Cells.Add(i);
		}
	}

	return Cells;
}

int32 ATicTacGameState::GetOccupiedCellCount() const
{
	int32 CellsCount = 0;
	for (int32 i = 0; i < BoardData.Num(); i++)
	{
		if (BoardData[i] == EPlayerType::None)
		{
			CellsCount++;
		}
	}

	return CellsCount;
}

bool ATicTacGameState::IsWinningCell(int32 CellIndex) const
{
	return WinningCells.Contains(CellIndex);
}

void ATicTacGameState::SetWinningCells(const TArray<int32>& Cells)
{
	WinningCells = Cells;
}

TArray<int32> ATicTacGameState::GetWinningCells() const
{
	return WinningCells;
}

TArray<int32> ATicTacGameState::GetEmptyCells() const
{
	TArray<int32> Cells;
	for (int32 i = 0; i < BoardData.Num(); i++)
	{
		if (BoardData[i] == EPlayerType::None)
		{
			Cells.Add(i);
		}
	}
	return Cells;
}

void ATicTacGameState::SetPlayerName(EPlayerType Player, const FString& Name)
{
	if (Player == EPlayerType::Player1)
	{
		Player1Name = Name;
	}
	else if (Player == EPlayerType::Player2)
	{
		Player2Name = Name;
	}
}

FString ATicTacGameState::GetPlayerName(EPlayerType Player) const
{
	FString Name;
	if (Player == EPlayerType::Player1)
	{
		Name = Player1Name;
	}
	else if (Player == EPlayerType::Player2)
	{
		Name = Player2Name;
	}
	return Name;
}

void ATicTacGameState::AddScore(EPlayerType Player, int32 Amount)
{
	if (Player == EPlayerType::Player1)
	{
		Player1Score += Amount;
		OnScoreUpdated.Broadcast(EPlayerType::Player1, Player1Score);
	}
	else if (Player == EPlayerType::Player2)
	{
		Player2Score += Amount;
		OnScoreUpdated.Broadcast(EPlayerType::Player2, Player2Score);
	}
}

int32 ATicTacGameState::GetScore(EPlayerType Player) const
{
	if (Player == EPlayerType::Player1)
	{
		return Player1Score;
	}
	else if (Player == EPlayerType::Player2)
	{
		return Player2Score;
	}
	return 0;
}

void ATicTacGameState::ResetScores()
{
	Player1Score = 0;
	Player2Score = 0;
	OnScoreUpdated.Broadcast(EPlayerType::Player1, 0);
	OnScoreUpdated.Broadcast(EPlayerType::Player2, 0);
}

void ATicTacGameState::SetGameState(EGameState NewState)
{
	EGameState OldState = CurrentGameState;
	CurrentGameState = NewState;
}

EPlayerType ATicTacGameState::GetCurrentPlayerTurn() const
{
	return playerTurn;
}

bool ATicTacGameState::IsBoardFull() const
{
	return GetOccupiedCellCount() == BoardSize;
}

void ATicTacGameState::SwitchPlayerTurn()
{
	if (playerTurn == EPlayerType::Player1)
	{
		playerTurn = EPlayerType::Player2;
	}
	else if (playerTurn == EPlayerType::Player2)
	{
		playerTurn = EPlayerType::Player1;
	}
}

void ATicTacGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
