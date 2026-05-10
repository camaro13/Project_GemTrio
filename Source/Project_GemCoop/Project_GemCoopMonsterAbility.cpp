// Fill out your copyright notice in the Description page of Project Settings.

#include "Project_GemCoopMonsterAbility.h"
#include "Project_GemCoopMonsterCharacter.h"
#include "Project_GemCoopObjectPSubsystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

bool UProject_GemCoopMonsterAbility::CanActivate() const
{
	return bUnlocked && CurrentCooldown <= 0.f;
}

void UProject_GemCoopMonsterAbility::TickCooldown(float DeltaTime)
{
	CurrentCooldown = FMath::Max(0.f, CurrentCooldown - DeltaTime);
}

void UProject_GemCoopMonsterAbility::OnAbilityEnd()
{
	CurrentCooldown = Cooldown;
}

void UProject_GemCoopMonsterAbility::ActivateAbility_Implementation(AActor* Target)
{
	OnAbilityEnd();
}

// ── BossAbility_Charge ────────────────────────────────────────
void UBossAbility_Charge::ActivateAbility_Implementation(AActor* Target)
{
    if (!Target || !OwnerMonster) return;

    FVector Dir = (Target->GetActorLocation()
        - OwnerMonster->GetActorLocation()).GetSafeNormal();

    // LaunchCharacter로 2000cm 돌진
    OwnerMonster->LaunchCharacter(Dir * 2000.f, true, true);

    // 0.3초 후 충돌 판정
    FTimerHandle HitTimer;
    TWeakObjectPtr<AActor> WeakTarget(Target);
    TWeakObjectPtr<AProject_GemCoopMonsterCharacter> WeakOwner(OwnerMonster);

    OwnerMonster->GetWorldTimerManager().SetTimer(HitTimer,
        FTimerDelegate::CreateLambda([WeakOwner, WeakTarget]() {
            if (!WeakOwner.IsValid() || !WeakTarget.IsValid()) return;
            float Dist = FVector::Dist(
                WeakOwner->GetActorLocation(),
                WeakTarget->GetActorLocation());
            if (Dist < 200.f)
                UGameplayStatics::ApplyDamage(WeakTarget.Get(),
                    WeakOwner->ATK * 2.f,
                    WeakOwner->GetController(),
                    WeakOwner.Get(), nullptr);
            }), 0.3f, false);

    OnAbilityEnd();
}

// ── BossAbility_AoESlam ───────────────────────────────────────
void UBossAbility_AoESlam::ActivateAbility_Implementation(AActor* Target)
{
    if (!OwnerMonster) return;

    TArray<AActor*> Ignored;
    Ignored.Add(OwnerMonster);

    UGameplayStatics::ApplyRadialDamage(
        OwnerMonster->GetWorld(),
        OwnerMonster->ATK * 1.5f,       // BaseDamage
        OwnerMonster->GetActorLocation(),// Origin
        SlamRadius,                      // DamageRadius
        nullptr,                         // DamageTypeClass
        Ignored,                         // IgnoreActors
        OwnerMonster,                    // DamageCauser
        OwnerMonster->GetController(),   // Instigator
        true);                           // bDoFullDamage (감쇠 없음)

    OnAbilityEnd();
}

// ── BossAbility_Summon ────────────────────────────────────────
void UBossAbility_Summon::ActivateAbility_Implementation(AActor* Target)
{
    if (!OwnerMonster || !SummonClass) return;

    auto* Pool = OwnerMonster->GetWorld()
        ->GetSubsystem<UProject_GemCoopObjectPSubsystem>();
    if (!Pool) return;

    for (int32 i = 0; i < SummonCount; i++)
    {
        FVector SpawnOffset = FVector(
            FMath::RandRange(-300.f, 300.f),
            FMath::RandRange(-300.f, 300.f), 0.f);
        FVector SpawnLoc = OwnerMonster->GetActorLocation() + SpawnOffset;

        Pool->SpawnFromPool(SummonClass, SpawnLoc, FRotator::ZeroRotator);
    }

    OnAbilityEnd();
}

// ── BossAbility_RageAura ──────────────────────────────────────
void UBossAbility_RageAura::ActivateAbility_Implementation(AActor* Target)
{
    if (!OwnerMonster) return;

    // Phase 2 진입 시 1회만 발동 (OnAbilityEnd로 쿨타임 무한 설정)
    OwnerMonster->ATK *= 1.5f;
    OwnerMonster->MoveSpeed *= 1.3f;
    OwnerMonster->GetCharacterMovement()->MaxWalkSpeed
        = OwnerMonster->MoveSpeed;

    // 쿨타임을 매우 크게 설정해 재발동 방지
    Cooldown = 99999.f;
    OnAbilityEnd();
}