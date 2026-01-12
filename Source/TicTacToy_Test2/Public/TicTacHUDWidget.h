// Copyright 2025

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TicTacGameTypes.h"
#include "TicTacHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class TICTACTOY_TEST2_API UTicTacHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:	
    UFUNCTION(BlueprintImplementableEvent, Category = "TicTacUI")
    void UpdateName(const FString& Player1Name, const FString& Player2Name);

    UFUNCTION(BlueprintImplementableEvent, Category = "TicTacUI")
    void UpdateScore(int32 Player1Score, int32 Player2Score);

    UFUNCTION(BlueprintImplementableEvent, Category = "TicTacUI")
    void UpdateTurnText(const FString& CurrentPlayerName, bool bIsPlayer1);
};
