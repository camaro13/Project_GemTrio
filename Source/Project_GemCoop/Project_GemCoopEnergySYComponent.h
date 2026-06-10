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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable)
	FOnEnergyChanged OnEnergyChanged;

	UPROPERTY(BlueprintAssignable)
	FOnEnergyInsufficient OnEnergyInsufficient;

	UPROPERTY(BlueprintAssignable)
	FOnUltGaugeChanged OnUltGaugeChanged;

	UPROPERTY(ReplicatedUsing = OnRep_Energy, VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	float CachedEnergy = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Energy, VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	float CachedMaxEnergy = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	float EnergyRegenMultiplier = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	float SafeZoneRegenBonus = 2.f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	bool bInSafeZone = false;

	UPROPERTY(ReplicatedUsing = OnRep_UltGauge, VisibleAnywhere, BlueprintReadOnly, Category = "Energy|Ult")
	float UltGaugeContribution = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Energy|Ult")
	float UltGaugeChargeRate = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate")
	float MaxUltGauge = 100.0f;

	UPROPERTY(ReplicatedUsing = OnRep_UltGauge, VisibleAnywhere, BlueprintReadOnly, Category = "Energy|Ult")
	bool bUltimateReady = false;

	UPROPERTY()
	AProject_GemCoopGameStateBase* GameStateRef = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Energy")
	bool TryConsumeEnergy(float Amount);

	UFUNCTION(BlueprintPure, Category = "Energy")
	bool HasEnoughEnergy(float Amount) const;

	UFUNCTION(BlueprintCallable, Category = "Energy")
	void SyncWithUI();

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
	float GetMaxEnergy() const;

	UFUNCTION(BlueprintPure, Category = "Energy")
	float GetEnergyPercent() const;

	/*UFUNCTION(Server, Reliable)
	void ServerRPC_ConsumeEnergy(float Amount);*/

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_Energy();

	UFUNCTION()
	void OnRep_UltGauge();

	void SetEnergy_Server(float NewEnergy);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
