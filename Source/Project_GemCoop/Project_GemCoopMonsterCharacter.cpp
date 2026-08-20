// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopMonsterCharacter.h"
#include "Project_GemCoopMonsterAbility.h"
#include "Project_GemCoopCharacter.h"
#include "Project_GemCoopGemComponent.h"
#include "Project_GemCoopStatComponent.h"
#include "Project_GemCoopObjectPSubsystem.h"
#include "AIController.h"
#include "Net/UnrealNetwork.h"
#include "Project_GemCoop.h"
#include "Engine/OverlapResult.h"
#include "Components/PrimitiveComponent.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Project_GemCoopMonsterHPWidget.h"
#include "Components/WidgetComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
//#include "Project_GemCoopMonsterAIControl.h"

// Sets default values
AProject_GemCoopMonsterCharacter::AProject_GemCoopMonsterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	NetUpdateFrequency = 30.f;
	MinNetUpdateFrequency = 15.f;

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

	//AIControllerClass = AProject_GemCoopMonsterAIControl::StaticClass();
	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	HealthBarWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 120.f));
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	HealthBarWidgetComponent->SetDrawAtDesiredSize(true);
	HealthBarWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	HealthBarWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthBarWidgetComponent->SetTickMode(ETickMode::Enabled);
	HealthBarWidgetComponent->SetAbsolute(false, true, false);
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
		GetCharacterMovement()->SetIsReplicated(true);
	}

	for (auto* Ab : Abilities)
	{
		if (Ab)
		{
			Ab->OwnerMonster = this;
		}
	}

	if (HasAuthority())
	{
		if (!GetController())
		{
			SpawnDefaultController();
		}

		/*if (AAIController* AIC = Cast<AAIController>(GetController()))
		{
			if (AIBehaviorTree)
			{
				AIC->RunBehaviorTree(AIBehaviorTree);
			}
		}*/

		RetargetTimer = RetargetInterval;
		MoveRefreshTimer = MoveRefreshInterval;
		AttackCooldownTimer = 0.0f;

		bIsAttacking = false;
		bAttackDamageApplied = false;
		AttackElapsedTime = 0.0f;

		SetCurrentTarget_Server(SelectNearestPlayerTarget());
	}

	/*for (UProject_GemCoopMonsterAbility* Ability : Abilities)
	{
		if (Ability)
		{
			Ability->OwnerMonster = this;
		}
	}*/

	InitializeHealthBarWidget();
	RefreshHealthBar();
}

void AProject_GemCoopMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHealthBarFacingCamera();

	if (!HasAuthority())
	{
		return;
	}

	if (CurrentHP <= 0.0f)
	{
		return;
	}

	ServerUpdateTargeting(DeltaTime);

	if (bIsAttacking)
	{
		ServerUpdateAttackState(DeltaTime);
		return;
	}

	ServerUpdateMovement(DeltaTime);
	ServerUpdateAttack(DeltaTime);

	TickAbilities(DeltaTime);
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
	DOREPLIFETIME(AProject_GemCoopMonsterCharacter, CurrentTarget);
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

	bIsAttacking = false;
	bAttackDamageApplied = false;
	AttackElapsedTime = 0.0f;
	AttackCooldownTimer = 0.0f;

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->SetMovementMode(MOVE_Walking);
		MovementComp->MaxWalkSpeed = MoveSpeed;
	}

	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetHiddenInGame(false, true);
		HealthBarWidgetComponent->SetVisibility(true, true);
	}

	RefreshHealthBar();

	if (HasAuthority())
	{
		ForceNetUpdate();

		if (!GetController())
		{
			SpawnDefaultController();
		}

		CurrentTarget = nullptr;

		RetargetTimer = RetargetInterval;
		MoveRefreshTimer = MoveRefreshInterval;
		AttackCooldownTimer = 0.0f;

		bIsAttacking = false;
		bAttackDamageApplied = false;
		AttackElapsedTime = 0.0f;

		SetCurrentTarget_Server(SelectNearestPlayerTarget());
	}
}

