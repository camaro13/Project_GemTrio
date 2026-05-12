// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopEnergySYComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnergyChanged, float, CurrentEnergy, float, MaxEnergy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnergyInsufficient, float, Required, float, Current);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUltGaugeChanged, float, GaugePercent);

class AProject_GemCoopGameStateBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_GEMCOOP_API UProject_GemCoopEnergySYComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProject_GemCoopEnergySYComponent();

	UPROPERTY(BlueprintAssignable)
	FOnEnergyChanged OnEnergyChanged;

	UPROPERTY(BlueprintAssignable)
	FOnEnergyInsufficient OnEnergyInsufficient;

	UPROPERTY(BlueprintAssignable)
	FOnUltGaugeChanged OnUltGaugeChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	float CachedSharedEnergy = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	float CachedMaxSharedEnergy = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	float EnergyRegenMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	float SafeZoneRegenBonus = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	bool bInSafeZone = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy|Ult")
	float UltGaugeContribution = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Energy|Ult")
	float UltGaugeChargeRate = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate")
	float MaxUltGauge = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy|Ult")
	bool bUltimateReady = false;

	UPROPERTY()
	AProject_GemCoopGameStateBase* GameStateRef = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Energy")
	bool TryConsumeEnergy(float Amount);

	UFUNCTION(BlueprintPure, Category = "Energy")
	bool HasEnoughEnergy(float Amount) const;

	UFUNCTION(BlueprintCallable, Category = "Energy")
	void SyncWithGameState();

	UFUNCTION(BlueprintCallable, Category = "Energy")
	void OnGameStateEnergyChanged(float CurrentEnergy, float MaxEnergy);

	UFUNCTION(BlueprintCallable, Category = "Energy")
	void OnEnergyChangedCallback(float NewEnergy);

	UFUNCTION(BlueprintCallable, Category = "Energy")
	void ApplySafeZoneBonus(bool bEntering);

	UFUNCTION(BlueprintCallable, Category = "Ultimate")
	void ChargeUltGauge(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Ultimate")
	void ConsumeUltGauge();

	UFUNCTION(BlueprintPure, Category = "Ultimate")
	float GetUltGaugePercent() const;

	UFUNCTION(BlueprintPure, Category = "Energy")
	float GetCurrentEnergy() const;

	UFUNCTION(BlueprintPure, Category = "Energy")
	float GetEnergyPercent() const;

	/*UFUNCTION(Server, Reliable)
	void ServerRPC_ConsumeEnergy(float Amount);*/

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
