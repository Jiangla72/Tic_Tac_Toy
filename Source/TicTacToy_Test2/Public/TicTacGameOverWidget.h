// Copyright 2025

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TicTacGameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class TICTACTOY_TEST2_API UTicTacGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, Category = "TicTacUI")
    void UpdateWinnerName(const FString& WinnerName);

    UFUNCTION(BlueprintImplementableEvent, Category = "TicTacUI")
    void UpdateScore(int32 WinnerScore);
};
