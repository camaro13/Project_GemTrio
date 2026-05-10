// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopBuffComponent.h"
#include "Project_GemCoopStatComponent.h"

// Sets default values for this component's properties
UProject_GemCoopBuffComponent::UProject_GemCoopBuffComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UProject_GemCoopBuffComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		StatCompRef = Owner->FindComponentByClass<UProject_GemCoopStatComponent>();
	}
}


// Called every frame
void UProject_GemCoopBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TickBuffs(DeltaTime);
	// ...
}

void UProject_GemCoopBuffComponent::AddBuff(FBuffData Buff)
{
	for (FBuffData& Existing : ActiveBuffs)
	{
		if (Existing.BuffID == Buff.BuffID)
		{
			Existing.Duration = FMath::Max(Existing.Duration, Buff.Duration);
			Existing.StackCount = FMath::Min(Existing.StackCount + 1, 5);
			RecalculateBonuses();
			OnBuffChanged.Broadcast();
			return;
		}
	}
	ActiveBuffs.Add(Buff);
	RecalculateBonuses();
	OnBuffChanged.Broadcast();
}

void UProject_GemCoopBuffComponent::AddDebuff(FDebuffData Debuff)
{
	if (Debuff.DebuffID == "Burn")
	{
		bIsBurning = true;
		BurnDamagePerSec = FMath::Max(BurnDamagePerSec, Debuff.DamagePerSec);
	}

	for (FDebuffData& Existing : ActiveDebuffs)
	{
		if (Existing.DebuffID == Debuff.DebuffID)
		{
			Existing.Duration = FMath::Max(Existing.Duration, Debuff.Duration);
			OnBuffChanged.Broadcast();
			return;
		}
	}
	ActiveDebuffs.Add(Debuff);
	OnBuffChanged.Broadcast();
}

void UProject_GemCoopBuffComponent::RemoveBuff(FName BuffID)
{
	ActiveBuffs.RemoveAll([&](const FBuffData& B) {return B.BuffID == BuffID; });
	RecalculateBonuses();
	OnBuffChanged.Broadcast();
}

void UProject_GemCoopBuffComponent::RemoveDebuff(FName DebuffID)
{
	ActiveDebuffs.RemoveAll([&](const FDebuffData& D) {return D.DebuffID == DebuffID; });
	RecalculateBonuses();
	OnBuffChanged.Broadcast();
}

void UProject_GemCoopBuffComponent::TickBuffs(float DeltaTime)
{
	bool bChanged = false;

	for (int32 i = ActiveBuffs.Num() - 1; i >= 0; --i)
	{
		ActiveBuffs[i].Duration -= DeltaTime;

		if (ActiveBuffs[i].Duration <= 0.f)
		{
			ActiveBuffs.RemoveAt(i);
			bChanged = true;
		}
	}

	for (int32 i = ActiveDebuffs.Num() - 1; i >= 0; --i)
	{
		ActiveDebuffs[i].Duration -= DeltaTime;

		if (ActiveDebuffs[i].Duration <= 0.f)
		{
			ActiveDebuffs.RemoveAt(i);
			bChanged = true;
		}
	}

	if (bIsBurning && StatCompRef)
	{
		StatCompRef->ApplyDamage(BurnDamagePerSec * DeltaTime);
	}

	if (bChanged)
	{
		RecalculateBonuses();
		OnBuffChanged.Broadcast();
	}
}

void UProject_GemCoopBuffComponent::RecalculateBonuses()
{
	TotalATKBonus = TotalDEFBonus = TotalHealBonus = TotalSPDBonus = 0.f;

	for (const FBuffData& Buff : ActiveBuffs)
	{
		float Stack = static_cast<float>(Buff.StackCount);
		TotalATKBonus += Buff.ATKBonus * Stack;
		TotalDEFBonus += Buff.DEFBonus * Stack;
		TotalHealBonus += Buff.HealBonus * Stack;
		TotalSPDBonus += Buff.SPDBonus * Stack;

		if (StatCompRef)
		{
			StatCompRef->SetModifier(GetCombinedModifier());
		}
	}
}

FStatModifier UProject_GemCoopBuffComponent::GetCombinedModifier() const
{
	FStatModifier Mod = TraitModifier;
	Mod.ATKMultiplier *= (1.f + TotalATKBonus);
	Mod.DEFMultiplier *= (1.f + TotalDEFBonus);
	Mod.HealMultiplier *= (1.f + TotalHealBonus);
	Mod.SPDMultiplier *= (1.f + TotalSPDBonus);
	return Mod;
}

void UProject_GemCoopBuffComponent::ApplyShield(float Amount)
{
	ShieldAmount += Amount;
	OnShieldChanged.Broadcast(ShieldAmount);
}

void UProject_GemCoopBuffComponent::AbsorbDamage(float& Damage)
{
	if (ShieldAmount <= 0.f)
	{
		return;
	}

	float Absorbed = FMath::Min(ShieldAmount, Damage);
	ShieldAmount -= Absorbed;
	Damage -= Absorbed;
	OnShieldChanged.Broadcast(ShieldAmount);
}

void UProject_GemCoopBuffComponent::AddDistortion(float Amount)
{
	DistortionValue += Amount;
	if (DistortionValue >= MaxDistortion)
	{
		TriggerDistortionExplosion();
	}
}

void UProject_GemCoopBuffComponent::TriggerDistortionExplosion()
{
	if (StatCompRef)
	{
		StatCompRef->ApplyDamage(DistortionValue);
		DistortionValue = 0.f;
	}
}

void UProject_GemCoopBuffComponent::SetTraitModifier(const FStatModifier& Modifier)
{
	TraitModifier = Modifier;
	RecalculateBonuses();
}

int32 UProject_GemCoopBuffComponent::GetBuffCount() const
{
	return ActiveBuffs.Num();
}

int32 UProject_GemCoopBuffComponent::GetDebuffCount() const
{
	return ActiveDebuffs.Num();
}