// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopMonsterAIControl.h"
#include "Project_GemCoopCharacter.h"
#include "Project_GemCoopMonsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"

AProject_GemCoopMonsterAIControl::AProject_GemCoopMonsterAIControl()
{
	PrimaryActorTick.bCanEverTick = true;

	SearchRadius = 5000.f;
	AttackRange = 180.f;
	AttackCooldown = 1.2f;
	AttackCooldownTimer = 0.0f;

	CurrentTarget = nullptr;
	ControlledMonster = nullptr;
}

void AProject_GemCoopMonsterAIControl::BeginPlay()
{
	Super::BeginPlay();
}

void AProject_GemCoopMonsterAIControl::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledMonster = Cast<AProject_GemCoopMonsterCharacter>(InPawn);

	if (!ControlledMonster)
	{
		UE_LOG(LogTemp, Warning, TEXT("MonsterAIController: Possessed pawn is not MonsterCharacter."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("MonsterAIController possessed: %s"), *GetNameSafe(ControlledMonster));
}

void AProject_GemCoopMonsterAIControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ControlledMonster || ControlledMonster->bIsDead)
	{
		StopMovement();
		return;
	}

	if (AttackCooldownTimer > 0.0f)
	{
		AttackCooldownTimer = FMath::Max(0.0f, AttackCooldownTimer - DeltaTime);
	}

	UpdateTarget();

	if (!CurrentTarget)
	{
		StopMovement();
		return;
	}

	if (IsTargetInAttackRange())
	{
		StopMovement();
		TryAttackCurrentTarget();
	}
	else
	{
		MoveToCurrentTarget();
	}
}

AProject_GemCoopCharacter* AProject_GemCoopMonsterAIControl::FindNearestPlayer() const
{
	if (!ControlledMonster)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		return nullptr;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, AProject_GemCoopCharacter::StaticClass(), FoundActors);

	AProject_GemCoopCharacter* NearestPlayer = nullptr;
	float NearestDistanceSq = SearchRadius * SearchRadius;

	for (AActor* Actor : FoundActors)
	{
		AProject_GemCoopCharacter* PlayerCharacter = Cast<AProject_GemCoopCharacter>(Actor);

		if (!PlayerCharacter)
		{
			continue;
		}

		if (!PlayerCharacter->bIsAlive)
		{
			continue;
		}

		float DistanceSq = FVector::DistSquared(ControlledMonster->GetActorLocation(), PlayerCharacter->GetActorLocation());

		if (DistanceSq < NearestDistanceSq)
		{
			NearestDistanceSq = DistanceSq;
			NearestPlayer = PlayerCharacter;
		}
	}

	return NearestPlayer;
}

void AProject_GemCoopMonsterAIControl::UpdateTarget()
{
	if (CurrentTarget && CurrentTarget->bIsAlive)
	{
		float Distance = FVector::Dist(ControlledMonster->GetActorLocation(), CurrentTarget->GetActorLocation());

		if (Distance <= SearchRadius)
		{
			return;
		}
	}

	CurrentTarget = FindNearestPlayer();
}

void AProject_GemCoopMonsterAIControl::MoveToCurrentTarget()
{
	if (!CurrentTarget || !ControlledMonster)
	{
		return;
	}

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(CurrentTarget);
	MoveRequest.SetAcceptanceRadius(AttackRange * 0.8f);
	MoveRequest.SetUsePathfinding(true);
	MoveRequest.SetAllowPartialPath(true);

	FNavPathSharedPtr NavPath;
	MoveTo(MoveRequest, &NavPath);
}

bool AProject_GemCoopMonsterAIControl::IsTargetInAttackRange() const
{
	if (!CurrentTarget || !ControlledMonster)
	{
		return false;
	}

	float Distance = FVector::Dist(ControlledMonster->GetActorLocation(), CurrentTarget->GetActorLocation());

	return Distance <= AttackRange;
}

void AProject_GemCoopMonsterAIControl::TryAttackCurrentTarget()
{
	if (!CurrentTarget || !ControlledMonster)
	{
		return;
	}

	if (AttackCooldownTimer > 0.0f)
	{
		return;
	}

	ControlledMonster->AttackTarget(CurrentTarget);

	AttackCooldownTimer = AttackCooldown;

	UE_LOG(LogTemp, Warning, TEXT("Monster AI Attack: %s -> %s"),
		*GetNameSafe(ControlledMonster),
		*GetNameSafe(CurrentTarget)
	);
}