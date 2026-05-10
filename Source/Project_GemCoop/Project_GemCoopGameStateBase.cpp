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

void AProject_GemCoopGameStateBase::BeginPlay()
{
    Super::BeginPlay();

    SafeZoneCycleTimer = SafeZoneCooldown;
}

void AProject_GemCoopGameStateBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!HasAuthority())
    {
        return;
    }

    TickEnergyRegen(DeltaTime);
    UpdateSafeZone(DeltaTime);
}

void AProject_GemCoopGameStateBase::ConsumeEnergy(float Amount)
{
    if (!HasAuthority())
    {
        return;
    }

    SharedEnergy = FMath::Clamp(SharedEnergy - Amount, 0.f, MaxSharedEnergy);
}

bool AProject_GemCoopGameStateBase::HasEnoughEnergy(float Amount) const
{
    return SharedEnergy >= Amount;
}

void AProject_GemCoopGameStateBase::AddMaxEnergy(float Amount)
{
    if (!HasAuthority())
    {
        return;
    }

    MaxSharedEnergy = FMath::Clamp(MaxSharedEnergy + Amount, 100.f, 150.f);
    SharedEnergy = FMath::Min(SharedEnergy + Amount, MaxSharedEnergy);
}

void AProject_GemCoopGameStateBase::TickEnergyRegen(float DeltaTime)
{
    SharedEnergy = FMath::Clamp(SharedEnergy + EnergyRegenRate * DeltaTime, 0.f, MaxSharedEnergy);
}

void AProject_GemCoopGameStateBase::OnRep_CurrentWaveNumber()
{
    OnWaveNumberChanged.Broadcast(CurrentWaveNumber);
}

void AProject_GemCoopGameStateBase::OnRep_SharedEnergy()
{
    OnSharedEnergyChanged.Broadcast(SharedEnergy, MaxSharedEnergy);

    TArray<AActor*> Chars;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProject_GemCoopCharacter::StaticClass(), Chars);

    for (AActor* A : Chars)
    {
        auto* C = Cast<AProject_GemCoopCharacter>(A);
        if (C && C->EnergySYCompRef)
        {
            C->EnergySYCompRef->OnEnergyChangedCallback(SharedEnergy);
        }
    }
}

void AProject_GemCoopGameStateBase::SetFormationActive(bool bActive)
{
    if (!HasAuthority())
    {
        return;
    }

    if (bFormationActive == bActive)
    {
        return;
    }

    bFormationActive = bActive;

    TArray<AActor*> Chars;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProject_GemCoopCharacter::StaticClass(), Chars);

    for (AActor* A : Chars)
    {
        auto* Char = Cast<AProject_GemCoopCharacter>(A);

        if (!Char)
        {
            continue;
        }

        FStatModifier FormMod;

        if (bActive)
        {
            FormMod.DEFMultiplier = 1.f + FormationBonus_Defense;
            FormMod.ATKMultiplier = 1.f + FormationBonus_ATK;
        }
        else
        {
            FormMod.DEFMultiplier = 1.f;
            FormMod.ATKMultiplier = 1.f;
        }

        Char->StatCompRef->SetModifier(FormMod);
    }
}

void AProject_GemCoopGameStateBase::OnRep_FormationActive()
{
    OnFormationStateChanged.Broadcast(bFormationActive);
}

void AProject_GemCoopGameStateBase::UpdateSafeZone(float DeltaTime)
{
    if (!HasAuthority())
    {
        return;
    }

    SafeZoneCycleTimer -= DeltaTime;

    if (SafeZoneCycleTimer <= 0.f)
    {
        SafeZoneActive = !SafeZoneActive;
        SafeZoneCycleTimer = SafeZoneActive ? SafeZoneDuration : SafeZoneCooldown;
    }
}

void AProject_GemCoopGameStateBase::OnRep_SafeZoneActive()
{
    OnSafeZoneChanged.Broadcast(SafeZoneActive);

    TArray<AActor*> Chars;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProject_GemCoopCharacter::StaticClass(), Chars);

    for (AActor* A : Chars)
    {
        auto* C = Cast<AProject_GemCoopCharacter>(A);

        if (C && C->EnergySYCompRef)
        {
            C->EnergySYCompRef->ApplySafeZoneBonus(SafeZoneActive);
        }
    }
}