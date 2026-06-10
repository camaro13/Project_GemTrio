// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopStatComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UProject_GemCoopStatComponent::UProject_GemCoopStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    SetIsReplicatedByDefault(true);
	// ...
}

// Called when the game starts
void UProject_GemCoopStatComponent::BeginPlay()
{
	Super::BeginPlay();

	RecalculateFinalStats();
	CurrentHP = FinalHP;
}


// Called every frame
void UProject_GemCoopStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsDead && RegenRate > 0.f && CurrentHP < FinalHP)
	{
		ApplyHeal(RegenRate * DeltaTime);
	}
}

void UProject_GemCoopStatComponent::RecalculateFinalStats()
{
    float OldFinalHP = FinalHP;
    float OldHPPercent = OldFinalHP > 0.f ? CurrentHP / OldFinalHP : 1.f;

    FinalHP = BaseHP * (1.f + UpgradeHP) * CurrentModifier.HPMultiplier;
    FinalATK = BaseATK * (1.f + UpgradeATK) * CurrentModifier.ATKMultiplier;
    FinalDEF = BaseDEF * (1.f + UpgradeDEF) * CurrentModifier.DEFMultiplier;
    FinalSPD = BaseSPD * (1.f + UpgradeSPD) * CurrentModifier.SPDMultiplier;

    FinalHP = FMath::Max(FinalHP, 1.f);
    FinalATK = FMath::Max(FinalATK, 0.f);
    FinalDEF = FMath::Max(FinalDEF, 0.f);
    FinalSPD = FMath::Max(FinalSPD, 0.f);

    if (bIsDead)
    {
        CurrentHP = 0.f;
    }
    else
    {
        CurrentHP = FMath::Clamp(FinalHP * OldHPPercent, 0.f, FinalHP);
    }

    OnHPChanged.Broadcast(CurrentHP, FinalHP);
}

void UProject_GemCoopStatComponent::ApplyDamage(float Amount)
{
    if (bIsDead || Amount <= 0.f) return;

    CurrentHP = FMath::Max(0.f, CurrentHP - Amount);
    OnHPChanged.Broadcast(CurrentHP, FinalHP);

    if (CurrentHP <= 0.f)
    {
        HandleDeath();
    }
}

void UProject_GemCoopStatComponent::ApplyHeal(float Amount)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }

    if (Amount <= 0.0f || bIsDead)
    {
        return;
    }

    CurrentHP = FMath::Clamp(CurrentHP + Amount, 0.0f, FinalHP);

    OnHPChanged.Broadcast(CurrentHP, FinalHP);
}

bool UProject_GemCoopStatComponent::IsDead() const
{
    return bIsDead;
}

float UProject_GemCoopStatComponent::GetCurrentHPPercent() const
{
    if (FinalHP <= 0.f)
    {
        return 0.f;
    }

    return CurrentHP / FinalHP;
}

void UProject_GemCoopStatComponent::SetCurrentHPPercent(float HPPercent)
{
    const float ClampedPercent = FMath::Clamp(HPPercent, 0.f, 1.f);

    bIsDead = false;
    CurrentHP = FinalHP * ClampedPercent;

    OnHPChanged.Broadcast(CurrentHP, FinalHP);
}

void UProject_GemCoopStatComponent::ApplyPermanentUpgrade(FName UpgradeID, int32 Level)
{
    if (UpgradeID == "HP_Upgrade")
    {
        UpgradeHP = Level * 0.1f;
    }
    else if (UpgradeID == "ATK_Upgrade")
    {
        UpgradeATK = Level * 0.1f;
    }
    else if (UpgradeID == "DEF_Upgrade")
    {
        UpgradeDEF = Level * 0.1f;
    }
    else if (UpgradeID == "SPD_Upgrade")
    {
        UpgradeSPD = Level * 0.05f;
    }
    else if (UpgradeID == "Crit_Upgrade")
    {
        UpgradeCrit = Level * 0.05f;
    }

    RecalculateFinalStats();
}

void UProject_GemCoopStatComponent::SetModifier(const FStatModifier& NewModifier)
{
    CurrentModifier = NewModifier;
    RecalculateFinalStats();
}

void UProject_GemCoopStatComponent::HandleDeath()
{
    if (bIsDead) return;
    bIsDead = true;

    OnDeath.Broadcast();
}

void UProject_GemCoopStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UProject_GemCoopStatComponent, CurrentHP);
    DOREPLIFETIME(UProject_GemCoopStatComponent, FinalHP);
    DOREPLIFETIME(UProject_GemCoopStatComponent, bIsDead);
}

void UProject_GemCoopStatComponent::OnRep_HP()
{
    OnHPChanged.Broadcast(CurrentHP, FinalHP);
}

void UProject_GemCoopStatComponent::OnRep_DeathState()
{
    if (bIsDead)
    {
        OnDeath.Broadcast();
    }
}

void UProject_GemCoopStatComponent::AddTemporaryMaxHP(float Amount, bool bAlsoHeal)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }

    if (Amount <= 0.0f)
    {
        return;
    }

    const float OldFinalHP = FinalHP;

    CurrentModifier.HPMultiplier += Amount;
    RecalculateFinalStats();

    if (bAlsoHeal)
    {
        const float DeltaMaxHP = FinalHP - OldFinalHP;
        CurrentHP = FMath::Clamp(CurrentHP + DeltaMaxHP, 0.0f, FinalHP);
    }

    OnHPChanged.Broadcast(CurrentHP, FinalHP);
}