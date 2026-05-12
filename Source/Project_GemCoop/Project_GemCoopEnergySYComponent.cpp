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

	GameStateRef = nullptr;

	CachedSharedEnergy = 0.0f;
	CachedMaxSharedEnergy = 100.0f;

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

	GameStateRef = GetWorld() ? GetWorld()->GetGameState<AProject_GemCoopGameStateBase>() : nullptr;
	
	if (GameStateRef)
	{
		CachedSharedEnergy = GameStateRef->SharedEnergy;
		CachedMaxSharedEnergy = GameStateRef->MaxSharedEnergy;

		GameStateRef->OnSharedEnergyChanged.AddDynamic(this, &UProject_GemCoopEnergySYComponent::OnGameStateEnergyChanged);

		OnEnergyChanged.Broadcast(CachedSharedEnergy, CachedMaxSharedEnergy);
	}
	// ...
}


// Called every frame
void UProject_GemCoopEnergySYComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//SyncWithGameState();
	// ...
}

bool UProject_GemCoopEnergySYComponent::TryConsumeEnergy(float Amount)
{
	SyncWithGameState();

	if (!GameStateRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnergySYComponent: GameStateRef missing."));
		return false;
	}

	if (Amount <= 0.0f)
	{
		return true;
	}

	if (!HasEnoughEnergy(Amount))
	{
		UE_LOG(LogTemp, Warning, TEXT("Energy not enough. Required %.1f / Current %.1f"),
			Amount,
			CachedSharedEnergy
		);

		OnEnergyInsufficient.Broadcast(Amount, CachedSharedEnergy);
		return false;
	}

	if (GameStateRef->HasAuthority())
	{
		bool bConsumed = GameStateRef->ConsumeEnergy(Amount);

		if (bConsumed)
		{
			ChargeUltGauge(UltGaugeChargeRate);
		}

		return bConsumed;
	}

	UE_LOG(LogTemp, Warning, TEXT("Energy consume requested on client. Server RPC not implemented yet."));
	return false;
}

bool UProject_GemCoopEnergySYComponent::HasEnoughEnergy(float Amount) const
{
	return CachedSharedEnergy >= Amount;
}

void UProject_GemCoopEnergySYComponent::SyncWithGameState()
{
	if (!GameStateRef && GetWorld())
	{
		GameStateRef = GetWorld()->GetGameState<AProject_GemCoopGameStateBase>();
	}

	if (!GameStateRef)
	{
		return;
	}

	CachedSharedEnergy = GameStateRef->SharedEnergy;
	CachedMaxSharedEnergy = GameStateRef->MaxSharedEnergy;
}

void UProject_GemCoopEnergySYComponent::OnGameStateEnergyChanged(float CurrentEnergy, float MaxEnergy)
{
	CachedSharedEnergy = CurrentEnergy;
	CachedMaxSharedEnergy = MaxEnergy;

	OnEnergyChanged.Broadcast(CachedSharedEnergy, CachedMaxSharedEnergy);
}

//void UProject_GemCoopEnergySYComponent::ServerRPC_ConsumeEnergy_Implementation(float Amount)
//{
//	if (GameStateRef)
//	{
//		GameStateRef->ConsumeEnergy(Amount);
//	}
//}

void UProject_GemCoopEnergySYComponent::OnEnergyChangedCallback(float NewEnergy)
{
	CachedSharedEnergy = NewEnergy;

	OnEnergyChanged.Broadcast(CachedSharedEnergy, CachedMaxSharedEnergy);
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
	bInSafeZone = bEntering;

	UE_LOG(LogTemp, Warning, TEXT("EnergySYComponent SafeZone Bonus: %s"),
		bInSafeZone ? TEXT("ON") : TEXT("OFF")
	);
}

void UProject_GemCoopEnergySYComponent::ChargeUltGauge(float Amount)
{
	if (Amount <= 0.0f)
	{
		return;
	}

	if (bUltimateReady)
	{
		return;
	}

	UltGaugeContribution = FMath::Clamp(UltGaugeContribution + Amount, 0.0f, MaxUltGauge);

	if (UltGaugeContribution >= MaxUltGauge)
	{
		UltGaugeContribution = MaxUltGauge;
		bUltimateReady = true;
	}

	OnUltGaugeChanged.Broadcast(GetUltGaugePercent());

	UE_LOG(LogTemp, Warning, TEXT("UltGauge Charged: %.1f / %.1f Ready=%d"),
		UltGaugeContribution,
		MaxUltGauge,
		bUltimateReady
	);
}

void UProject_GemCoopEnergySYComponent::ConsumeUltGauge()
{
	if (!bUltimateReady)
	{
		return;
	}

	UltGaugeContribution = 0.0f;
	bUltimateReady = false;

	OnUltGaugeChanged.Broadcast(GetUltGaugePercent());

	UE_LOG(LogTemp, Warning, TEXT("Ultimate Gauge Consumed."));
}

float UProject_GemCoopEnergySYComponent::GetUltGaugePercent() const
{
	if (MaxUltGauge <= 0.0f)
	{
		return 0.0f;
	}

	return UltGaugeContribution / MaxUltGauge;
}
