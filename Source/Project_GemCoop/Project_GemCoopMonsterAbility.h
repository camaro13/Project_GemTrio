// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopMonsterAbility.generated.h"

class AProject_GemCoopMonsterCharacter;
/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API UProject_GemCoopMonsterAbility : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Cooldown = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CurrentCooldown = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AbilityPhase = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUnlocked = true;

	UPROPERTY()
	AProject_GemCoopMonsterCharacter* OwnerMonster = nullptr;

	/*UFUNCTION(BlueprintCallable)
	void ActivateAbility(AActor* Target);*/
	virtual void ActivateAbility_Implementation(AActor* Target);

	UFUNCTION(BlueprintPure)
	bool CanActivate() const;

	UFUNCTION(BlueprintCallable)
	void TickCooldown(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void OnAbilityEnd();
};

UCLASS()
class PROJECT_GEMCOOP_API UBossAbility_Charge : public UProject_GemCoopMonsterAbility
{
	GENERATED_BODY()
public:
	UBossAbility_Charge() { AbilityName = FName("BossCharge"); Cooldown = 8.f; AbilityPhase = 0; }
	virtual void ActivateAbility_Implementation(AActor* Target) override;
};

// ── 서브클래스 BossAbility_AoESlam ─────────────────────────
UCLASS()
class PROJECT_GEMCOOP_API UBossAbility_AoESlam : public UProject_GemCoopMonsterAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere) float SlamRadius = 600.f;
	UBossAbility_AoESlam() { AbilityName = FName("AoESlam"); Cooldown = 12.f; AbilityPhase = 0; }
	virtual void ActivateAbility_Implementation(AActor* Target) override;
};

// ── 서브클래스 BossAbility_Summon ───────────────────────────
UCLASS()
class PROJECT_GEMCOOP_API UBossAbility_Summon : public UProject_GemCoopMonsterAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere) TSubclassOf<AProject_GemCoopMonsterCharacter> SummonClass;
	UPROPERTY(EditAnywhere) int32 SummonCount = 3;
	UBossAbility_Summon() { AbilityName = FName("Summon"); Cooldown = 20.f; AbilityPhase = 1; }
	virtual void ActivateAbility_Implementation(AActor* Target) override;
};

// ── 서브클래스 BossAbility_RageAura ─────────────────────────
UCLASS()
class PROJECT_GEMCOOP_API UBossAbility_RageAura : public UProject_GemCoopMonsterAbility
{
	GENERATED_BODY()
public:
	UBossAbility_RageAura() { AbilityName = FName("RageAura"); Cooldown = 0.f; AbilityPhase = 2; bUnlocked = false; }
	virtual void ActivateAbility_Implementation(AActor* Target) override;
};
