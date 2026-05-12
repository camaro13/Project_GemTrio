// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopGameStateBase.h"
#include "Project_GemCoopCharacter.h"
#include "Project_GemCoopEnergySYComponent.h"
#include "Project_GemCoopStatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AProject_GemCoopGameStateBase::AProject_GemCoopGameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    SharedEnergy = 100.f;
    MaxSharedEnergy = 100.f;
    EnergyRegenRate = 5.f;
    EnergyRegenMultiplier = 1.f;
    SafeZoneEnergyRegenMultiplier = 2.f;

    CurrentWaveNumber = 0;
    GameElapsedTime = 0.0f;

    SafeZoneActive = false;
    SafeZoneDuration = 3.f;
    SafeZoneCooldown = 10.f;
    SafeZoneCycleTimer = 0.0f;

    bFormationActive = false;
    FormationBonus_Defense = 0.3f;
    FormationBonus_ATK = 0.15f;

    FormationMinDistance = 3000.f;
    FormationMaxDistance = 7000.f;
}

void AProject_GemCoopGameStateBase::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        SharedEnergy = FMath::Clamp(SharedEnergy, 0.0f, MaxSharedEnergy);
    }

    OnSharedEnergyChanged.Broadcast(SharedEnergy, MaxSharedEnergy);
    OnWaveNumberChanged.Broadcast(CurrentWaveNumber);
    OnSafeZoneChanged.Broadcast(SafeZoneActive);
    OnFormationStateChanged.Broadcast(bFormationActive);
}

void AProject_GemCoopGameStateBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!HasAuthority())
    {
        return;
    }

    GameElapsedTime += DeltaTime;

    TickEnergyRegen(DeltaTime);
    UpdateSafeZone(DeltaTime);
}

void AProject_GemCoopGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AProject_GemCoopGameStateBase, SharedEnergy);
    DOREPLIFETIME(AProject_GemCoopGameStateBase, MaxSharedEnergy);
    DOREPLIFETIME(AProject_GemCoopGameStateBase, bFormationActive);
    DOREPLIFETIME(AProject_GemCoopGameStateBase, CurrentWaveNumber);
    DOREPLIFETIME(AProject_GemCoopGameStateBase, GameElapsedTime);
    DOREPLIFETIME(AProject_GemCoopGameStateBase, SafeZoneActive);
    DOREPLIFETIME(AProject_GemCoopGameStateBase, ActiveFusionPreview);
}

bool AProject_GemCoopGameStateBase::ConsumeEnergy(float Amount)
{
    if (!HasAuthority())
    {
        return false;
    }

    if (Amount <= 0.0f)
    {
        return true;
    }

    if (!HasEnoughEnergy(Amount))
    {
        return false;
    }

    SharedEnergy = FMath::Clamp(SharedEnergy - Amount, 0.0f, MaxSharedEnergy);

    OnSharedEnergyChanged.Broadcast(SharedEnergy, MaxSharedEnergy);

    UE_LOG(LogTemp, Warning, TEXT("GameState ConsumeEnergy: %.1f, Current %.1f / %.1f"),
        Amount,
        SharedEnergy,
        MaxSharedEnergy
    );

    return true;
}

bool AProject_GemCoopGameStateBase::HasEnoughEnergy(float Amount) const
{
    return SharedEnergy >= Amount;
}

void AProject_GemCoopGameStateBase::AddEnergy(float Amount)
{
    if (!HasAuthority())
    {
        return;
    }

    if (Amount <= 0.0f)
    {
        return;
    }

    SharedEnergy = FMath::Clamp(SharedEnergy + Amount, 0.0f, MaxSharedEnergy);

    OnSharedEnergyChanged.Broadcast(SharedEnergy, MaxSharedEnergy);
}

void AProject_GemCoopGameStateBase::AddMaxEnergy(float Amount)
{
    if (!HasAuthority())
    {
        return;
    }

    if (Amount <= 0.0f)
    {
        return;
    }

    MaxSharedEnergy = FMath::Clamp(MaxSharedEnergy + Amount, 1.f, 150.f);
    SharedEnergy = FMath::Clamp(SharedEnergy + Amount, 0.0f, MaxSharedEnergy);

    OnSharedEnergyChanged.Broadcast(SharedEnergy, MaxSharedEnergy);
}

void AProject_GemCoopGameStateBase::SetEnergyRegenMultiplier(float NewMultiplier)
{
    if (!HasAuthority())
    {
        return;
    }

    EnergyRegenMultiplier = FMath::Max(0.0f, NewMultiplier);
}

void AProject_GemCoopGameStateBase::TickEnergyRegen(float DeltaTime)
{
    if (!HasAuthority())
    {
        return;
    }

    if (SharedEnergy >= MaxSharedEnergy)
    {
        return;
    }

    float RegenAmount = GetCurrentEnergyRegenRate() * DeltaTime;

    if (RegenAmount <= 0.0f)
    {
        return;
    }

    SharedEnergy = FMath::Clamp(SharedEnergy + RegenAmount, 0.0f, MaxSharedEnergy);

    OnSharedEnergyChanged.Broadcast(SharedEnergy, MaxSharedEnergy);
}

