// Copyright 2025

#include "TicTacAIController.h"
#include "TicTacGameMode.h"
#include "TicTacGameState.h"
#include "TicTacGameConfig.h"

void UTicTacAIController::Initialize(ATicTacGameMode* InGameMode, EPlayerType InAIPlayer)
{
    GameMode = InGameMode;
    AIPlayer = InAIPlayer;
    Difficulty = EAIDifficulty::Easy;  

    if (GameMode)
    {
        GameState = GameMode->GetTicTacGameState();
    }
    TranspositionTable.Empty();
}

int32 UTicTacAIController::MakeMove()
{
    if (!GameState)
    {
        UE_LOG(LogTemp, Error, TEXT("AI GameState is null"));
        return -1;
    }

    int32 BestMove = -1;

    switch (Difficulty)
    {
    case EAIDifficulty::Easy:
        BestMove = MakeRandomMove();
        break;

    case EAIDifficulty::Medium:
        BestMove = MakeMediumMove();
        break;

    case EAIDifficulty::Hard:
        BestMove = MakeHardMove();
        break;
    }

    if (BestMove >= 0)
    {
        UE_LOG(LogTemp, Log, TEXT("AI selected cell %d"), BestMove);
    }

    return BestMove;
}

int32 UTicTacAIController::MakeRandomMove()
{
    TArray<int32> EmptyCells = GameState->GetEmptyCells();

    if (EmptyCells.Num() == 0)
    {
        return -1;
    }

    int32 RandomIndex = FMath::RandRange(0, EmptyCells.Num() - 1);
    return EmptyCells[RandomIndex];
}

int32 UTicTacAIController::MakeMediumMove()
{
    TArray<EPlayerType> Board = GameState->GetBoardData();
    TArray<int32> EmptyCells = GameState->GetEmptyCells();
    int32 BoardSize = GameState->GetBoardSize();

    if (EmptyCells.Num() == 0)
        return -1;

    // ai 直接赢
    for (int32 Cell : EmptyCells)
    {
        TArray<EPlayerType> TestBoard = Board;
        SimulateMove(TestBoard, Cell, AIPlayer);

        if (CheckWinner(TestBoard, AIPlayer))
        {
            return Cell;
        }
    }

    // 阻止对手赢
    EPlayerType Opponent = (AIPlayer == EPlayerType::Player1) ?
        EPlayerType::Player2 : EPlayerType::Player1;
    for (int32 Cell : EmptyCells)
    {
        TArray<EPlayerType> TestBoard = Board;
        SimulateMove(TestBoard, Cell, Opponent);

        if (CheckWinner(TestBoard, Opponent))
        {
            return Cell;
        }
    }
    
    for (int32 Cell : EmptyCells)
    {
        TArray<EPlayerType> TestBoard = Board;
        SimulateMove(TestBoard, Cell, AIPlayer);
        int32 ThreatCount = CountThreats(TestBoard, Cell, AIPlayer);
        // 占领两个威胁点位
        if (ThreatCount >= 2)
        {
            return Cell;
        }
    }

    for (int32 Cell : EmptyCells)
    {
        TArray<EPlayerType> TestBoard = Board;
        SimulateMove(TestBoard, Cell, Opponent);
        int32 ThreatCount = CountThreats(TestBoard, Cell, Opponent);
        // 阻止对手占领两个威胁点位
        if (ThreatCount >= 2)
        {
            return Cell;
        }
    }
    
    if (BoardSize % 2 == 1)
    {
        int32 Center = (BoardSize / 2) * BoardSize + (BoardSize / 2);
        // 有中心先占领中心
        if (EmptyCells.Contains(Center))
        {
            return Center;
        }
    }

    //先占角落
    TArray<int32> Corners = { 0, BoardSize - 1,
        BoardSize * (BoardSize - 1), BoardSize * BoardSize - 1 };

    for (int32 Corner : Corners)
    {
        if (EmptyCells.Contains(Corner))
        {
            return Corner;
        }
    }

    return MakeRandomMove();
}

