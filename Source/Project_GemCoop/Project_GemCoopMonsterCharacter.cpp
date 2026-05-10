// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopMonsterCharacter.h"
#include "Project_GemCoopMonsterAbility.h"
#include "Project_GemCoopCharacter.h"
#include "Project_GemCoopGemComponent.h"
#include "Project_GemCoopObjectPSubsystem.h"
#include "AIController.h"
#include "Net/UnrealNetwork.h"
#include "Project_GemCoop.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProject_GemCoopMonsterCharacter::AProject_GemCoopMonsterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	MaxHP = 80.f;
	CurrentHP = MaxHP;
	ATK = 8.f;
	MoveSpeed = 350.f;

	MonsterType = EMonsterType::Goblin;
	GemDropGrade = EGemGrade::Common;
	GoldDropAmount = 10;
	WeaknessGemType = EGemType::None;

	bIsDead = false;
	CurrentPhase = 1;
	CurrentZone = EArenaZone::Combat;
	LastHitGemType = EGemType::None;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}
}

void AProject_GemCoopMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHP = FMath::Clamp(CurrentHP, 0.0f, MaxHP);

	if (CurrentHP <= 0.0f)
	{
		CurrentHP = MaxHP;
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}

	if (auto* AIC = Cast<AAIController>(GetController()))
	{
		if (AIBehaviorTree)
		{
			AIC->RunBehaviorTree(AIBehaviorTree);
		}
	}

	for (auto* Ab : Abilities)
	{
		if (Ab)
		{
			Ab->OwnerMonster = this;
		}
	}
}

void AProject_GemCoopMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		TickAbilities(DeltaTime);
	}
}

void AProject_GemCoopMonsterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProject_GemCoopMonsterCharacter, MaxHP);
	DOREPLIFETIME(AProject_GemCoopMonsterCharacter, CurrentHP);
	DOREPLIFETIME(AProject_GemCoopMonsterCharacter, ATK);
	DOREPLIFETIME(AProject_GemCoopMonsterCharacter, MoveSpeed);
	DOREPLIFETIME(AProject_GemCoopMonsterCharacter, MonsterType);
	DOREPLIFETIME(AProject_GemCoopMonsterCharacter, bIsDead);
	DOREPLIFETIME(AProject_GemCoopMonsterCharacter, CurrentPhase);
}

void AProject_GemCoopMonsterCharacter::InitializeFromData(FMonsterData& Data)
{
	MonsterType = Data.MonsterType;
	MaxHP = Data.BaseHP;
	CurrentHP = MaxHP;
	ATK = Data.BaseATK;
	MoveSpeed = Data.MoveSpeed;
	WeaknessGemType = Data.WeaknessGem;
	GemDropGrade = Data.DropGrade;
	GoldDropAmount = Data.GoldDrop;

	bIsDead = false;
	CurrentPhase = 1;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}
}

bool AProject_GemCoopMonsterCharacter::IsWeakTo(EGemType GemType) const
{
	return (WeaknessGemType != EGemType::None) && (WeaknessGemType == GemType);
}

void AProject_GemCoopMonsterCharacter::TickAbilities(float DeltaTime)
{
	AActor* NearestTarget = nullptr;

	if (auto* AIC = Cast<AAIController>(GetController()))
	{
		if (auto* BB = AIC->GetBlackboardComponent())
		{
			NearestTarget = Cast<AActor>(BB->GetValueAsObject(FName("TargetActor")));
		}
	}

	for (auto* Ab : Abilities)
	{
		if (!Ab)
		{
			continue;
		}

		Ab->TickCooldown(DeltaTime);

		if (Ab->CanActivate() && NearestTarget)
		{
			Ab->ActivateAbility_Implementation(NearestTarget);
		}
	}
}

float AProject_GemCoopMonsterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bIsDead || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	float FinalDamage = DamageAmount;
	bool bIsWeak = false;

	if (LastHitGemType != EGemType::None && IsWeakTo(LastHitGemType))
	{
		FinalDamage *= 1.5f;
		bIsWeak = true;
	}

	CurrentHP = FMath::Clamp(CurrentHP - FinalDamage, 0.0f, MaxHP);

	OnMonsterHit.Broadcast(FinalDamage, bIsWeak);

	TryPhaseTransition();

	if (CurrentHP <= 0.0f)
	{
		OnDeath();
	}

	return FinalDamage;
}

void AProject_GemCoopMonsterCharacter::AttackTarget(AActor* Target)
{
	if (bIsDead || !Target)
	{
		return;
	}

	UGameplayStatics::ApplyDamage(Target, ATK, GetController(), this, nullptr);
}

void AProject_GemCoopMonsterCharacter::SetCurrentZone(EArenaZone NewZone)
{
	CurrentZone = NewZone;
}

EArenaZone AProject_GemCoopMonsterCharacter::GetCurrentZone() const
{
	return CurrentZone;
}

void AProject_GemCoopMonsterCharacter::TryPhaseTransition()
{
	if (MonsterType != EMonsterType::Boss && MonsterType != EMonsterType::MiniBoss)
	{
		return;
	}

	if (CurrentPhase >= 2)
	{
		return;
	}

	if (CurrentHP <= MaxHP * 0.5f)
	{
		CurrentPhase = 2;
		ATK *= 1.5f;
		MoveSpeed *= 1.3f;

		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
		}

		OnPhaseChanged.Broadcast(CurrentPhase);
	}
}

void AProject_GemCoopMonsterCharacter::DebugApplyMonsterDamage(float DamageAmount)
{
	UGameplayStatics::ApplyDamage(this, DamageAmount, nullptr, nullptr, nullptr);
}

void AProject_GemCoopMonsterCharacter::OnDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	CurrentHP = 0.0f;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnMonsterDied.Broadcast(this);

	SetLifeSpan(1.0f);
}



// Called to bind functionality to input
void AProject_GemCoopMonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