float AProject_GemCoopGameStateBase::GetEnergyPercent() const
{
    if (MaxSharedEnergy <= 0.0f)
    {
        return 0.0f;
    }

    return SharedEnergy / MaxSharedEnergy;
}

void AProject_GemCoopGameStateBase::SetCurrentWaveNumber(int32 NewWaveNumber)
{
    if (!HasAuthority())
    {
        return;
    }

    CurrentWaveNumber = FMath::Max(0, NewWaveNumber);

    OnWaveNumberChanged.Broadcast(CurrentWaveNumber);

    UE_LOG(LogTemp, Warning, TEXT("GameState WaveNumber Updated: %d"), CurrentWaveNumber);
}

void AProject_GemCoopGameStateBase::SetSafeZoneActive(bool bNewActive)
{
    if (!HasAuthority())
    {
        return;
    }

    if (SafeZoneActive == bNewActive)
    {
        return;
    }

    SafeZoneActive = bNewActive;
    SafeZoneCycleTimer = 0.0f;

    OnSafeZoneChanged.Broadcast(SafeZoneActive);

    UE_LOG(LogTemp, Warning, TEXT("SafeZoneActive: %d"), SafeZoneActive);
}

void AProject_GemCoopGameStateBase::UpdateSafeZone(float DeltaTime)
{
    if (!HasAuthority())
    {
        return;
    }

    SafeZoneCycleTimer += DeltaTime;

    if (SafeZoneActive)
    {
        if (SafeZoneCycleTimer >= SafeZoneDuration)
        {
            SetSafeZoneActive(false);
        }
    }
    else
    {
        if (SafeZoneCycleTimer >= SafeZoneCooldown)
        {
            SetSafeZoneActive(true);
        }
    }
}

void AProject_GemCoopGameStateBase::SetFormationActive(bool bNewActive)
{
    if (!HasAuthority())
    {
        return;
    }

    if (bFormationActive == bNewActive)
    {
        return;
    }

    bFormationActive = bNewActive;

    OnFormationStateChanged.Broadcast(bFormationActive);

    UE_LOG(LogTemp, Warning, TEXT("FormationActive: %d"), bFormationActive);
}

TArray<AProject_GemCoopCharacter*> AProject_GemCoopGameStateBase::GetPartyMembers() const
{
    TArray<AProject_GemCoopCharacter*> Result;
    TArray<AActor*> FoundActors;

    UWorld* World = GetWorld();

    if (!World)
    {
        return Result;
    }

    UGameplayStatics::GetAllActorsOfClass(World, AProject_GemCoopCharacter::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        AProject_GemCoopCharacter* Character = Cast<AProject_GemCoopCharacter>(Actor);

        if (Character)
        {
            Result.Add(Character);
        }
    }

    return Result;
}

bool AProject_GemCoopGameStateBase::IsInFormationRange() const
{
    TArray<AProject_GemCoopCharacter*> PartyMembers = GetPartyMembers();

    if (PartyMembers.Num() < 3)
    {
        return false;
    }

    for (int32 i = 0; i < PartyMembers.Num(); ++i)
    {
        AProject_GemCoopCharacter* A = PartyMembers[i];

        if (!A || !A->bIsAlive)
        {
            return false;
        }

        for (int32 j = i + 1; j < PartyMembers.Num(); ++j)
        {
            AProject_GemCoopCharacter* B = PartyMembers[j];

            if (!B || !B->bIsAlive)
            {
                return false;
            }

            float Distance = FVector::Dist(A->GetActorLocation(), B->GetActorLocation());

            if (Distance < FormationMinDistance || Distance > FormationMaxDistance)
            {
                return false;
            }
        }
    }

    return true;
}

void AProject_GemCoopGameStateBase::OnRep_SharedEnergy()
{
    OnSharedEnergyChanged.Broadcast(SharedEnergy, MaxSharedEnergy);
}

void AProject_GemCoopGameStateBase::OnRep_MaxSharedEnergy()
{
    OnSharedEnergyChanged.Broadcast(SharedEnergy, MaxSharedEnergy);
}

void AProject_GemCoopGameStateBase::OnRep_CurrentWaveNumber()
{
    OnWaveNumberChanged.Broadcast(CurrentWaveNumber);
}

void AProject_GemCoopGameStateBase::OnRep_SafeZoneActive()
{
    OnSafeZoneChanged.Broadcast(SafeZoneActive);
}

void AProject_GemCoopGameStateBase::OnRep_FormationActive()
{
    OnFormationStateChanged.Broadcast(bFormationActive);
}

float AProject_GemCoopGameStateBase::GetCurrentEnergyRegenRate() const
{
    float Result = EnergyRegenRate * EnergyRegenMultiplier;

    if (SafeZoneActive)
    {
        Result *= SafeZoneEnergyRegenMultiplier;
    }

    return Result;
}