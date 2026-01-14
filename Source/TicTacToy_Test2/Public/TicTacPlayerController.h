// Copyright 2025

#pragma once

#include "CoreMinimal.h"
#include "TicTacGameTypes.h"
#include "GameFramework/PlayerController.h"
#include "TicTacPlayerController.generated.h"

class UTicTacUIManager;
class UUserWidget;
class ATicTacGameMode;
class ATicTacGameState;
class ATicTacBoard;


/**
 * Player Controller for TicTacToe game
 */
UCLASS()
class TICTACTOY_TEST2_API ATicTacPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATicTacPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameSettingsWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;
	
public:
	UPROPERTY()
	UTicTacUIManager* UIManager = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* MainMenuWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* GameSettingsWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* GameHUDWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* GameOverWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* PauseMenuWidget = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameSettings();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideGameSettings();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideGameHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameOver();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideGameOver();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HidePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void TogglePause();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideAllUI();

	void OnMouseLeftClick();

	void OnMouseRightClick();

	void OnEscapePressed();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void HandleCellClick(int32 CellIndex);

	UFUNCTION(BlueprintPure, Category = "Game")
	ATicTacGameMode* GetTicTacGameMode() const;

	UFUNCTION(BlueprintPure, Category = "Game")
	ATicTacGameState* GetTicTacGameState() const;

	UFUNCTION(BlueprintPure, Category = "Game")
	ATicTacBoard* GetBoard() const { return Board; }

	UFUNCTION(BlueprintCallable, Category = "Game")
	void SetBoard(ATicTacBoard* NewBoard) { Board = NewBoard; }

	UFUNCTION(BlueprintCallable, Category = "Input Mode")
	void SetInputModeUIOnly();

	UFUNCTION(BlueprintCallable, Category = "Input Mode")
	void SetInputModeGameAndUI();

	UFUNCTION(BlueprintCallable, Category = "Input Mode")
	void SetInputModeGameOnly();

protected:
	UFUNCTION()
	void SubscribeToGameStateEvents();

	UFUNCTION()
	void OnBoardUpdated(int32 CellIndex, EPlayerType NewOwner);

	UFUNCTION()
	void OnPlayerChanged(EPlayerType NewPlayer);

	UFUNCTION()
	void OnGameEnded(EPlayerType Winner, bool bIsDraw);

	UFUNCTION()
	void OnScoreUpdated(EPlayerType CurPlayer, int32 NewScore);

	void ShowWidget(UUserWidget* Widget);

	void HideWidget(UUserWidget* Widget);

	//template<typename T>
	//T* CreateWidgetInstance(TSubclassOf<UUserWidget> WidgetClass);
protected:
	UPROPERTY()
	ATicTacBoard* Board = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Input")
	bool bClickInputEnabled = false;
};
