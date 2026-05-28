// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopGemInvenRowWidget.h"
#include "Project_GemCoopLobbyWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UProject_GemCoopGemInvenRowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BTN_EquipQ)
	{
		BTN_EquipQ->OnClicked.AddDynamic(this, &UProject_GemCoopGemInvenRowWidget::OnClickedEquipQ);
	}

	if (BTN_EquipW)
	{
		BTN_EquipW->OnClicked.AddDynamic(this, &UProject_GemCoopGemInvenRowWidget::OnClickedEquipW);
	}

	if (BTN_EquipE)
	{
		BTN_EquipE->OnClicked.AddDynamic(this, &UProject_GemCoopGemInvenRowWidget::OnClickedEquipE);
	}

	//RefreshText();
}

void UProject_GemCoopGemInvenRowWidget::SetupRow(UProject_GemCoopLobbyWidget* InOwner, const FName& InGemID, EGemType InGemType, EGemGrade InGrade, int32 InCount)
{
	OwnerLobbyWidget = InOwner;
	GemID = InGemID;
	GemType = InGemType;
	Grade = InGrade;
	Count = InCount;

	RefreshText();
}

void UProject_GemCoopGemInvenRowWidget::RefreshText()
{
	if (!TXT_GemInfo)
	{
		return;
	}

	const FString Text = FString::Printf(TEXT("%s | %s | %s | x%d"), *GemID.ToString(), *GemTypeToString(GemType), *GemGradeToString(Grade), Count);

	TXT_GemInfo->SetText(FText::FromString(Text));
}

void UProject_GemCoopGemInvenRowWidget::OnClickedEquipQ()
{
	if (OwnerLobbyWidget)
	{
		OwnerLobbyWidget->EquipGemFromInventory(0, GemID);
	}
}

void UProject_GemCoopGemInvenRowWidget::OnClickedEquipW()
{
	if (OwnerLobbyWidget)
	{
		OwnerLobbyWidget->EquipGemFromInventory(1, GemID);
	}
}

void UProject_GemCoopGemInvenRowWidget::OnClickedEquipE()
{
	if (OwnerLobbyWidget)
	{
		OwnerLobbyWidget->EquipGemFromInventory(2, GemID);
	}
}

FString UProject_GemCoopGemInvenRowWidget::GemTypeToString(EGemType InGemType) const
{
	switch (InGemType)
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

FString UProject_GemCoopGemInvenRowWidget::GemGradeToString(EGemGrade InGrade) const
{
	switch (InGrade)
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
		return TEXT("UnKnown");
	}
}