bool AProject_GemCoopMonsterCharacter::IsWeakTo(EGemType GemType) const
{
	return (WeaknessGemType != EGemType::None) && (WeaknessGemType == GemType);
}

void AProject_GemCoopMonsterCharacter::TickAbilities(float DeltaTime)
{
	/*AActor* NearestTarget = nullptr;

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
	}*/

	AActor* AbilityTarget = CurrentTarget;

	for (auto* Ab : Abilities)
	{
		if (!Ab)
		{
			continue;
		}

		Ab->TickCooldown(DeltaTime);

		if (Ab->CanActivate() && IsValidPlayerTarget(AbilityTarget))
		{
			Ab->ActivateAbility_Implementation(AbilityTarget);
		}
	}
}

float AProject_GemCoopMonsterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority())
	{
		return 0.0f;
	}

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bIsDead || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	float FinalDamage = DamageAmount;
	bool bIsWeak = false;

	if (AProject_GemCoopCharacter* DamageCauserCharacter = Cast<AProject_GemCoopCharacter>(DamageCauser))
	{
		if (DamageCauserCharacter->GemCompRef)
		{
			LastHitGemType = DamageCauserCharacter->GemCompRef->LastUsedGemType;
			bIsWeak = IsWeakTo(LastHitGemType);

			if (bIsWeak)
			{
				FinalDamage *= 1.5f;
			}
		}
	}

	CurrentHP = FMath::Clamp(CurrentHP - FinalDamage, 0.0f, MaxHP);

	RefreshHealthBar();
	ForceNetUpdate();

	OnMonsterHit.Broadcast(FinalDamage, bIsWeak);

	TryPhaseTransition();

	if (CurrentHP <= 0.0f)
	{
		OnDeath();
	}

	return FinalDamage;
}

//void AProject_GemCoopMonsterCharacter::AttackTarget(AActor* Target)
//{
//	if (bIsDead || !Target)
//	{
//		return;
//	}
//
//	UGameplayStatics::ApplyDamage(Target, ATK, GetController(), this, nullptr);
//
//	UE_LOG(LogTemp, Warning, TEXT("Monster AttackTarget: %s -> %s Damage %.1f"),
//		*GetNameSafe(this),
//		*GetNameSafe(Target),
//		ATK
//	);
//}

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
	if (!HasAuthority())
	{
		return;
	}
	
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	CurrentHP = 0.0f;
	CurrentTarget = nullptr;

	bIsAttacking = false;
	bAttackDamageApplied = false;
	AttackElapsedTime = 0.0f;
	AttackCooldownTimer = 0.0f;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->StopMovement();
		AIC->ClearFocus(EAIFocusPriority::Gameplay);
	}

	ApplyDeathState();

	OnMonsterDied.Broadcast(this);

	ForceNetUpdate();

	SetLifeSpan(DeathDestroyDelay);
}

// Called to bind functionality to input
void AProject_GemCoopMonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//void AProject_GemCoopMonsterCharacter::ServerUpdateTargeting(float DeltaTime)
//{
//	if (!HasAuthority())
//	{
//		return;
//	}
//
//	RetargetTimer -= DeltaTime;
//
//	const bool bTargetInvalid = !IsValidPlayerTarget(CurrentTarget);
//
//	if (!bTargetInvalid && RetargetTimer > 0.0f)
//	{
//		return;
//	}
//
//	RetargetTimer = RetargetInterval;
//
//	AActor* NewTarget = SelectNearestPlayerTarget();
//
//	SetCurrentTarget_Server(NewTarget);
//
//	MoveRefreshTimer = 0.0f;
//}

AActor* AProject_GemCoopMonsterCharacter::SelectNearestPlayerTarget() const
{
	if (!GetWorld())
	{
		return nullptr;
	}

	AProject_GemCoopCharacter* NearestPlayer = nullptr;
	float BestDistanceSq = TNumericLimits<float>::Max();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();

		if (!PC)
		{
			continue;
		}

		AProject_GemCoopCharacter* PlayerCharacter = Cast<AProject_GemCoopCharacter>(PC->GetPawn());

		if (!IsValidPlayerTarget(PlayerCharacter))
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(GetActorLocation(), PlayerCharacter->GetActorLocation());

		if (DistanceSq < BestDistanceSq)
		{
			BestDistanceSq = DistanceSq;
			NearestPlayer = PlayerCharacter;
		}
	}

	return NearestPlayer;
}

