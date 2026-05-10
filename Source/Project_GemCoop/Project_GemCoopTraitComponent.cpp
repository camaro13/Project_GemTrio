// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopTraitComponent.h"
#include "Project_GemCoopBuffComponent.h"
#include "Project_GemCoopEnergySYComponent.h"
#include "Project_GemCoopStatComponent.h"

// Sets default values for this component's properties
UProject_GemCoopTraitComponent::UProject_GemCoopTraitComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UProject_GemCoopTraitComponent::BeginPlay()
{
	Super::BeginPlay();

	BuffCompRef = GetOwner()->FindComponentByClass<UProject_GemCoopBuffComponent>();
	EnergySYCompRef = GetOwner()->FindComponentByClass<UProject_GemCoopEnergySYComponent>();
	// ...
}


// Called every frame
void UProject_GemCoopTraitComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UProject_GemCoopTraitComponent::SetTrait(EGemTrait NewTrait)
{
	if (CurrentTrait == NewTrait)
	{
		return;
	}

	CurrentTrait = NewTrait;

	ApplyTraitMultipliers();
	InitPreferredGems();

	if (BuffCompRef)
	{
		BuffCompRef->SetTraitModifier(GetStatModifier());
	}

	if (EnergySYCompRef)
	{
		EnergySYCompRef->EnergyRegenMultiplier = EnergyRegenMultiplier;
	}

	OnTraitChanged.Broadcast(NewTrait);
}

FStatModifier UProject_GemCoopTraitComponent::GetStatModifier() const
{
	FStatModifier Mod;
	Mod.ATKMultiplier = ATKMultiplier;
	Mod.DEFMultiplier = DEFMultiplier;
	Mod.HPMultiplier = HPMultiplier;
	Mod.HealMultiplier = HealEffectMultiplier;
	Mod.EnergyRegenMultiplier = EnergyRegenMultiplier;
	Mod.CritBonus = CritChanceBonus;
	return Mod;
}

bool UProject_GemCoopTraitComponent::IsPreferredGem(EGemType GemType) const
{
	return PreferredGemTypes.Contains(GemType);
}

float UProject_GemCoopTraitComponent::GetHealBonus() const
{
	return HealEffectMultiplier;
}

float UProject_GemCoopTraitComponent::GetEnergyRegenMultiplier() const
{
	return EnergyRegenMultiplier;
}

FLinearColor UProject_GemCoopTraitComponent::GetTraitColor() const
{
	return TraitColor;
}

void UProject_GemCoopTraitComponent::ApplyTraitMultipliers()
{
	ATKMultiplier = DEFMultiplier = HPMultiplier = SPDMultiplier = 1.0f;
	HealEffectMultiplier = EnergyRegenMultiplier = 1.0f;
	CritChanceBonus = 0.0f;
	TraitColor = FLinearColor::White;

	switch (CurrentTrait)
	{
	case EGemTrait::Offense:
		ATKMultiplier = 1.3f;
		CritChanceBonus = 0.15f;
		HealEffectMultiplier = 0.8f;
		TraitColor = FLinearColor(0.8f, 0.1f, 0.1f, 1.f);
		break;
	case EGemTrait::Defense:
		DEFMultiplier = 1.4f;
		HPMultiplier = 1.3f;
		SPDMultiplier = 0.9f;
		TraitColor = FLinearColor(0.1f, 0.3f, 0.9f, 1.f);
		break;
	case EGemTrait::Support:
		HealEffectMultiplier = 1.5f;
		EnergyRegenMultiplier = 1.3f;
		ATKMultiplier = 0.85;
		TraitColor = FLinearColor(0.1f, 0.7f, 0.2f, 1.f);
		break;
	default:
		break;
	}
}

void UProject_GemCoopTraitComponent::InitPreferredGems()
{
	PreferredGemTypes.Empty();

	switch (CurrentTrait)
	{
	case EGemTrait::Offense:
		PreferredGemTypes.Add(EGemType::Ruby);
		break;
	case EGemTrait::Defense:
		PreferredGemTypes.Add(EGemType::Sapphire);
		PreferredGemTypes.Add(EGemType::Amethyst);
		break;
	case EGemTrait::Support:
		PreferredGemTypes.Add(EGemType::Emerald);
		PreferredGemTypes.Add(EGemType::Topaz);
		break;
	default:
		break;
	}
}

