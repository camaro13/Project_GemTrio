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

UCLASS()
class PROJECT_GEMCOOP_API AProject_GemCoopMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AProject_GemCoopMonsterCharacter();

	UPROPERTY(BlueprintAssignable)
	FOnMonsterDied OnMonsterDied;

	UPROPERTY(BlueprintAssignable)
	FOnMonsterHit OnMonsterHit;

	UPROPERTY(BlueprintAssignable)
	FOnPhaseChanged OnPhaseChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Monster")
	float MaxHP = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	float CurrentHP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float ATK = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float MoveSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	EMonsterType MonsterType = EMonsterType::Goblin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	EGemGrade GemDropGrade = EGemGrade::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	int32 GoldDropAmount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	EGemType WeaknessGemType = EGemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
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

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnDeath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
