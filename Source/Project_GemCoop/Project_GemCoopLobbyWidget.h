// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopLobbyWidget.generated.h"

class UTextBlock;
class UButton;
class UVerticalBox;
class UWidget;
class UEditableTextBox;
class UWidgetSwitcher;
class UProject_GemCoopGameInstance;
class UProject_GemCoopGemInvenRowWidget;

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
	UWidgetSwitcher* WS_Lobby;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_Main;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_CreateGame;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_JoinGame;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_GemInventory;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_Codex;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_Settings;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_Gold;

	UPROPERTY(meta = (BindWidgetOptional))
	UVerticalBox* VB_GemInventory;

	UPROPERTY(meta = (BindWidgetOptional))
	UVerticalBox* VB_Codex;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_CreateGame;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_JoinGame;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_GemInventory;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_Codex;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_Settings;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_BackToMainMenu;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_CreateBack;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_JoinBack;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_InventoryBack;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_CodexBack;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_SettingsBack;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_EquippedQ;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_EquippedW;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_EquippedE;

	UPROPERTY(meta = (BindWidgetOptional))
	UEditableTextBox* ETB_RoomName;

	UPROPERTY(meta = (BindWidgetOptional))
	UEditableTextBox* ETB_JoinCode;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_CreateStart;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_JoinStart;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_CreateStatus;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_JoinStatus;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby")
	FName MainMenuLevelName = TEXT("MainMenu");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby|Inventory")
	TSubclassOf<UProject_GemCoopGemInvenRowWidget> GemInventoryRowWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby")
	FName GameLevelName = TEXT("Lvl_ThirdPerson");

public:
	UFUNCTION(BlueprintCallable, Category = "Lobby|Inventory")
	void EquipGemFromInventory(int32 SlotIndex, FName GemID);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void RefreshLobby();

protected:
	UFUNCTION()
	void OnClickedCreateGame();

	UFUNCTION()
	void OnClickedJoinGame();

	UFUNCTION()
	void OnClickedGemInventory();

	UFUNCTION()
	void OnClickedCodex();

	UFUNCTION()
	void OnClickedSettings();

	UFUNCTION()
	void OnClickedBackToMainMenu();

	UFUNCTION()
	void OnClickedBackToLobbyMain();

	UFUNCTION()
	void OnClickedCreateStart();

	UFUNCTION()
	void OnClickedJoinStart();

private:
	UProject_GemCoopGameInstance* GetGemCoopGameInstance() const;

	void SwitchToPanel(UWidget* TargetPanel);

	void RefreshGold();
	void RefreshGemInventory();
	void RefreshCodex();
	void RefreshEquippedGems();

	FString GemTypeToString(EGemType GemType) const;
	FString GemGradeToString(EGemGrade Grade) const;
};
