// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopLobbyWidget.generated.h"

class UTextBlock;
class UButton;
class UVerticalBox;
class UProject_GemCoopGameInstance;

/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API UProject_GemCoopLobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_Gold;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_Trait;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_InventoryTitle;

	UPROPERTY(meta = (BindWidgetOptional))
	UVerticalBox* VB_GemInventory;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_StartGame;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_Save;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_Load;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_Quit;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby")
	FName GameLevelName = TEXT("GameLevel");

public:
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void RefreshLobby();

protected:
	UFUNCTION()
	void OnClickedStartGame();

	UFUNCTION()
	void OnClickedSave();

	UFUNCTION()
	void OnClickedLoad();

	UFUNCTION()
	void OnClickedQuit();

private:
	UProject_GemCoopGameInstance* GetGemCoopGameInstance() const;

	void RefreshGold();
	void RefreshTrait();
	void RefreshGemInventory();

public:
	FString GemTypeToString(EGemType GemType) const;
	FString GemGradeToString(EGemGrade Grade) const;
};
