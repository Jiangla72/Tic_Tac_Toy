// Copyright 2025


#include "TicTacPlayerController.h"

#include "TicTacBoard.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TicTacGameMode.h"
#include "TicTacGameState.h"
#include "TicTacGridCell.h"
#include "Engine/World.h"
#include "Misc/Paths.h"

ATicTacPlayerController::ATicTacPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ATicTacPlayerController::BeginPlay()
{
	Super::BeginPlay();
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		SetViewTarget(ControlledPawn);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("cant find pawn"));
	}

	FString CurrentLevelName = GetWorld()->GetMapName(); // 获取关卡路径 GetWorld()不为空不用判空
	CurrentLevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); // 移除目录只留下关卡名
	CurrentLevelName = FPaths::GetBaseFilename(CurrentLevelName);

	if (CurrentLevelName == TEXT("MainMenu"))
	{
		ShowMainMenu();
		SetInputModeUIOnly();
	}
	else if (CurrentLevelName == TEXT("GameLevel"))
	{
		ShowGameHUD();
		SetInputModeGameAndUI();
	}
}

void ATicTacPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->BindAction("LeftClick", IE_Pressed, this, &ATicTacPlayerController::OnMouseLeftClick);
		InputComponent->BindAction("RightClick", IE_Pressed, this, &ATicTacPlayerController::OnMouseRightClick);
		InputComponent->BindAction("Pause", IE_Pressed, this, &ATicTacPlayerController::OnEscapePressed);
	}
}

void ATicTacPlayerController::ShowMainMenu()
{
	if (!MainMenuWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenuWidgetClass 未设置！"));
		return;
	}

	// 如果Widget已存在，直接显示
	if (MainMenuWidget)
	{
		ShowWidget(MainMenuWidget);
	}
	else
	{
		// 创建新Widget
		MainMenuWidget = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidget)
		{
			ShowWidget(MainMenuWidget);
		}
	}

	SetInputModeUIOnly();
}

void ATicTacPlayerController::HideMainMenu()
{
	if (MainMenuWidgetClass)
	{
		HideWidget(MainMenuWidget);
	}
}

void ATicTacPlayerController::ShowGameSettings()
{
	if (!GameSettingsWidgetClass)
	{
		return;
	}

	if (GameSettingsWidget)
	{
		ShowWidget(GameSettingsWidget);
	}
	else
	{
		GameSettingsWidget = CreateWidget<UUserWidget>(this, GameSettingsWidgetClass);
		if (GameSettingsWidget)
		{
			ShowWidget(GameSettingsWidget);
			UE_LOG(LogTemp, Log, TEXT("游戏设置已显示"));
		}
	}

	SetInputModeUIOnly();
}

void ATicTacPlayerController::HideGameSettings()
{
	if (GameSettingsWidget)
	{
		HideWidget(GameSettingsWidget);
	}
}

void ATicTacPlayerController::ShowGameHUD()
{
	UE_LOG(LogTemp, Log, TEXT("ShowGameHUD"));

	if (!GameHUDWidgetClass)
	{
		UE_LOG(LogTemp, Log, TEXT("GameHUDWidgetClass == null"));
		return;
	}

	if (GameHUDWidget)
	{
		ShowWidget(GameHUDWidget);
	}
	else
	{
		GameHUDWidget = CreateWidget<UUserWidget>(this, GameHUDWidgetClass);
		if (GameHUDWidget)
		{
			ShowWidget(GameHUDWidget);
		}
	}

	SetInputModeGameAndUI();
	bClickInputEnabled = true;
}

void ATicTacPlayerController::HideGameHUD()
{
	UE_LOG(LogTemp, Log, TEXT("HideGameHUD"));
	if (GameHUDWidgetClass)
	{
		HideWidget(GameHUDWidget);
	}
}

void ATicTacPlayerController::ShowGameOver()
{
	if (!GameOverWidgetClass)
	{
		return;
	}

	if (GameOverWidget)
	{
		ShowWidget(GameOverWidget);
	}
	else
	{
		GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
		if (GameOverWidget)
		{
			ShowWidget(GameOverWidget);
		}
	}

	SetInputModeUIOnly();
	bClickInputEnabled = false;
}