bool AProject_GemCoopMonsterCharacter::IsValidPlayerTarget(AActor* Target) const
{
	if (!Target)
	{
		return false;
	}

	if (Target->IsPendingKillPending())
	{
		return false;
	}

	AProject_GemCoopCharacter* PlayerCharacter = Cast<AProject_GemCoopCharacter>(Target);

	if (!PlayerCharacter)
	{
		return false;
	}

	UProject_GemCoopStatComponent* TargetStatComp = PlayerCharacter->FindComponentByClass<UProject_GemCoopStatComponent>();

	if (!TargetStatComp)
	{
		return false;
	}

	if (TargetStatComp->IsDead())
	{
		return false;
	}

	return true;
}

void AProject_GemCoopMonsterCharacter::SetCurrentTarget_Server(AActor* NewTarget)
{
	/*if (!HasAuthority())
	{
		return;
	}

	CurrentTarget = NewTarget;

	UpdateBlackboardTarget();

	MoveRefreshTimer = 0.0f;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->StopMovement();
	}

	MoveToCurrentTarget_Server(true);

	ForceNetUpdate();*/

	if (!HasAuthority())
	{
		return;
	}

	CurrentTarget = NewTarget;

	MoveRefreshTimer = 0.0f;

	AAIController* AIC = Cast<AAIController>(GetController());

	if (!AIC)
	{
		SpawnDefaultController();
		AIC = Cast<AAIController>(GetController());
	}

	if (AIC)
	{
		AIC->ClearFocus(EAIFocusPriority::Gameplay);
		AIC->StopMovement();

		if (CurrentTarget)
		{
			AIC->SetFocus(CurrentTarget, EAIFocusPriority::Gameplay);
		}
	}

	MoveToCurrentTarget_Server(true);

	ForceNetUpdate();
}

void AProject_GemCoopMonsterCharacter::OnRep_CurrentTarget()
{

}

void AProject_GemCoopMonsterCharacter::OnRep_Health()
{
	RefreshHealthBar();
	
	UE_LOG(
		LogTemp,
		Verbose,
		TEXT("Monster OnRep_Health. Monster=%s HP=%.1f / %.1f"),
		*GetNameSafe(this),
		CurrentHP,
		MaxHP
	);
}

void AProject_GemCoopMonsterCharacter::OnRep_IsDead()
{
	if (bIsDead)
	{
		ApplyDeathState();
	}
}

void AProject_GemCoopMonsterCharacter::ApplyDeathState()
{
	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(false, true);
		HealthBarWidgetComponent->SetHiddenInGame(true, true);
	}

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
		MovementComp->DisableMovement();
	}

	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (GetNetMode() != NM_DedicatedServer && DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
}

void AProject_GemCoopMonsterCharacter::InitializeHealthBarWidget()
{
	if (!HealthBarWidgetComponent)
	{
		return;
	}

	HealthBarWidgetComponent->InitWidget();

	UProject_GemCoopMonsterHPWidget* HealthBarWidget = Cast<UProject_GemCoopMonsterHPWidget>(HealthBarWidgetComponent->GetUserWidgetObject());

	if (!HealthBarWidget)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Monster HealthBar widget missing or wrong class. Monster=%s"),
			*GetNameSafe(this)
		);

		return;
	}

	HealthBarWidget->SetOwningMonster(this);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Monster HealthBar widget Good. Monster=%s"),
		*GetNameSafe(this)
	);
}