int32 UTicTacAIController::MakeHardMove()
{
    TArray<EPlayerType> Board = GameState->GetBoardData();
    TArray<int32> EmptyCells = GameState->GetEmptyCells();

    if (EmptyCells.Num() == 0)
        return -1;

    TArray<int32> SortedMoves = SortMovesByPriority(EmptyCells, Board);

    for (int32 Cell : EmptyCells)
    {
        TArray<EPlayerType> TestBoard = Board;
        SimulateMove(TestBoard, Cell, AIPlayer);
        if (CheckWinner(TestBoard, AIPlayer))
        {
            return Cell;
        }
    }

    EPlayerType Opponent = (AIPlayer == EPlayerType::Player1) ? EPlayerType::Player2 : EPlayerType::Player1;
    for (int32 Cell : EmptyCells)
    {
        TArray<EPlayerType> TestBoard = Board;
        SimulateMove(TestBoard, Cell, Opponent);
        if (CheckWinner(TestBoard, Opponent))
        {
            return Cell;
        }
    }
    
    for (int32 Cell : EmptyCells)
    {
        TArray<EPlayerType> TestBoard = Board;
        SimulateMove(TestBoard, Cell, AIPlayer);
        int32 ThreatCount = CountThreats(TestBoard, Cell, AIPlayer);
        // 占领两个威胁点位
        if (ThreatCount >= 2)
        {
            return Cell;
        }
    }

    for (int32 Cell : EmptyCells)
    {
        TArray<EPlayerType> TestBoard = Board;
        SimulateMove(TestBoard, Cell, Opponent);
        int32 ThreatCount = CountThreats(TestBoard, Cell, Opponent);
        // 阻止对手占领两个威胁点位
        if (ThreatCount >= 2)
        {
            return Cell;
        }
    }

    int32 BestMove = -1;
    int32 BestScore = INT_MIN;
    int32 Alpha = INT_MIN; // ai 最次要拿的分数 当前轮最好的分数 之后只会更好（每次遍历的最大值）
    int32 Beta = INT_MAX; // 玩家想让ai拿到的分数 当前最差的分数，继续遍历只会更差（每次遍历的最小值）

    for (int32 Cell : SortedMoves)
    {
        TArray<EPlayerType> TestBoard = Board;
        SimulateMove(TestBoard, Cell, AIPlayer);

        int32 Score = Minimax(TestBoard, 1, false, Alpha, Beta);

        if (Score > BestScore)
        {
            BestScore = Score;
            BestMove = Cell;
        }

        Alpha = FMath::Max(Alpha, Score);
        
        if (BestScore >= 90)
        {
            break;
        }
    }

    return BestMove;
}

int32 UTicTacAIController::Minimax(TArray<EPlayerType>& Board, int32 Depth, bool bIsMaximizing, int32 Alpha, int32 Beta)
{
    EPlayerType Opponent = (AIPlayer == EPlayerType::Player1) ?
        EPlayerType::Player2 : EPlayerType::Player1;

    if (CheckWinner(Board, AIPlayer))
        return 100 - Depth; // 深度越小越好

    if (CheckWinner(Board, Opponent))
        return Depth - 100; // 深度越小分数越差，对玩家越好

    TArray<int32> EmptyCells = GetEmptyCells(Board);
    if (EmptyCells.Num() == 0)
        return 0;

    int32 BoardSize = GameState->GetBoardSize();
    int32 MaxDepth = 9;
    
    if (BoardSize == 3)
    {
        MaxDepth = 9;
    }
    else if (BoardSize == 4)
    {
        MaxDepth = 6;
    }
    else if (BoardSize == 5)
    {
        MaxDepth = 5;
    }
    else
    {
        MaxDepth = 4;
    }
    
    if (EmptyCells.Num() <= 10)
    {
        // 空位少的时候可以适当放宽递归次数，但是肯定不超过剩余格子数
        MaxDepth = FMath::Min(MaxDepth + 2, EmptyCells.Num());
    }
    
    if (Depth >= MaxDepth)
        return EvaluateBoard(Board);

    uint32 BoardHash = CalculateBoardHash(Board);
    if (TranspositionTable.Contains(BoardHash))
    {
        return TranspositionTable[BoardHash];
    }

    int32 ResultScore = 0;

    if (bIsMaximizing)
    {
        // 当前是ai 轮次
        int32 MaxScore = INT_MIN;

        for (int32 Cell : EmptyCells)
        {
            SimulateMove(Board, Cell, AIPlayer);
            // 计算下一轮玩家轮次（玩家只会返回最小分数，我们需要其中最大的那个分数
            int32 Score = Minimax(Board, Depth + 1, false, Alpha, Beta);
            UndoMove(Board, Cell);

            MaxScore = FMath::Max(MaxScore, Score);
            Alpha = FMath::Max(Alpha, Score);

            // ai分数已经超过上一玩家轮次的最小分数，玩家肯定不要之后的结果了，直接剪枝
            if (Beta <= Alpha)
                break;
        }

        ResultScore = MaxScore;
    }
    else
    {
        // 当前是玩家 轮次
        int32 MinScore = INT_MAX;

        for (int32 Cell : EmptyCells)
        {
            SimulateMove(Board, Cell, Opponent);
            int32 Score = Minimax(Board, Depth + 1, true, Alpha, Beta);
            UndoMove(Board, Cell);

            MinScore = FMath::Min(MinScore, Score);
            Beta = FMath::Min(Beta, Score);

            if (Beta <= Alpha)
                break;
        }

        ResultScore = MinScore;
    }

    TranspositionTable.Add(BoardHash, ResultScore);
    return ResultScore;
}

