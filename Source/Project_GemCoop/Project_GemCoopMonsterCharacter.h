// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopMonsterCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDied, AProject_GemCoopMonsterCharacter*, Monster);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMonsterHit, float, Damage, bool, bIsWeak);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, int32, NewPhase);

class UProject_GemCoopMonsterAbility;
class AProject_GemCoopCharacter;
class UWidgetComponent;
class UProject_GemCoopMonsterHPWidget;
class UAnimMontage;

UCLASS()
class PROJECT_GEMCOOP_API AProject_GemCoopMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AProject_GemCoopMonsterCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable)
	FOnMonsterDied OnMonsterDied;

	UPROPERTY(BlueprintAssignable)
	FOnMonsterHit OnMonsterHit;

	UPROPERTY(BlueprintAssignable)
	FOnPhaseChanged OnPhaseChanged;

	UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, BlueprintReadWrite, Category="Monster")
	float MaxHP = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	float CurrentHP = 100.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float ATK = 10.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float MoveSpeed = 300.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Monster")
	EMonsterType MonsterType = EMonsterType::Goblin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	EGemGrade GemDropGrade = EGemGrade::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	int32 GoldDropAmount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	EGemType WeaknessGemType = EGemType::None;

	UPROPERTY(ReplicatedUsing = OnRep_IsDead, EditAnywhere, BlueprintReadWrite, Category = "Monster")
	bool bIsDead = false;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	int32 CurrentPhase = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	EArenaZone CurrentZone = EArenaZone::Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	EGemType LastHitGemType = EGemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	UBehaviorTree* AIBehaviorTree = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	UAnimMontage* AttackMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	TArray<UProject_GemCoopMonsterAbility*> Abilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|UI")
	UWidgetComponent* HealthBarWidgetComponent = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentTarget, VisibleAnywhere, BlueprintReadOnly, Category = "Monster|AI")
	AActor* CurrentTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|AI")
	float RetargetInterval = 5.0f;//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|AI")
	float RetargetRandomDeviation = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|AI")
	float TargetSwitchChance = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|AI")
	float MoveRefreshInterval = 0.25f;//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat")
	float AttackStartRange = 180.0f;//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat")
	float AttackDuration = 0.8f;//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat")
	float AttackCooldown = 1.5f;//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat")
	float AttackDamageTime = 0.35f;//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat")
	float AttackHitRadius = 180.0f;//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat")
	float AttackForwardOffset = 90.0f;//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat")
	float AttackHalfAngle = 90.0f;//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat")
	bool bUseAttackConeCheck = true;//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|AI")
	FName TargetBlackboardKeyName = TEXT("TargetActor");//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Death")
	UAnimMontage* DeathMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Death", meta = (ClampMin = "0.1"))
	float DeathDestroyDelay = 2.0f;

public:
	UFUNCTION(BlueprintCallable)
	void InitializeFromData(FMonsterData& Data);

	UFUNCTION(BlueprintCallable)
	void AttackTarget(AActor* Target);

	UFUNCTION(BlueprintPure)
	bool IsWeakTo(EGemType GemType) const;

	UFUNCTION(BlueprintCallable)
	void SetCurrentZone(EArenaZone NewZone);

	UFUNCTION(BlueprintPure)
	EArenaZone GetCurrentZone() const;

	UFUNCTION(BlueprintCallable)
	void TryPhaseTransition();

	UFUNCTION(BlueprintCallable)
	void DebugApplyMonsterDamage(float DamageAmount = 10.f);

	UFUNCTION(BlueprintCallable)
	void TickAbilities(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	void AnimNotify_AttackHit();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void OnDeath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_CurrentTarget();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayAttackMontage();

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_IsDead();

	void ApplyDeathState();

	void InitializeHealthBarWidget();
	void RefreshHealthBar();

	void UpdateHealthBarFacingCamera();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster")
	FRotator HealthBarFacingRotationOffset = FRotator(0.0f, 0.0f, 0.0f);

private:
	void ServerUpdateTargeting(float DeltaTime);
	void ServerUpdateMovement(float DeltaTime);
	void ServerUpdateAttack(float DeltaTime);
	void ServerUpdateAttackState(float DeltaTime);

	AActor* SelectNearestPlayerTarget() const;
	bool IsValidPlayerTarget(AActor* Target) const;

	void SetCurrentTarget_Server(AActor* NewTarget);
	void UpdateBlackboardTarget();

	void StartAttack_ServerOnly();
	void ApplyAttackAreaDamage_ServerOnly();
	void FinishAttack_ServerOnly();

	void MoveToCurrentTarget_Server(bool bForceMove);

	float RetargetTimer = 0.0f;
	float MoveRefreshTimer = 0.0f;
	float AttackCooldownTimer = 0.0f;

	bool bIsAttacking = false;
	bool bAttackDamageApplied = false;
	float AttackElapsedTime = 0.0f;

	FTimerHandle AttackHitTimerHandle;
	FTimerHandle AttackEndTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
