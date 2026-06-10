// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopEnergySYComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UProject_GemCoopEnergySYComponent::UProject_GemCoopEnergySYComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	GameStateRef = nullptr;

	CachedEnergy = 0.0f;
	CachedMaxEnergy = 100.0f;

	EnergyRegenMultiplier = 1.0f;
	SafeZoneRegenBonus = 2.0f;
	bInSafeZone = false;

	UltGaugeContribution = 0.0f;
	UltGaugeChargeRate = 5.0f;
	MaxUltGauge = 100.0f;
	bUltimateReady = false;
	// ...
}

// Called when the game starts
void UProject_GemCoopEnergySYComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		CachedEnergy = FMath::Clamp(CachedEnergy, 0.0f, CachedMaxEnergy);
		UltGaugeContribution = FMath::Clamp(UltGaugeContribution, 0.0f, MaxUltGauge);
		bUltimateReady = UltGaugeContribution >= MaxUltGauge;
	}

	OnEnergyChanged.Broadcast(CachedEnergy, CachedMaxEnergy);
	OnUltGaugeChanged.Broadcast(GetUltGaugePercent());
	// ...
}

// Called every frame
void UProject_GemCoopEnergySYComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (CachedEnergy >= CachedMaxEnergy)
	{
		return;
	}

	float FinalRegenMultiplier = EnergyRegenMultiplier;

	if (bInSafeZone)
	{
		FinalRegenMultiplier += SafeZoneRegenBonus;
	}

	const float RegenAmount = FinalRegenMultiplier * DeltaTime;

	if (RegenAmount > 0.0f)
	{
		SetEnergy_Server(CachedEnergy + RegenAmount);
	}
	//SyncWithGameState();
	// ...
}

void UProject_GemCoopEnergySYComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UProject_GemCoopEnergySYComponent, CachedEnergy);
	DOREPLIFETIME(UProject_GemCoopEnergySYComponent, CachedMaxEnergy);
	DOREPLIFETIME(UProject_GemCoopEnergySYComponent, bInSafeZone);
	DOREPLIFETIME(UProject_GemCoopEnergySYComponent, UltGaugeContribution);
	DOREPLIFETIME(UProject_GemCoopEnergySYComponent, bUltimateReady);
}

void UProject_GemCoopEnergySYComponent::OnRep_Energy()
{
	OnEnergyChanged.Broadcast(CachedEnergy, CachedMaxEnergy);
}

void UProject_GemCoopEnergySYComponent::OnRep_UltGauge()
{
	OnUltGaugeChanged.Broadcast(GetUltGaugePercent());
}

bool UProject_GemCoopEnergySYComponent::TryConsumeEnergy(float Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	if (Amount <= 0.0f)
	{
		return true;
	}

	if (!HasEnoughEnergy(Amount))
	{
		OnEnergyInsufficient.Broadcast(Amount, CachedEnergy);
		return false;
	}

	SetEnergy_Server(CachedEnergy - Amount);

	return true;
}

bool UProject_GemCoopEnergySYComponent::HasEnoughEnergy(float Amount) const
{
	return CachedEnergy >= Amount;
}

void UProject_GemCoopEnergySYComponent::SetEnergy_Server(float NewEnergy)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	CachedEnergy = FMath::Clamp(NewEnergy, 0.0f, CachedMaxEnergy);

	OnEnergyChanged.Broadcast(CachedEnergy, CachedMaxEnergy);
}

void UProject_GemCoopEnergySYComponent::SyncWithUI()
{
	OnEnergyChanged.Broadcast(CachedEnergy, CachedMaxEnergy);
	OnUltGaugeChanged.Broadcast(GetUltGaugePercent());
}

void UProject_GemCoopEnergySYComponent::OnGameStateEnergyChanged(float CurrentEnergy, float MaxEnergy)
{
	
}

void UProject_GemCoopEnergySYComponent::OnEnergyChangedCallback(float NewEnergy)
{

}

float UProject_GemCoopEnergySYComponent::GetCurrentEnergy() const
{
	return CachedEnergy;
}

float UProject_GemCoopEnergySYComponent::GetMaxEnergy() const
{
	return CachedMaxEnergy;
}

float UProject_GemCoopEnergySYComponent::GetEnergyPercent() const
{
	if (CachedMaxEnergy <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(CachedEnergy / CachedMaxEnergy, 0.0f, 1.f);
}

void UProject_GemCoopEnergySYComponent::ApplySafeZoneBonus(bool bEntering)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	bInSafeZone = bEntering;
}

void UProject_GemCoopEnergySYComponent::ChargeUltGauge(float Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (Amount <= 0.0f)
	{
		return;
	}

	UltGaugeContribution = FMath::Clamp(UltGaugeContribution + Amount * UltGaugeChargeRate, 0.0f, MaxUltGauge);

	bUltimateReady = UltGaugeContribution >= MaxUltGauge;

	OnUltGaugeChanged.Broadcast(GetUltGaugePercent());
}

void UProject_GemCoopEnergySYComponent::ConsumeUltGauge()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	UltGaugeContribution = 0.0f;
	bUltimateReady = false;

	OnUltGaugeChanged.Broadcast(GetUltGaugePercent());
}

float UProject_GemCoopEnergySYComponent::GetUltGaugePercent() const
{
	if (MaxUltGauge <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(UltGaugeContribution / MaxUltGauge, 0.0f, 1.f);
}
