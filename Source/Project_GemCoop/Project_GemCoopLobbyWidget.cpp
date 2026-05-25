// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopLobbyWidget.h"
#include "Project_GemCoopGameInstance.h"
#include "Project_GemCoopTypes.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

void UProject_GemCoopLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BTN_CreateGame)
	{
		BTN_CreateGame->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedCreateGame);
	}

	if (BTN_JoinGame)
	{
		BTN_JoinGame->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedJoinGame);
	}

	if (BTN_GemInventory)
	{
		BTN_GemInventory->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedGemInventory);
	}

	if (BTN_Codex)
	{
		BTN_Codex->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedCodex);
	}

	if (BTN_Settings)
	{
		BTN_Settings->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedSettings);
	}

	if (BTN_BackToMainMenu)
	{
		BTN_BackToMainMenu->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedBackToMainMenu);
	}

	if (BTN_CreateBack)
	{
		BTN_CreateBack->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedBackToLobbyMain);
	}

	if (BTN_JoinBack)
	{
		BTN_JoinBack->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedBackToLobbyMain);
	}

	if (BTN_InventoryBack)
	{
		BTN_InventoryBack->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedBackToLobbyMain);
	}

	if (BTN_CodexBack)
	{
		BTN_CodexBack->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedBackToLobbyMain);
	}

	if (BTN_SettingsBack)
	{
		BTN_SettingsBack->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedBackToLobbyMain);
	}

	if (UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance())
	{
		GI->OnGemInventoryChanged.AddDynamic(this, &UProject_GemCoopLobbyWidget::RefreshLobby);
	}

	SwitchToPanel(Panel_Main);
	RefreshLobby();
}

UProject_GemCoopGameInstance* UProject_GemCoopLobbyWidget::GetGemCoopGameInstance() const
{
	return GetWorld() ? Cast<UProject_GemCoopGameInstance>(GetWorld()->GetGameInstance()) : nullptr;
}

void UProject_GemCoopLobbyWidget::SwitchToPanel(UWidget* TargetPanel)
{
	if (!WS_Lobby || !TargetPanel)
	{
		return;
	}

	WS_Lobby->SetActiveWidget(TargetPanel);
}

void UProject_GemCoopLobbyWidget::RefreshLobby()
{
	RefreshGold();
}

void UProject_GemCoopLobbyWidget::RefreshGold()
{
	UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance();

	if (!GI || !TXT_Gold)
	{
		return;
	}

	TXT_Gold->SetText(FText::FromString(FString::Printf(TEXT("Gold: %d"), GI->TotalGoldCurrency)));
}

void UProject_GemCoopLobbyWidget::OnClickedCreateGame()
{
	SwitchToPanel(Panel_CreateGame);

	UE_LOG(LogTemp, Warning, TEXT("Create Game panel opened. Not implemented yet."));
}

void UProject_GemCoopLobbyWidget::OnClickedJoinGame()
{
	SwitchToPanel(Panel_JoinGame);

	UE_LOG(LogTemp, Warning, TEXT("Join Game panel opened. Not implemented yet."));
}

void UProject_GemCoopLobbyWidget::OnClickedGemInventory()
{
	RefreshGemInventory();
	SwitchToPanel(Panel_GemInventory);
}

void UProject_GemCoopLobbyWidget::OnClickedCodex()
{
	RefreshCodex();
	SwitchToPanel(Panel_Codex);
}

void UProject_GemCoopLobbyWidget::OnClickedSettings()
{
	SwitchToPanel(Panel_Settings);
}

void UProject_GemCoopLobbyWidget::OnClickedBackToMainMenu()
{
	if (!MainMenuLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, MainMenuLevelName);
	}
}

void UProject_GemCoopLobbyWidget::OnClickedBackToLobbyMain()
{
	SwitchToPanel(Panel_Main);
	RefreshLobby();
}

void UProject_GemCoopLobbyWidget::RefreshGemInventory()
{
	UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance();

	if (!GI || !VB_GemInventory || !WidgetTree)
	{
		return;
	}

	VB_GemInventory->ClearChildren();

	TArray<FOwnedGemStack> Inventory = GI->GetGemInventory();

	if (Inventory.Num() <= 0)
	{
		UTextBlock* EmptyText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

		if (EmptyText)
		{
			EmptyText->SetText(FText::FromString(TEXT("No gems owned,")));
			VB_GemInventory->AddChild(EmptyText);
		}

		return;
	}

	for (FOwnedGemStack& Stack : Inventory)
	{
		UTextBlock* RowText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

		if (!RowText)
		{
			continue;
		}

		FString RowString = FString::Printf(TEXT("%s | %s | %s | x%d"), *Stack.GemID.ToString(), *GemTypeToString(Stack.GemType), *GemGradeToString(Stack.Grade),Stack.Count);

		RowText->SetText(FText::FromString(RowString));
		VB_GemInventory->AddChild(RowText);
	}
}

void UProject_GemCoopLobbyWidget::RefreshCodex()
{
	UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance();

	if (!GI || !VB_Codex || !WidgetTree)
	{
		return;
	}

	VB_Codex->ClearChildren();

	if (GI->GemCodexData.Num() <= 0)
	{
		UTextBlock* EmptyText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

		if (EmptyText)
		{
			EmptyText->SetText(FText::FromString(TEXT("No codex entries.")));
			VB_Codex->AddChild(EmptyText);
		}

		return;
	}

	for (TPair<FName, FGemCodexEntry>& Pair : GI->GemCodexData)
	{
		UTextBlock* RowText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

		if (!RowText)
		{
			continue;
		}

		FString RowString = FString::Printf(TEXT("GemID: %s | Collected"), *Pair.Key.ToString());

		RowText->SetText(FText::FromString(RowString));
		VB_Codex->AddChild(RowText);
	}
}

FString UProject_GemCoopLobbyWidget::GemTypeToString(EGemType GemType) const
{
	switch (GemType)
	{
	case EGemType::Ruby:
		return TEXT("Ruby");

	case EGemType::Sapphire:
		return TEXT("Sapphire");

	case EGemType::Topaz:
		return TEXT("Topaz");

	case EGemType::Emerald:
		return TEXT("Emerald");

	case EGemType::Amethyst:
		return TEXT("Amethyst");

	default:
		return TEXT("None");
	}
}

FString UProject_GemCoopLobbyWidget::GemGradeToString(EGemGrade Grade) const
{
	switch (Grade)
	{
	case EGemGrade::Common:
		return TEXT("Common");

	case EGemGrade::Flawless:
		return TEXT("Flawless");

	case EGemGrade::Star:
		return TEXT("Star");

	case EGemGrade::Prismatic:
		return TEXT("Prismatic");

	default:
		return TEXT("Unknown");
	}
}