void AProject_GemCoopMonsterCharacter::RefreshHealthBar()
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Monster HealthBar widget Refresh Good. Monster=%s"),
		*GetNameSafe(this)
	);

	if (!HealthBarWidgetComponent)
	{
		return;
	}

	UProject_GemCoopMonsterHPWidget* HealthBarWidget = Cast<UProject_GemCoopMonsterHPWidget>(HealthBarWidgetComponent->GetUserWidgetObject());

	if (!HealthBarWidget)
	{
		InitializeHealthBarWidget();

		HealthBarWidget = Cast<UProject_GemCoopMonsterHPWidget>(HealthBarWidgetComponent->GetUserWidgetObject());
	}

	if (!HealthBarWidget)
	{
		return;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Monster HealthBar widgetUPdateHealth Good. Monster=%s"),
		*GetNameSafe(this)
	);

	HealthBarWidget->UpdateHealth(CurrentHP, MaxHP);

	const bool bShouldShow = !bIsDead && CurrentHP > 0.0f && !IsHidden();

	HealthBarWidgetComponent->SetVisibility(bShouldShow, true);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Monster HealthBar widget visibility Good. Visibility=%d"),
		bShouldShow
	);
}

void AProject_GemCoopMonsterCharacter::UpdateHealthBarFacingCamera()
{
	if (!HealthBarWidgetComponent)
	{
		return;
	}

	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	APlayerCameraManager* LocalCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	if (!LocalCameraManager)
	{
		return;
	}

	const FVector WidgetLocation = HealthBarWidgetComponent->GetComponentLocation();

	const FVector CameraLocation = LocalCameraManager->GetCameraLocation();

	const FVector DirectionToCamera = CameraLocation - WidgetLocation;

	if (DirectionToCamera.IsNearlyZero())
	{
		return;
	}

	FRotator TargetRotation = DirectionToCamera.Rotation();

	TargetRotation.Roll = 0.0f;

	TargetRotation += HealthBarFacingRotationOffset;

	HealthBarWidgetComponent->SetWorldRotation(TargetRotation);
}

void AProject_GemCoopMonsterCharacter::UpdateBlackboardTarget()
{
	/*AAIController* AIC = Cast<AAIController>(GetController());

	if (!AIC)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = AIC->GetBlackboardComponent();

	if (!BlackboardComp)
	{
		return;
	}

	BlackboardComp->SetValueAsObject(TargetBlackboardKeyName, CurrentTarget);*/
}

void AProject_GemCoopMonsterCharacter::ServerUpdateMovement(float DeltaTime)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bIsAttacking)
	{
		return;
	}

	MoveRefreshTimer += DeltaTime;

	if (MoveRefreshTimer < MoveRefreshInterval)
	{
		return;
	}

	MoveRefreshTimer = 0.0f;

	MoveToCurrentTarget_Server(false);
}

void AProject_GemCoopMonsterCharacter::ServerUpdateAttack(float DeltaTime)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bIsAttacking)
	{
		return;
	}

	AttackCooldownTimer = FMath::Max(0.0f, AttackCooldownTimer - DeltaTime);

	if (AttackCooldownTimer > 0.0f)
	{
		return;
	}

	if (!IsValidPlayerTarget(CurrentTarget))
	{
		return;
	}

	const float DistanceToTarget =
		FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());

	if (DistanceToTarget > AttackStartRange)
	{
		return;
	}

	StartAttack_ServerOnly();
}

void AProject_GemCoopMonsterCharacter::ServerUpdateAttackState(float DeltaTime)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!bIsAttacking)
	{
		return;
	}

	AttackElapsedTime += DeltaTime;

	const float SafeDamageTime = FMath::Max(0.05f, AttackDamageTime);
	const float SafeAttackDuration = FMath::Max(AttackDuration, SafeDamageTime + 0.1f);

	if (!bAttackDamageApplied && AttackElapsedTime >= SafeDamageTime)
	{
		bAttackDamageApplied = true;
		ApplyAttackAreaDamage_ServerOnly();
	}

	if (AttackElapsedTime >= SafeAttackDuration)
	{
		FinishAttack_ServerOnly();
	}
}

