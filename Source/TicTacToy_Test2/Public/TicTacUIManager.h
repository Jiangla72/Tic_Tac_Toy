// Copyright 2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TicTacGameTypes.h"
#include "TicTacUIManager.generated.h"

class ATicTacGameState;
class ATicTacPlayerController;

/**
 * 
 */
UCLASS()
class TICTACTOY_TEST2_API UTicTacUIManager : public UObject
{
	GENERATED_BODY()
	
public:
	//UFUNCTION(BlueprintCallable, Category = "UI")
	//static UTicTacUIManager* GetInstance(UWorld* World);

	void Initialize(ATicTacPlayerController* InPlayerController);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateGameHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateCurrentPlayerIndicator(EPlayerType CurrentPlayer);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateScoreDisplay(EPlayerType Player, int32 NewScore);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdatePlayerName(EPlayerType Player, FString Name);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameResult(EPlayerType Winner, bool bIsDraw);

protected:
	UPROPERTY()
	ATicTacPlayerController* PlayerController;

	UPROPERTY()
	ATicTacGameState* GameState;

	//static UTicTacUIManager* Instance;
};
