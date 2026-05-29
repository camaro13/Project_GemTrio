// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopLobbyWidget.h"
#include "Project_GemCoopGameInstance.h"
#include "Project_GemCoopGemInvenRowWidget.h"
#include "Project_GemCoopTypes.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "Project_GemCoopSettingsWidget.h"

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

	if (BTN_CreateStart)
	{
		BTN_CreateStart->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedCreateStart);
	}

	if (BTN_JoinStart)
	{
		BTN_JoinStart->OnClicked.AddDynamic(this, &UProject_GemCoopLobbyWidget::OnClickedJoinStart);
	}

	if (UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance())
	{
		GI->LoadGameData();
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

void UProject_GemCoopLobbyWidget::EquipGemFromInventory(int32 SlotIndex, FName GemID)
{
	UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance();

	if (!GI)
	{
		return;
	}

	bool bEquipped = GI->EquipGemToSlot(SlotIndex, GemID);

	if (!bEquipped)
	{
		UE_LOG(LogTemp, Warning, TEXT("Lobby equip failed. Slot=%d GemID=%s"),
			SlotIndex,
			*GemID.ToString()
		);
		return;
	}

	GI->SaveGameToSlot();
	GI->DebugPrintEquippedGems();

	RefreshEquippedGems();
	RefreshGemInventory();
}

void UProject_GemCoopLobbyWidget::RefreshEquippedGems()
{
	UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance();

	if (!GI)
	{
		return;
	}

	if (TXT_EquippedQ)
	{
		TXT_EquippedQ->SetText(FText::FromString(FString::Printf(TEXT("Q: %s"), *GI->GetEquippedGemID(0).ToString())));
	}

	if (TXT_EquippedW)
	{
		TXT_EquippedW->SetText(FText::FromString(FString::Printf(TEXT("W: %s"), *GI->GetEquippedGemID(1).ToString())));
	}

	if (TXT_EquippedE)
	{
		TXT_EquippedE->SetText(FText::FromString(FString::Printf(TEXT("E: %s"), *GI->GetEquippedGemID(2).ToString())));
	}
}

void UProject_GemCoopLobbyWidget::RefreshLobby()
{
	RefreshGold();
	RefreshEquippedGems();
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
	if (TXT_CreateStatus)
	{
		TXT_CreateStatus->SetText(FText::FromString(TEXT("Create private game.")));
	}

	if (ETB_RoomName)
	{
		ETB_RoomName->SetText(FText::FromString(TEXT("MyRoom")));
	}

	SwitchToPanel(Panel_CreateGame);

	UE_LOG(LogTemp, Warning, TEXT("Create Game panel opened. Not implemented yet."));
}

void UProject_GemCoopLobbyWidget::OnClickedJoinGame()
{
	if (TXT_JoinStatus)
	{
		TXT_JoinStatus->SetText(FText::FromString(TEXT("Enter room code")));
	}

	if (ETB_JoinCode)
	{
		ETB_JoinCode->SetText(FText::GetEmpty());
	}

	SwitchToPanel(Panel_JoinGame);

	UE_LOG(LogTemp, Warning, TEXT("Join Game panel opened. Not implemented yet."));
}

void UProject_GemCoopLobbyWidget::OnClickedGemInventory()
{
	if (UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance())
	{
		GI->LoadGameData();
		GI->DebugPrintGemInventory();
	}
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

void UProject_GemCoopLobbyWidget::OnClickedCreateStart()
{
	FString RoomName = TEXT("MyRoom");

	if (ETB_RoomName)
	{
		RoomName = ETB_RoomName->GetText().ToString();
	}

	if (RoomName.TrimStartAndEnd().IsEmpty())
	{
		if (TXT_CreateStatus)
		{
			TXT_CreateStatus->SetText(FText::FromString(TEXT("Room name is empty.")));
		}

		UE_LOG(LogTemp, Warning, TEXT("CreateStart failed. RoomName is empty."));
		return;
	}

	if (UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance())
	{
		GI->CurrentSessionInfo = RoomName;
		GI->bIsOnlineSession = false;
		GI->SaveGameToSlot();
	}

	if (TXT_CreateStatus)
	{
		TXT_CreateStatus->SetText(FText::FromString(TEXT("Starting game...")));
	}

	UE_LOG(LogTemp, Warning, TEXT("Create private game. RoomName=%s"), *RoomName);

	if (!GameLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, GameLevelName);
	}
}

void UProject_GemCoopLobbyWidget::OnClickedJoinStart()
{
	FString JoinCode;

	if (ETB_JoinCode)
	{
		JoinCode = ETB_JoinCode->GetText().ToString();
	}

	if (JoinCode.TrimStartAndEnd().IsEmpty())
	{
		if (TXT_JoinStatus)
		{
			TXT_JoinStatus->SetText(FText::FromString(TEXT("Join code is empty.")));
		}

		UE_LOG(LogTemp, Warning, TEXT("JoinStart failed. JoinCode is empty."));
		return;
	}

	if (TXT_JoinStatus)
	{
		TXT_JoinStatus->SetText(FText::FromString(TEXT("Join system is not implemented yet.")));
	}

	UE_LOG(LogTemp, Warning, TEXT("Join Game requested. Code=%s. Not implemented yet."), *JoinCode);
}

void UProject_GemCoopLobbyWidget::RefreshGemInventory()
{
	UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance();

	if (!GI || !VB_GemInventory || !WidgetTree)
	{
		return;
	}

	VB_GemInventory->ClearChildren();

	RefreshEquippedGems();	

	TArray<FOwnedGemStack> Inventory = GI->GetGemInventory();

	if (Inventory.Num() <= 0)
	{
		UTextBlock* EmptyText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

		if (EmptyText)
		{
			EmptyText->SetText(FText::FromString(TEXT("No gems owned.")));
			VB_GemInventory->AddChild(EmptyText);
		}

		return;
	}

	for (const FOwnedGemStack& Stack : Inventory)
	{
		if (GemInventoryRowWidgetClass)
		{
			UProject_GemCoopGemInvenRowWidget* RowWidget = CreateWidget<UProject_GemCoopGemInvenRowWidget>(GetOwningPlayer(), GemInventoryRowWidgetClass);

			if (RowWidget)
			{
				RowWidget->SetupRow(this, Stack.GemID, Stack.GemType, Stack.Grade, Stack.Count);

				VB_GemInventory->AddChild(RowWidget);
			}
		}
		/*else
		{
			UTextBlock* RowText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

			if (!RowText)
			{
				continue;
			}

			FString RowString = FString::Printf(TEXT("%s | %s | %s | x%d"), *Stack.GemID.ToString(), *GemTypeToString(Stack.GemType), *GemGradeToString(Stack.Grade), Stack.Count);

			RowText->SetText(FText::FromString(RowString));
			VB_GemInventory->AddChild(RowText);
		}*/
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

	GI->LoadGameData();

	const int32 CollectedCount = GI->GemCodexData.Num();

	if (CollectedCount <= 0)
	{
		UTextBlock* EmptyText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

		if (EmptyText)
		{
			EmptyText->SetText(FText::FromString(TEXT("No codex entries.")));
			VB_Codex->AddChild(EmptyText);
		}

		return;
	}

	for (const TPair<FName, FGemCodexEntry>& Pair : GI->GemCodexData)
	{
		UTextBlock* RowText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

		if (!RowText)
		{
			continue;
		}

		const FString RowString = FString::Printf(TEXT("Collected Gem: %s"), *Pair.Key.ToString());

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