void AProject_GemCoopMonsterCharacter::StartAttack_ServerOnly()
{
	if (!HasAuthority())
	{
		return;
	}

	if (bIsAttacking)
	{
		return;
	}

	bIsAttacking = true;
	bAttackDamageApplied = false;
	AttackElapsedTime = 0.0f;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->StopMovement();
	}

	if (CurrentTarget)
	{
		FVector Direction = CurrentTarget->GetActorLocation() - GetActorLocation();
		Direction.Z = 0.0f;

		if (!Direction.IsNearlyZero())
		{
			SetActorRotation(Direction.Rotation());
		}
	}

	MulticastPlayAttackMontage();
}

void AProject_GemCoopMonsterCharacter::FinishAttack_ServerOnly()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsAttacking = false;
	bAttackDamageApplied = false;
	AttackElapsedTime = 0.0f;

	AttackCooldownTimer = AttackCooldown;

	MoveRefreshTimer = 0.0f;
}

void AProject_GemCoopMonsterCharacter::ApplyAttackAreaDamage_ServerOnly()
{
	if (!HasAuthority())
	{
		return;
	}

	if (CurrentHP <= 0.0f)
	{
		return;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	const FVector AttackCenter = GetActorLocation() + GetActorForwardVector() * AttackForwardOffset;

	TArray<FOverlapResult> OverlapResults;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = false;

	const FCollisionShape AttackShape = FCollisionShape::MakeSphere(AttackHitRadius);

	const bool bHit = World->OverlapMultiByChannel(OverlapResults, AttackCenter, FQuat::Identity, ECC_Pawn, AttackShape, QueryParams);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	DrawDebugSphere(
		World,
		AttackCenter,
		AttackHitRadius,
		16,
		FColor::Red,
		false,
		1.0f
	);
#endif

	if (!bHit)
	{
		return;
	}

	TSet<AActor*> DamagedActors;

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		UPrimitiveComponent* HitComponent = OverlapResult.Component.Get();

		if (!HitComponent)
		{
			continue;
		}

		AActor* HitActor = HitComponent->GetOwner();

		if (!HitActor || DamagedActors.Contains(HitActor))
		{
			continue;
		}

		AProject_GemCoopCharacter* HitPlayer = Cast<AProject_GemCoopCharacter>(HitActor);

		if (!HitPlayer)
		{
			continue;
		}

		if (!IsValidPlayerTarget(HitPlayer))
		{
			continue;
		}

		if (bUseAttackConeCheck)
		{
			FVector ToTarget2D = HitPlayer->GetActorLocation() - GetActorLocation();
			ToTarget2D.Z = 0.0f;

			if (ToTarget2D.IsNearlyZero())
			{
				continue;
			}

			ToTarget2D.Normalize();

			FVector Forward2D = GetActorForwardVector();
			Forward2D.Z = 0.0f;

			if (!Forward2D.IsNearlyZero())
			{
				Forward2D.Normalize();
			}

			const float Dot = FVector::DotProduct(Forward2D, ToTarget2D);
			const float MinDot = FMath::Cos(FMath::DegreesToRadians(AttackHalfAngle));

			if (Dot < MinDot)
			{
				continue;
			}
		}

		UGameplayStatics::ApplyDamage(
			HitPlayer,
			ATK,
			GetController(),
			this,
			UDamageType::StaticClass()
		);

		DamagedActors.Add(HitActor);
	}
}

void AProject_GemCoopMonsterCharacter::AnimNotify_AttackHit() //공격 몽타주에서 타격 프레임에 AnimNotify를 하나 만들고, 그 노티파이에서 몬스터의 이 함수를 호출
{
	if (!HasAuthority())
	{
		return;
	}

	if (!bIsAttacking)
	{
		return;
	}

	if (bAttackDamageApplied)
	{
		return;
	}

	bAttackDamageApplied = true;
	ApplyAttackAreaDamage_ServerOnly();

	// AnimNotify만 쓸 거면 StartAttack_ServerOnly 안의 이 부분 삭제
	//GetWorldTimerManager().SetTimer(
	//	AttackHitTimerHandle,
	//	this,
	//	&AProject_GemCoopMonsterCharacter::ApplyAttackAreaDamage_ServerOnly,
	//	AttackDamageDelay,
	//	false
	//);
}

