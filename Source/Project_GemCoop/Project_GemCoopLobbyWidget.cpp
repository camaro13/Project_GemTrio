// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopLobbyWidget.h"

#include "Project_GemCoopGameInstance.h"
#include "Project_GemCoopTypes.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

void UProject_GemCoopLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BTN_StartGame)
	{
		BTN_StartGame->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedStartGame);
	}

	if (BTN_Save)
	{
		BTN_Save->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedSave);
	}

	if (BTN_Load)
	{
		BTN_Load->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedLoad);
	}

	if (BTN_Quit)
	{
		BTN_Quit->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedQuit);
	}

	if (UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance())
	{
		GI->OnGemInventoryChanged.AddDynamic(this, &UProject_GemCoopLobbyWidget::RefreshLobby);
	}

	RefreshLobby();
}

UProject_GemCoopGameInstance* UProject_GemCoopLobbyWidget::GetGemCoopGameInstance() const
{
	return GetWorld() ? Cast<UProject_GemCoopGameInstance>(GetWorld()->GetGameInstance()) : nullptr;
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

void UProject_GemCoopLobbyWidget::RefreshLobby()
{
	RefreshGold();
	RefreshTrait();
	RefreshGemInventory();
}

void UProject_GemCoopLobbyWidget::RefreshTrait()
{
	UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance();

	if (!GI || !TXT_Trait)
	{
		return;
	}

	FString TraitText = TEXT("Offense");

	switch (GI->LocalPlayerTrait)
	{
	case EGemTrait::Offense:
		TraitText = TEXT("Offense");
		break;

	case EGemTrait::Defense:
		TraitText = TEXT("Defense");
		break;

	case EGemTrait::Support:
		TraitText = TEXT("Support");
		break;

	default:
		TraitText = TEXT("None");
		break;
	}

	TXT_Trait->SetText(FText::FromString(FString::Printf(TEXT("Trait: %s"), *TraitText)));
}

void UProject_GemCoopLobbyWidget::RefreshGemInventory()
{
	UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance();

	if (!GI || !VB_GemInventory)
	{
		return;
	}

	VB_GemInventory->ClearChildren();

	TArray<FOwnedGemStack> Inventory = GI->GetGemInventory();

	if (Inventory.Num() <= 0)
	{
		UTextBlock* EmptyText = WidgetTree ? WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass()) : NewObject<UTextBlock>(this);

		if (EmptyText)
		{
			EmptyText->SetText(FText::FromString(TEXT("No gems owned.")));
			VB_GemInventory->AddChild(EmptyText);
		}

		return;
	}

	for (const FOwnedGemStack& Stack : Inventory)
	{
		UTextBlock* RowText = WidgetTree ? WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass()) : NewObject<UTextBlock>(this);
		
		if (!RowText)
		{
			continue;
		}

		FString RowString = FString::Printf(TEXT("%s | %s | %s | x%d"), *Stack.GemID.ToString(), *GemTypeToString(Stack.GemType), *GemGradeToString(Stack.Grade), Stack.Count);

		RowText->SetText(FText::FromString(RowString));
		VB_GemInventory->AddChild(RowText);
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

	case EGemType::Emerald:
		return TEXT("Emerald");

	case EGemType::Topaz:
		return TEXT("Topaz");

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

	case EGemGrade::Prismatic:
		return TEXT("Prismatic");

	case EGemGrade::Star:
		return TEXT("Star");

	default:
		return TEXT("Unknown");
	}
}

void UProject_GemCoopLobbyWidget::OnClickedStartGame()
{
	if (UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance())
	{
		GI->SaveGameToSlot();
	}

	if (!GameLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, GameLevelName);
	}
}

void UProject_GemCoopLobbyWidget::OnClickedSave()
{
	if (UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance())
	{
		GI->SaveGameToSlot();
		GI->DebugPrintGemInventory();
	}

	RefreshLobby();
}

void UProject_GemCoopLobbyWidget::OnClickedLoad()
{
	if (UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance())
	{
		GI->LoadGameData();
		GI->DebugPrintGemInventory();
	}

	RefreshLobby();
}

void UProject_GemCoopLobbyWidget::OnClickedQuit()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->ConsoleCommand(TEXT("quit"));
	}
}