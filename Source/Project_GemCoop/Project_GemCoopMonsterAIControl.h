// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Project_GemCoopMonsterAIControl.generated.h"

class AProject_GemCoopCharacter;
class AProject_GemCoopMonsterCharacter;

/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API AProject_GemCoopMonsterAIControl : public AAIController
{
	GENERATED_BODY()
	
public:
	AProject_GemCoopMonsterAIControl();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	float SearchRadius = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackRange = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackCooldown = 1.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	float AttackCooldownTimer = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	AProject_GemCoopCharacter* CurrentTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Owner")
	AProject_GemCoopMonsterCharacter* ControlledMonster = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	AProject_GemCoopCharacter* FindNearestPlayer() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void UpdateTarget();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void MoveToCurrentTarget();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void TryAttackCurrentTarget();

	UFUNCTION(BlueprintPure, Category = "AI")
	bool IsTargetInAttackRange() const;
};