int32 UTicTacAIController::EvaluateBoard(const TArray<EPlayerType>& Board)
{
    if (!GameState || !GameMode)
        return 0;

    int32 BoardSize = GameState->GetBoardSize();
    int32 WinLength = GameMode->GetWinConditionCount();
    int32 TotalScore = 0;

    EPlayerType Opponent = (AIPlayer == EPlayerType::Player1) ?
        EPlayerType::Player2 : EPlayerType::Player1;

    // rows
    for (int32 Row = 0; Row < BoardSize; ++Row)
    {
        for (int32 StartCol = 0; StartCol <= BoardSize - WinLength; ++StartCol)
        {
            TotalScore += EvaluateLine(Board, Row, StartCol, 0, 1, WinLength, WinLength, Opponent);
        }
    }

    // Evaluate columns
    for (int32 Col = 0; Col < BoardSize; ++Col)
    {
        for (int32 StartRow = 0; StartRow <= BoardSize - WinLength; ++StartRow)
        {
            TotalScore += EvaluateLine(Board, StartRow, Col, 1, 0, WinLength, WinLength, Opponent);
        }
    }

    bool bAllowDiagonal = true;
    if (GameMode && GameMode->GetGameConfig())
    {
        bAllowDiagonal = GameMode->GetGameConfig()->bAllowDiagonalWins;
    }
    if (bAllowDiagonal)
    {
        for (int32 StartRow = 0; StartRow <= BoardSize - WinLength; ++StartRow)
        {
            for (int32 StartCol = 0; StartCol <= BoardSize - WinLength; ++StartCol)
            {
                TotalScore += EvaluateLine(Board, StartRow, StartCol, 1, 1, WinLength, WinLength, Opponent);
            }
        }
        for (int32 StartRow = 0; StartRow <= BoardSize - WinLength; ++StartRow)
        {
            for (int32 StartCol = WinLength - 1; StartCol < BoardSize; ++StartCol)
            {
                TotalScore += EvaluateLine(Board, StartRow, StartCol, 1, -1, WinLength, WinLength, Opponent);
            }
        }
    }

    int32 CenterRow = BoardSize / 2;
    int32 CenterCol = BoardSize / 2;
    
    for (int32 i = 0; i < Board.Num(); ++i)
    {
        if (Board[i] != EPlayerType::None)
        {
            int32 Row = i / BoardSize;
            int32 Col = i % BoardSize;
            
            int32 DistanceToCenter = FMath::Abs(Row - CenterRow) + FMath::Abs(Col - CenterCol);
            int32 PositionValue = FMath::Max(1, BoardSize - DistanceToCenter);
            
            if (Board[i] == AIPlayer)
            {
                TotalScore += PositionValue;
            }
            else if (Board[i] == Opponent)
            {
                TotalScore -= PositionValue;
            }
        }
    }

    return TotalScore;
}

int32 UTicTacAIController::EvaluateLine(const TArray<EPlayerType>& Board, int32 StartRow, int32 StartCol, int32 RowStep,
	int32 ColStep, int32 Length, int32 WinLength, EPlayerType Opponent)
{

    if (!GameState)
        return 0;

    int32 BoardSize = GameState->GetBoardSize();
    int32 AICount = 0;
    int32 OpponentCount = 0;
    int32 EmptyCount = 0;

    for (int32 i = 0; i < Length; ++i)
    {
        int32 Row = StartRow + i * RowStep;
        int32 Col = StartCol + i * ColStep;

        if (Row < 0 || Row >= BoardSize || Col < 0 || Col >= BoardSize)
            continue;

        int32 Index = Row * BoardSize + Col;

        if (!Board.IsValidIndex(Index))
            continue;

        EPlayerType CellOwner = Board[Index];

        if (CellOwner == AIPlayer)
        {
            AICount++;
        }
        else if (CellOwner == Opponent)
        {
            OpponentCount++;
        }
        else
        {
            EmptyCount++;
        }
    }

    int32 Score = 0;

    if (AICount > 0 && OpponentCount > 0)
    {
        return 0; 
    }

    if (AICount > 0 && OpponentCount == 0)
    {
        if (AICount == WinLength)
        {
            Score += 10000;
        }
        else if (AICount == WinLength - 1 && EmptyCount > 0)
        {
            Score += 100;
        }
        else if (AICount == WinLength - 2 && EmptyCount >= 2)
        {
            Score += 20;
        }
        else
        {
            Score += AICount * 3;
        }
    }

    if (OpponentCount > 0 && AICount == 0)
    {
        if (OpponentCount == WinLength)
        {
            Score -= 10000;
        }
        else if (OpponentCount == WinLength - 1 && EmptyCount > 0)
        {
            Score -= 150;
        }
        else if (OpponentCount == WinLength - 2 && EmptyCount >= 2)
        {
            Score -= 40;
        }
        else
        {
            Score -= OpponentCount * 3;
        }
    }

    return Score;
}

