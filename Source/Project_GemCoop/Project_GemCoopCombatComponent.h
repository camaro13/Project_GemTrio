// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopCombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCastingStarted, float, CastTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCastingCancelled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCastingCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCriticalHit, float, Damage, AActor*, Target);

class UProject_GemCoopStatComponent;
class UProject_GemCoopBuffComponent;

//struct FStatEvent
//{
//
//	float TimeStamp = 0.f;
//	float DamageVal = 0.f;
//	float HealVal = 0.f;
//	int32 BuffCount = 0;
//};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_GEMCOOP_API UProject_GemCoopCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProject_GemCoopCombatComponent();

	UPROPERTY(BlueprintAssignable)
	FOnCastingStarted OnCastingStarted;

	UPROPERTY(BlueprintAssignable)
	FOnCastingCancelled OnCastingCancelled;

	UPROPERTY(BlueprintAssignable)
	FOnCastingCompleted OnCastingCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnCriticalHit OnCriticalHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Total")
	float TotalDamageDealt = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Total")
	float TotalHealGiven = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Total")
	int32 TotalBuffsApplied = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Total")
	int32 TotalCritCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Total")
	int32 TotalKillCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|10s")
	float RecentDamage_10s = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|10s")
	float RecentHeal_10s = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|10s")
	int32 RecentBuffs_10s = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Casting")
	float LastAttackTime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Casting")
	bool bIsCasting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Casting")
	float CastingTimer = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Casting")
	float TotalCastTime = 0.5f;

	UPROPERTY()
	UProject_GemCoopStatComponent* StatCompRef = nullptr;

	UPROPERTY()
	UProject_GemCoopBuffComponent* BuffCompRef = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DealDamage(AActor* Target, float BaseDamage, EGemType DamageType);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RecordHeal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RecordBuff();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	FContributionRatio GetRecentContribution() const;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartCasting(float CastTime);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CancelCasting();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnKill(AActor* DeadActor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	TArray<FStatEvent> StatEventLog;

	void UpdateRecentStats(float DeltaTime);
	bool RollCritical();
	float CalculateFinalDamage(float Base, AActor* Target, EGemType Type);
	bool IsWeakness(AActor* Target, EGemType Type) const;
};
