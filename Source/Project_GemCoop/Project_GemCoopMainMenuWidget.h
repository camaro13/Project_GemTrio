// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_GemCoopMainMenuWidget.generated.h"

class UButton;
class UWidget;
class UWidgetSwitcher;

/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API UProject_GemCoopMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UWidgetSwitcher* WS_MainMenu;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_Main;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_Settings;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_ExitConfirm;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_StartGame;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_Settings;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_Quit;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_SettingsBack;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_QuitYes;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_QuitNo;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainMenu")
	FName PrivateLobbyLevelName = TEXT("LobbyLevel");

protected:
	UFUNCTION()
	void OnClickedStartGame();

	UFUNCTION()
	void OnClickedSettings();

	UFUNCTION()
	void OnClickedQuit();

	UFUNCTION()
	void OnClickedSettingBack();

	UFUNCTION()
	void OnClickedQuitYes();

	UFUNCTION()
	void OnClickedQuitNo();

private:
	void SwitchToPanel(UWidget* TargetPanel);
};