bool UTicTacAIController::CheckWinner(const TArray<EPlayerType>& Board, EPlayerType Player)
{
    if (!GameState)
        return false;

    int32 BoardSize = GameState->GetBoardSize();
    int32 WinLength = GameMode->GetWinConditionCount();

    for (int32 Row = 0; Row < BoardSize; ++Row)
    {
        for (int32 StartCol = 0; StartCol <= BoardSize - WinLength; ++StartCol)
        {
            bool bIsWin = true;

            for (int32 i = 0; i < WinLength; ++i)
            {
                int32 Index = Row * BoardSize + (StartCol + i);

                if (!Board.IsValidIndex(Index) || Board[Index] != Player)
                {
                    bIsWin = false;
                    break;
                }
            }

            if (bIsWin)
            {
                return true;
            }
        }
    }

    for (int32 Col = 0; Col < BoardSize; ++Col)
    {
        for (int32 StartRow = 0; StartRow <= BoardSize - WinLength; ++StartRow)
        {
            bool bIsWin = true;

            for (int32 i = 0; i < WinLength; ++i)
            {
                int32 Index = (StartRow + i) * BoardSize + Col;

                if (!Board.IsValidIndex(Index) || Board[Index] != Player)
                {
                    bIsWin = false;
                    break;
                }
            }

            if (bIsWin)
            {
                return true;
            }
        }
    }

    bool bAllowDiagonal = true;
    if (GameMode && GameMode->GetGameConfig())
    {
        bAllowDiagonal = GameMode->GetGameConfig()->bAllowDiagonalWins;
    }
    if (bAllowDiagonal)
    {
        for (int32 StartRow = 0; StartRow <= BoardSize - WinLength; ++StartRow)
        {
            for (int32 StartCol = 0; StartCol <= BoardSize - WinLength; ++StartCol)
            {
                bool bIsWin = true;
                for (int32 i = 0; i < WinLength; ++i)
                {
                    int32 Row = StartRow + i;
                    int32 Col = StartCol + i;
                    int32 Index = Row * BoardSize + Col;
                    if (!Board.IsValidIndex(Index) || Board[Index] != Player)
                    {
                        bIsWin = false;
                        break;
                    }
                }
                if (bIsWin)
                {
                    return true;
                }
            }
        }
        for (int32 StartRow = 0; StartRow <= BoardSize - WinLength; ++StartRow)
        {
            for (int32 StartCol = WinLength - 1; StartCol < BoardSize; ++StartCol)
            {
                bool bIsWin = true;
                for (int32 i = 0; i < WinLength; ++i)
                {
                    int32 Row = StartRow + i;
                    int32 Col = StartCol - i;
                    int32 Index = Row * BoardSize + Col;
                    if (!Board.IsValidIndex(Index) || Board[Index] != Player)
                    {
                        bIsWin = false;
                        break;
                    }
                }
                if (bIsWin)
                {
                    return true;
                }
            }
        }
    }
    
    return false;
}

TArray<int32> UTicTacAIController::GetEmptyCells(const TArray<EPlayerType>& Board)
{
    TArray<int32> EmptyCells;

    for (int32 i = 0; i < Board.Num(); ++i)
    {
        if (Board[i] == EPlayerType::None)
        {
            EmptyCells.Add(i);
        }
    }

    return EmptyCells;
}

void UTicTacAIController::SimulateMove(TArray<EPlayerType>& Board, int32 CellIndex, EPlayerType Player)
{
    if (Board.IsValidIndex(CellIndex))
    {
        Board[CellIndex] = Player;
    }
}

void UTicTacAIController::UndoMove(TArray<EPlayerType>& Board, int32 CellIndex)
{
    if (Board.IsValidIndex(CellIndex))
    {
        Board[CellIndex] = EPlayerType::None;
    }
}

