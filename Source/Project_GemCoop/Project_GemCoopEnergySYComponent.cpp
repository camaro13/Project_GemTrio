// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopEnergySYComponent.h"
#include "Project_GemCoopGameStateBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UProject_GemCoopEnergySYComponent::UProject_GemCoopEnergySYComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UProject_GemCoopEnergySYComponent::BeginPlay()
{
	Super::BeginPlay();

	GameStateRef = Cast<AProject_GemCoopGameStateBase>(UGameplayStatics::GetGameState(this));

	if (GameStateRef)
	{
		CachedSharedEnergy = GameStateRef->SharedEnergy;
	}
	// ...
}


// Called every frame
void UProject_GemCoopEnergySYComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SyncWithGameState();
	// ...
}

bool UProject_GemCoopEnergySYComponent::TryConsumeEnergy(float Amount)
{
	if (CachedSharedEnergy < Amount)
	{
		OnEnergyInsufficient.Broadcast(Amount, CachedSharedEnergy);
		return false;
	}

	ServerRPC_ConsumeEnergy(Amount);

	ChargeUltGauge(UltGaugeChargeRate);

	return true;
}

void UProject_GemCoopEnergySYComponent::ServerRPC_ConsumeEnergy_Implementation(float Amount)
{
	if (GameStateRef)
	{
		GameStateRef->ConsumeEnergy(Amount);
	}
}

void UProject_GemCoopEnergySYComponent::OnEnergyChangedCallback(float NewEnergy)
{
	CachedSharedEnergy = NewEnergy;
	float MaxEnergy = GameStateRef ? GameStateRef->MaxSharedEnergy : 100.f;
	OnEnergyChanged.Broadcast(CachedSharedEnergy, MaxEnergy);
}

float UProject_GemCoopEnergySYComponent::GetCurrentEnergy() const
{
	return CachedSharedEnergy;
}

float UProject_GemCoopEnergySYComponent::GetEnergyPercent() const
{
	float MaxEnergy = GameStateRef ? GameStateRef->MaxSharedEnergy : 100.f;
	return MaxEnergy > 0.f ? CachedSharedEnergy / MaxEnergy : 0.f;
}

void UProject_GemCoopEnergySYComponent::ApplySafeZoneBonus(bool bEntering)
{
	if (bInSafeZone == bEntering)
	{
		return;
	}

	bInSafeZone = bEntering;

	if (!GameStateRef)
	{
		return;
	}

	if (bEntering)
	{
		GameStateRef->EnergyRegenRate *= SafeZoneRegenBonus;
	}
	else
	{
		GameStateRef->EnergyRegenRate /= SafeZoneRegenBonus;
	}
}

void UProject_GemCoopEnergySYComponent::ChargeUltGauge(float Amount)
{
	UltGaugeContribution = FMath::Clamp(UltGaugeContribution + Amount, 0.f, 100.f);
	OnUltGaugeChanged.Broadcast(UltGaugeContribution / 100.f);

	if (UltGaugeContribution >= 100.f)
	{
		bUltimateReady = true;
	}
}

void UProject_GemCoopEnergySYComponent::ConsumeUltGauge()
{
	UltGaugeContribution = 0.f;
	bUltimateReady = false;
	OnUltGaugeChanged.Broadcast(0.f);
}

void UProject_GemCoopEnergySYComponent::SyncWithGameState()
{
	if (!GameStateRef)
	{
		return;
	}

	float ServerEnergy = GameStateRef->SharedEnergy;

	if (!FMath::IsNearlyEqual(CachedSharedEnergy, ServerEnergy, 0.5f))
	{
		OnEnergyChangedCallback(ServerEnergy);
	}
}

