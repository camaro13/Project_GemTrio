// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopTraitDSubsystem.h"
//#include "UObject/ConstructorHelpers.h"

void UProject_GemCoopTraitDSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TraitColorMap.Empty();
	TraitColorMap.Add(EGemTrait::Offense, FLinearColor(0.8f, 0.1f, 0.1f, 1.0f));
	TraitColorMap.Add(EGemTrait::Defense, FLinearColor(0.1f, 0.3f, 0.8f, 1.0f));
	TraitColorMap.Add(EGemTrait::Support, FLinearColor(0.1f, 0.7f, 0.2f, 1.0f));

	/*static ConstructorHelpers::FObjectFinder<UDataTable> TraitTableObj(TEXT("/Game/Data/DT_TraitData.DT_TraitData"));

	if (TraitTableObj.Succeeded())
	{
		TraitDataTable = TraitTableObj.Object;
	}*/

	TraitDataTable = LoadObject<UDataTable>(
		nullptr,
		TEXT("/Game/GemBlast/Data/DT_TraitData.DT_TraitData")
	);

	if (!TraitDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("TraitDSubsystem: DT_TraitData not found."));
	}
}

FName UProject_GemCoopTraitDSubsystem::GetEnumCleanName(EGemTrait Trait) const
{
	FString EnumString = UEnum::GetValueAsString(Trait);
	EnumString.RemoveFromStart(TEXT("EGemTrait::"));
	return FName(*EnumString);
}

FTraitData UProject_GemCoopTraitDSubsystem::GetTraitData(EGemTrait Trait) const
{
	if (!TraitDataTable || Trait == EGemTrait::None)
	{
		return FTraitData{};
	}

	FName RowName = GetEnumCleanName(Trait);
	FString ContextString = TEXT("GetTraitData");

	FTraitData* Row = TraitDataTable->FindRow<FTraitData>(RowName, ContextString);

	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("TraitDSubsystem: Trait row not found: %s"), *RowName.ToString());
		return FTraitData{};
	}

	return *Row;
}

FStatModifier UProject_GemCoopTraitDSubsystem::GetTraitStatModifier(EGemTrait Trait) const
{
	FTraitData Data = GetTraitData(Trait);

	if (Data.Trait == EGemTrait::None)
	{
		return FStatModifier{};
	}

	return Data.StatModifier;
}

TArray<EGemType> UProject_GemCoopTraitDSubsystem::GetPreferredGems(EGemTrait Trait) const
{
	FTraitData Data = GetTraitData(Trait);

	return Data.PreferredGems;
}

FLinearColor UProject_GemCoopTraitDSubsystem::GetTraitColor(EGemTrait Trait) const
{
	FTraitData Data = GetTraitData(Trait);

	if (Data.Trait != EGemTrait::None)
	{
		return Data.TraitColor;
	}

	const FLinearColor* FoundColor = TraitColorMap.Find(Trait);
	return FoundColor ? *FoundColor : FLinearColor::White;
}

FText UProject_GemCoopTraitDSubsystem::GetTraitDescription(EGemTrait Trait) const
{
	FTraitData Data = GetTraitData(Trait);

	if (!Data.Description.IsEmpty())
	{
		return Data.Description;
	}

	switch (Trait)
	{
	case EGemTrait::Offense:
		return FText::FromString(TEXT("ATK +30%, Crit +15%, Heal -20%"));

	case EGemTrait::Defense:
		return FText::FromString(TEXT("DEF +40%, HP +30%, SPD -10%"));

	case EGemTrait::Support:
		return FText::FromString(TEXT("Heal +50%, Energy Regen +30%, ATK -15%"));

	default:
		return FText::FromString(TEXT("No trait selected."));
	}
}