void AProject_GemCoopMonsterCharacter::AttackTarget(AActor* Target)
{
	if (!HasAuthority())
	{
		return;
	}

	StartAttack_ServerOnly();
}

void AProject_GemCoopMonsterCharacter::MulticastPlayAttackMontage_Implementation()
{
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}

void AProject_GemCoopMonsterCharacter::ServerUpdateTargeting(float DeltaTime)
{
	if (!HasAuthority())
	{
		return;
	}

	RetargetTimer += DeltaTime;

	const bool bTargetInvalid = !IsValidPlayerTarget(CurrentTarget);

	if (!bTargetInvalid && RetargetTimer < RetargetInterval)
	{
		return;
	}

	RetargetTimer = 0.0f;

	AActor* NewTarget = SelectNearestPlayerTarget();

	if (NewTarget != CurrentTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster Retarget. Monster=%s Old=%s New=%s"),
			*GetNameSafe(this),
			*GetNameSafe(CurrentTarget),
			*GetNameSafe(NewTarget)
		);
		SetCurrentTarget_Server(NewTarget);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster Retarget Checked. Monster=%s Target Still=%s"),
			*GetNameSafe(this),
			*GetNameSafe(CurrentTarget)
		);
		MoveToCurrentTarget_Server(true);
	}
}

void AProject_GemCoopMonsterCharacter::MoveToCurrentTarget_Server(bool bForceMove)
{
	/*if (!HasAuthority())
	{
		return;
	}

	if (bIsAttacking)
	{
		return;
	}

	if (!IsValidPlayerTarget(CurrentTarget))
	{
		return;
	}

	AAIController* AIC = Cast<AAIController>(GetController());

	if (!AIC)
	{
		SpawnDefaultController();
		AIC = Cast<AAIController>(GetController());
	}

	if (!AIC)
	{
		return;
	}

	const float DistanceToTarget = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());

	if (DistanceToTarget > AttackStartRange * 0.85f)
	{
		const EPathFollowingRequestResult::Type MoveResult = AIC->MoveToActor(CurrentTarget, AttackStartRange * 0.75f, true, true, true);
	}
	else
	{
		AIC->StopMovement();
	}*/

	if (!HasAuthority())
	{
		return;
	}

	if (bIsAttacking)
	{
		return;
	}

	if (!IsValidPlayerTarget(CurrentTarget))
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveTo failed. Invalid CurrentTarget. Monster=%s Target=%s"),
			*GetNameSafe(this),
			*GetNameSafe(CurrentTarget)
		);
		return;
	}

	AAIController* AIC = Cast<AAIController>(GetController());

	if (!AIC)
	{
		SpawnDefaultController();
		AIC = Cast<AAIController>(GetController());
	}

	if (!AIC)
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveTo failed. No AIController. Monster=%s"),
			*GetNameSafe(this)
		);
		return;
	}

	const float DistanceToTarget =
		FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());

	if (DistanceToTarget > AttackStartRange * 0.85f)
	{
		const EPathFollowingRequestResult::Type MoveResult =
			AIC->MoveToActor(
				CurrentTarget,
				AttackStartRange * 0.75f,
				true,
				true,
				true
			);

		UE_LOG(LogTemp, Warning, TEXT("Monster MoveTo. Monster=%s Target=%s Distance=%.1f Result=%d Controller=%s"),
			*GetNameSafe(this),
			*GetNameSafe(CurrentTarget),
			DistanceToTarget,
			static_cast<int32>(MoveResult),
			*GetNameSafe(AIC)
		);
	}
	else
	{
		AIC->StopMovement();

		UE_LOG(LogTemp, Warning, TEXT("Monster NearTarget Stop. Monster=%s Target=%s Distance=%.1f"),
			*GetNameSafe(this),
			*GetNameSafe(CurrentTarget),
			DistanceToTarget
		);
	}
}