uint32 UTicTacAIController::CalculateBoardHash(const TArray<EPlayerType>& Board)
{
    uint32 Hash = 0;
    for (int32 i = 0; i < Board.Num(); ++i)
    {
        Hash = Hash * 3 + (uint32)Board[i];
    }
    return Hash;
}

TArray<int32> UTicTacAIController::SortMovesByPriority(const TArray<int32>& Moves, const TArray<EPlayerType>& Board)
{
    if (!GameState)
        return Moves;
        
    int32 BoardSize = GameState->GetBoardSize();
    int32 CenterRow = BoardSize / 2;
    int32 CenterCol = BoardSize / 2;
    
    TArray<TPair<int32, int32>> MovePriorities;
    
    for (int32 Move : Moves)
    {
        int32 Row = Move / BoardSize;
        int32 Col = Move % BoardSize;
        
        int32 DistanceToCenter = FMath::Abs(Row - CenterRow) + FMath::Abs(Col - CenterCol);
        int32 Priority = 100 - DistanceToCenter * 10;
        
        // 搜索空格周围的8个格子
        for (int32 dRow = -1; dRow <= 1; ++dRow)
        {
            for (int32 dCol = -1; dCol <= 1; ++dCol)
            {
                if (dRow == 0 && dCol == 0)
                    continue;
                    
                int32 AdjRow = Row + dRow;
                int32 AdjCol = Col + dCol;
                
                if (AdjRow >= 0 && AdjRow < BoardSize && AdjCol >= 0 && AdjCol < BoardSize)
                {
                    int32 AdjIndex = AdjRow * BoardSize + AdjCol;
                    if (Board.IsValidIndex(AdjIndex) && Board[AdjIndex] != EPlayerType::None)
                    {
                        // 如果空格周围被占领加分
                        Priority += 20;
                    }
                }
            }
        }
        
        MovePriorities.Add(TPair<int32, int32>(Move, Priority));
    }
    
    MovePriorities.Sort([](const TPair<int32, int32>& A, const TPair<int32, int32>& B) {
        return A.Value > B.Value;
    });
    
    TArray<int32> SortedMoves;
    for (const auto& Pair : MovePriorities)
    {
        SortedMoves.Add(Pair.Key);
    }
    
    return SortedMoves;
}

int32 UTicTacAIController::CountThreats(const TArray<EPlayerType>& Board, int32 LastMove, EPlayerType Player)
{
    if (!GameState || !GameMode)
        return 0;
        
    int32 BoardSize = GameState->GetBoardSize();
    int32 WinLength = GameMode->GetWinConditionCount();
    int32 ThreatCount = 0;
    
    int32 Row = LastMove / BoardSize;
    int32 Col = LastMove % BoardSize;
    
    bool bAllowDiagonal = true;
    if (GameMode && GameMode->GetGameConfig())
    {
        bAllowDiagonal = GameMode->GetGameConfig()->bAllowDiagonalWins;
    }
    int32 Directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
    int32 DirCount = bAllowDiagonal ? 4 : 2;
    
    for (int32 d = 0; d < DirCount; ++d)
    {
        int32 SelfCount = 1;
        int32 EmptyCount = 0;
        int32 Span = 1;
        
        // Forward direction
        for (int32 i = 1; i < WinLength; ++i)
        {
            int32 CheckRow = Row + i * Directions[d][0];
            int32 CheckCol = Col + i * Directions[d][1];
            
            if (CheckRow < 0 || CheckRow >= BoardSize || CheckCol < 0 || CheckCol >= BoardSize)
                break;
                
            int32 Index = CheckRow * BoardSize + CheckCol;
            if (!Board.IsValidIndex(Index))
                break;
                
            if (Board[Index] == Player)
                SelfCount++;
            else if (Board[Index] == EPlayerType::None)
                EmptyCount++;
            else
                break;

            Span++;
        }
        
        // Backward direction
        for (int32 i = 1; i < WinLength; ++i)
        {
            int32 CheckRow = Row - i * Directions[d][0];
            int32 CheckCol = Col - i * Directions[d][1];
            
            if (CheckRow < 0 || CheckRow >= BoardSize || CheckCol < 0 || CheckCol >= BoardSize)
                break;
                
            int32 Index = CheckRow * BoardSize + CheckCol;
            if (!Board.IsValidIndex(Index))
                break;
                
            if (Board[Index] == Player)
                SelfCount++;
            else if (Board[Index] == EPlayerType::None)
                EmptyCount++;
            else
                break;

            Span++;
        }
        
        if (Span >= WinLength && SelfCount >= WinLength - 1 && EmptyCount > 0)
        {
            ThreatCount++;
        }
    }
    
    return ThreatCount;
}
