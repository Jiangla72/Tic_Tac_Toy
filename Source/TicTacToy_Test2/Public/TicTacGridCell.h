// Copyright 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TicTacGameTypes.h"
#include "TicTacGridCell.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS()
class TICTACTOY_TEST2_API ATicTacGridCell : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATicTacGridCell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "Grid Cell")
	void InitializeCell(int32 InIndex, int32 InRow, int32 InColumn);
	
	UFUNCTION(BlueprintCallable, Category = "Grid Cell")
	void SetOwningPlayer(EPlayerType NewOwner);
	UFUNCTION(BlueprintCallable, Category = "Grid Cell")
	void ResetCell();

	UFUNCTION(BlueprintCallable, Category = "Grid Cell")
	void SetWinningCell(bool bIsWinning);

	UFUNCTION(BlueprintCallable, Category = "Grid Cell")
	void ShowHoverEffect();

	UFUNCTION(BlueprintCallable, Category = "Grid Cell")
	void HideHoverEffect();

	UFUNCTION(BlueprintCallable, Category = "Grid Cell")
	void PlayPlacementAnimation();

	UFUNCTION(BlueprintCallable, Category = "Grid Cell")
	void PlayWinAnimation();

	UFUNCTION(BlueprintPure, Category = "Grid Cell")
	int32 GetGridIndex() const { return GridIndex; }

	UFUNCTION(BlueprintPure, Category = "Grid Cell")
	int32 GetRow() const { return Row; }

	UFUNCTION(BlueprintPure, Category = "Grid Cell")
	int32 GetColumn() const { return Column; }

	UFUNCTION(BlueprintPure, Category = "Grid Cell")
	EPlayerType GetOwningPlayer() const { return Player; }

	UFUNCTION(BlueprintPure, Category = "Grid Cell")
	bool IsOccupied() const { return bIsOccupied; }
protected:
	UFUNCTION()
	void OnMouseBeginHover(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnMouseEndHover(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnMouseClick(UPrimitiveComponent* ClickedComponent, FKey ButtonPressed);

	void UpdateCellMaterial();

	void UpdatePieceDisplay();

	void CreateMaterialInstances();

	//UFUNCTION(BlueprintCallable, Category = "Materials")
	//void ApplyHighlightMaterial();

	//UFUNCTION(BlueprintCallable, Category = "Materials")
	//void ApplyOwnerMaterial(EPlayerType Player);

	UFUNCTION(BlueprintCallable, Category = "Materials")
	void CreateDynamicMaterialInstance();

	UFUNCTION(BlueprintCallable, Category = "Materials")
	void SetMaterialParameter(FName ParameterName, float Value);

	UFUNCTION(BlueprintCallable, Category = "Materials")
	void SetMaterialColorParameter(FName ParameterName, FLinearColor Color);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* PlacePieceSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* HoverSound;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlaySound(USoundBase* Sound, float VolumeMultiplier = 1.0f);
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BaseMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PieceMesh;
	
	UPROPERTY(BlueprintReadOnly, Category = "Grid Cell")
	int32 GridIndex;
	UPROPERTY(BlueprintReadOnly, Category = "Grid Cell")
	int32 Row;
	UPROPERTY(BlueprintReadOnly, Category = "Grid Cell")
	int32 Column;
	UPROPERTY(BlueprintReadOnly, Category = "Grid Cell")
	EPlayerType Player;
	UPROPERTY(BlueprintReadOnly, Category = "Grid Cell")
	bool bIsOccupied;
	UPROPERTY(BlueprintReadOnly, Category = "Grid Cell")
	bool bIsWinningCell;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* DefaultMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* HoverMaterial = nullptr;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	//UMaterialInterface* HighlightMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* Player1Material = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* Player2Material =nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* WinningCellMaterial=nullptr;
	UPROPERTY()
	UMaterialInstanceDynamic* BaseMaterialInstance=nullptr;
	UPROPERTY()
	UMaterialInstanceDynamic* PieceMaterialInstance=nullptr;

	UPROPERTY()
	EPlayerType CurrentMaterialType = EPlayerType::None;
};
