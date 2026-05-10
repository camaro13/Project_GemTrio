// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopCombatComponent.h"
#include "Project_GemCoopBuffComponent.h"
#include "Project_GemCoopStatComponent.h"
#include "Project_GemCoopMonsterCharacter.h"

// Sets default values for this component's properties
UProject_GemCoopCombatComponent::UProject_GemCoopCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
	// ...
}


// Called when the game starts
void UProject_GemCoopCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	StatCompRef = GetOwner()->FindComponentByClass<UProject_GemCoopStatComponent>();
	BuffCompRef = GetOwner()->FindComponentByClass<UProject_GemCoopBuffComponent>();
	// ...
	
}


// Called every frame
void UProject_GemCoopCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsCasting)
	{
		CastingTimer -= DeltaTime;
		if (CastingTimer <= 0.f)
		{
			bIsCasting = false;
			OnCastingCompleted.Broadcast();
		}
	}
	
	UpdateRecentStats(DeltaTime);
}

void UProject_GemCoopCombatComponent::DealDamage(AActor* Target, float BaseDamage, EGemType DamageType)
{
	if (!Target)
	{
		return;
	}

	auto* TargetStat = Target->FindComponentByClass<UProject_GemCoopStatComponent>();
	
	if (!TargetStat || TargetStat->IsDead())
	{
		return;
	}

	float FinalDmg = CalculateFinalDamage(BaseDamage, Target, DamageType);

	auto* TargetBuff = Target->FindComponentByClass<UProject_GemCoopBuffComponent>();
}

void UProject_GemCoopCombatComponent::RecordHeal(float HealAmount)
{
	TotalHealGiven += HealAmount;
	StatEventLog.Add({ (float)GetWorld()->GetTimeSeconds(), 0.f, HealAmount, 0 });
}

void UProject_GemCoopCombatComponent::RecordBuff()
{
	TotalBuffsApplied++;
	StatEventLog.Add({ (float)GetWorld()->GetTimeSeconds(), 0.f, 0.f, 1 });
}

FContributionRatio UProject_GemCoopCombatComponent::GetRecentContribution() const
{
	float Total = RecentDamage_10s + RecentHeal_10s + (float)RecentBuffs_10s;
	
	if (Total <= 0.f)
	{
		return FContributionRatio();
	}

	FContributionRatio Ratio;
	Ratio.DamageRatio = RecentDamage_10s / Total;
	Ratio.HealRatio = RecentHeal_10s / Total;
	Ratio.BuffRatio = (float)RecentBuffs_10s / Total;

	if (Ratio.DamageRatio <= Ratio.HealRatio && Ratio.DamageRatio <= Ratio.BuffRatio)
	{
		Ratio.LowestContribution = EStatEventType::Damage;
	}
	else if (Ratio.HealRatio <= Ratio.BuffRatio)
	{
		Ratio.LowestContribution = EStatEventType::Heal;
	}
	else
	{
		Ratio.LowestContribution = EStatEventType::Buff;
	}

	return Ratio;
}

void UProject_GemCoopCombatComponent::StartCasting(float CastTime)
{
	bIsCasting = true;
	CastingTimer = CastTime;
	TotalCastTime = CastTime;
	OnCastingStarted.Broadcast(CastTime);
}

void UProject_GemCoopCombatComponent::CancelCasting()
{
	if (!bIsCasting)
	{
		return;
	}

	bIsCasting = false;
	CastingTimer = 0.f;
	OnCastingCancelled.Broadcast();
}

void UProject_GemCoopCombatComponent::OnKill(AActor* DeadActor)
{
	TotalKillCount++;
	// AchievementManager, GameMode 통보는 GameMode 에서 OnMonsterDeath 통해 처리
}

void UProject_GemCoopCombatComponent::UpdateRecentStats(float DeltaTime)
{
	float Cutoff = GetWorld()->GetTimeSeconds() - 10.f;
	StatEventLog.RemoveAll([Cutoff](const FStatEvent& E) {return E.TimeStamp < Cutoff; });

	RecentDamage_10s = 0.f;
	RecentHeal_10s = 0.f;
	RecentBuffs_10s = 0;

	for (const FStatEvent& E : StatEventLog)
	{
		RecentDamage_10s += E.DamageVal;
		RecentHeal_10s += E.HealVal;
		RecentBuffs_10s += E.BuffCount;
	}
}

bool UProject_GemCoopCombatComponent::RollCritical()
{
	float CritRate = StatCompRef ? (StatCompRef->CritChance + StatCompRef->UpgradeCrit) : 0.05f;
	float Roll = FMath::FRand();
	
	if (Roll < CritRate)
	{
		TotalCritCount++;
		return true;
	}
	return false;
}

float UProject_GemCoopCombatComponent::CalculateFinalDamage(float Base, AActor* Target, EGemType Type)
{
	auto* TargetStat = Target->FindComponentByClass<UProject_GemCoopStatComponent>();
	float TargetDEF = TargetStat ? TargetStat->FinalDEF : 0.f;
	float DefReduction = TargetDEF / (TargetDEF + 100.f);
	float AfterDEF = Base * (1.f - DefReduction);
	float TypeMult = IsWeakness(Target, Type) ? 1.5f : 1.0f;
	float AfterType = AfterDEF * TypeMult;
	bool bCrit = RollCritical();
	float Final = bCrit ? AfterType * 2.f : AfterType;

	if (bCrit)
	{
		OnCriticalHit.Broadcast(Final, Target);
	}

	return Final;
}

bool UProject_GemCoopCombatComponent::IsWeakness(AActor* Target, EGemType Type) const
{
	auto* Monster = Cast<AProject_GemCoopMonsterCharacter>(Target);
	return Monster && Monster->WeaknessGemType == Type;
}