void ATicTacPlayerController::HideGameOver()
{
	if (GameOverWidget)
	{
		HideWidget(GameOverWidget);
	}
}

void ATicTacPlayerController::ShowPauseMenu()
{
	if (!PauseMenuWidgetClass)
	{
		return;
	}

	if (PauseMenuWidget)
	{
		ShowWidget(PauseMenuWidget);
	}
	else
	{
		PauseMenuWidget = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
		if (PauseMenuWidget)
		{
			ShowWidget(PauseMenuWidget);
		}
	}

	// 暂停游戏
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	SetInputModeUIOnly();
	bClickInputEnabled = false;
}

void ATicTacPlayerController::HidePauseMenu()
{
	if (PauseMenuWidget)
	{
		HideWidget(PauseMenuWidget);
	}

	// 恢复游戏
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	SetInputModeGameAndUI();
	bClickInputEnabled = true;
}

void ATicTacPlayerController::TogglePause()
{
	bool bIsGamePaused = UGameplayStatics::IsGamePaused(GetWorld());
	if (PauseMenuWidget && PauseMenuWidget->IsInViewport())
	{
		HidePauseMenu();
	}
	else
	{
		ShowPauseMenu();
	}
}

void ATicTacPlayerController::HideAllUI()
{
	HideMainMenu();
	HideGameHUD();
	HideGameOver();
	HideGameSettings();
	HidePauseMenu();
}

void ATicTacPlayerController::OnMouseLeftClick()
{
	UE_LOG(LogTemp, Log, TEXT("OnMouseLeftClick"));
	if (!bClickInputEnabled)
	{
		UE_LOG(LogTemp, Log, TEXT("bClickInputEnabled = false"));
		return;
	}
	FHitResult HitResult;

	if (GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		UE_LOG(LogTemp, Log, TEXT("GetHitResultUnderCursor"));
		// 射线检测能看到的物体
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->ActorHasTag(TEXT("GridCell")))
		{
			ATicTacGridCell* GridCell = Cast<ATicTacGridCell>(HitActor);
			if (GridCell)
			{
				int32 CellIndex = GridCell->GetGridIndex();
				HandleCellClick(CellIndex);
			}
		}
	}
}

void ATicTacPlayerController::OnMouseRightClick()
{
	UE_LOG(LogTemp, Log, TEXT("OnMouseRightClick"));

}

void ATicTacPlayerController::OnEscapePressed()
{
	UE_LOG(LogTemp, Log, TEXT("OnEscapePressed"));
	TogglePause();
}

void ATicTacPlayerController::HandleCellClick(int32 CellIndex)
{
	ATicTacGameMode* GameMode = GetTicTacGameMode();
	if (!GameMode)
	{
		return;
	}

	// 调用GameMode处理落子
	bool bSuccess = GameMode->ProcessPlayerMove(CellIndex);

	if (bSuccess)
	{
	}
	else
	{
	}
}

ATicTacGameMode* ATicTacPlayerController::GetTicTacGameMode() const
{
	return Cast<ATicTacGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}

ATicTacGameState* ATicTacPlayerController::GetTicTacGameState() const
{
	return Cast<ATicTacGameState>(UGameplayStatics::GetGameState(this));
}

void ATicTacPlayerController::SetInputModeUIOnly()
{
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void ATicTacPlayerController::SetInputModeGameAndUI()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void ATicTacPlayerController::SetInputModeGameOnly()
{
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
}

void ATicTacPlayerController::ShowWidget(UUserWidget* Widget)
{
	if (Widget && !Widget->IsInViewport())
	{
		Widget->AddToViewport();
	}
}

void ATicTacPlayerController::HideWidget(UUserWidget* Widget)
{
	if (Widget && Widget->IsInViewport())
	{
		Widget->RemoveFromParent();
	}
}
