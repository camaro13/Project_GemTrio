// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSharedEnergyChanged, float, Current, float, Max);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFormationStateChanged, bool, bActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSafeZoneChanged, bool, bActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveNumberChanged, int32, WaveNum);

class AProject_GemCoopCharacter;

/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API AProject_GemCoopGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AProject_GemCoopGameStateBase();

	UPROPERTY(BlueprintAssignable)
	FOnSharedEnergyChanged OnSharedEnergyChanged;

	UPROPERTY(BlueprintAssignable)
	FOnFormationStateChanged OnFormationStateChanged;

	UPROPERTY(BlueprintAssignable)
	FOnSafeZoneChanged OnSafeZoneChanged;

	UPROPERTY(BlueprintAssignable)
	FOnWaveNumberChanged OnWaveNumberChanged;

	UPROPERTY(ReplicatedUsing = OnRep_SharedEnergy, VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	float SharedEnergy = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_MaxSharedEnergy, VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	float MaxSharedEnergy = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
	float EnergyRegenRate = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
	float EnergyRegenMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
	float SafeZoneEnergyRegenMultiplier = 2.0f;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWaveNumber, VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveNumber = 0;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	float GameElapsedTime = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_SafeZoneActive, VisibleAnywhere, BlueprintReadOnly, Category = "SafeZone")
	bool SafeZoneActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SafeZone")
	float SafeZoneDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SafeZone")
	float SafeZoneCooldown = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone")
	float SafeZoneCycleTimer = 0.0f;

	UPROPERTY(ReplicatedUsing = OnRep_FormationActive, VisibleAnywhere, BlueprintReadOnly, Category = "Formation")
	bool bFormationActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formation")
	float FormationBonus_Defense = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formation")
	float FormationBonus_ATK = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	float FormationMinDistance = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	float FormationMaxDistance = 7000.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Fusion")
	FGemFusionPreview ActiveFusionPreview;

	UFUNCTION(BlueprintCallable, Category = "Energy")
	bool ConsumeEnergy(float Amount);

	UFUNCTION(BlueprintPure, Category = "Energy")
	bool HasEnoughEnergy(float Amount) const;

	UFUNCTION(BlueprintCallable, Category = "Energy")
	void AddEnergy(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Energy")
	void AddMaxEnergy(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Energy")
	void SetEnergyRegenMultiplier(float NewMultiplier);

	UFUNCTION(BlueprintCallable, Category = "GameState|Energy")
	void TickEnergyRegen(float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "GameState|Energy")
	float GetEnergyPercent() const;

	UFUNCTION(BlueprintCallable, Category = "GameState|Wave")
	void SetCurrentWaveNumber(int32 NewWaveNumber);

	UFUNCTION(BlueprintCallable, Category = "GameState|Zone")
	void SetSafeZoneActive(bool bNewActive);

	UFUNCTION(BlueprintCallable, Category = "GameState|Zone")
	void UpdateSafeZone(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "GameState|Formation")
	void SetFormationActive(bool bNewActive);

	UFUNCTION(BlueprintCallable, Category = "Formation")
	TArray<AProject_GemCoopCharacter*> GetPartyMembers() const;

	UFUNCTION(BlueprintPure, Category = "Formation")
	bool IsInFormationRange() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
	void OnRep_SharedEnergy();

	UFUNCTION()
	void OnRep_MaxSharedEnergy();

	UFUNCTION()
	void OnRep_FormationActive();

	UFUNCTION()
	void OnRep_SafeZoneActive();

	UFUNCTION()
	void OnRep_CurrentWaveNumber();

private:
	float GetCurrentEnergyRegenRate() const;
};
