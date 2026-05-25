// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopMainMenuWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"

void UProject_GemCoopMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BTN_StartGame)
	{
		BTN_StartGame->OnClicked.AddDynamic(this, &UProject_GemCoopMainMenuWidget::OnClickedStartGame);
	}

	if (BTN_Settings)
	{
		BTN_Settings->OnClicked.AddDynamic(this, &UProject_GemCoopMainMenuWidget::OnClickedSettings);
	}

	if (BTN_Quit)
	{
		BTN_Quit->OnClicked.AddDynamic(this, &UProject_GemCoopMainMenuWidget::OnClickedQuit);
	}

	if (BTN_SettingsBack)
	{
		BTN_SettingsBack->OnClicked.AddDynamic(this, &UProject_GemCoopMainMenuWidget::OnClickedSettingBack);
	}

	if (BTN_QuitYes)
	{
		BTN_QuitYes->OnClicked.AddDynamic(this, &UProject_GemCoopMainMenuWidget::OnClickedQuitYes);
	}

	if (BTN_QuitNo)
	{
		BTN_QuitNo->OnClicked.AddDynamic(this, &UProject_GemCoopMainMenuWidget::OnClickedQuitNo);
	}

	SwitchToPanel(Panel_Main);
}

void UProject_GemCoopMainMenuWidget::SwitchToPanel(UWidget* TargetPanel)
{
	if (!WS_MainMenu || !TargetPanel)
	{
		return;
	}

	WS_MainMenu->SetActiveWidget(TargetPanel);
}

void UProject_GemCoopMainMenuWidget::OnClickedStartGame()
{
	if (!PrivateLobbyLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, PrivateLobbyLevelName);
	}
}

void UProject_GemCoopMainMenuWidget::OnClickedSettings()
{
	SwitchToPanel(Panel_Settings);
}

void UProject_GemCoopMainMenuWidget::OnClickedQuit()
{
	SwitchToPanel(Panel_ExitConfirm);
}

void UProject_GemCoopMainMenuWidget::OnClickedSettingBack()
{
	SwitchToPanel(Panel_Main);
}

void UProject_GemCoopMainMenuWidget::OnClickedQuitYes()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->ConsoleCommand(TEXT("quit"));
	}
}

void UProject_GemCoopMainMenuWidget::OnClickedQuitNo()
{
	SwitchToPanel(Panel_Main);
}