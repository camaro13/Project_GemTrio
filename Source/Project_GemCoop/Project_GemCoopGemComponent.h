// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopGemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGemUsed, int32, SlotIndex, FGemData, GemData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoolDownChanged, int32, SlotIndex, float, Remaining);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFusionAttempt, int32, SlotIndex, FGemData, GemData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGemExchanged, int32, MySlot, int32, PartnerSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotRefilled, int32, SlotIndex, FGemData, NewGem);

class UProject_GemCoopCombatComponent;
class UProject_GemCoopEnergySYComponent;
class UProject_GemCoopFusionSYComponent;
class UProject_GemCoopGemDataSubsystem;
class UProject_GemCoopStatComponent;
class UProject_GemCoopBuffComponent;
class AProject_GemCoopCharacter;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_GEMCOOP_API UProject_GemCoopGemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProject_GemCoopGemComponent();

	UPROPERTY(BlueprintAssignable)
	FOnGemUsed OnGemUsed;

	UPROPERTY(BlueprintAssignable)
	FOnCoolDownChanged OnCoolDownChanged;

	UPROPERTY(BlueprintAssignable)
	FOnFusionAttempt OnFusionAttempt;
	
	UPROPERTY(BlueprintAssignable)
	FOnGemExchanged OnGemExchanged;

	UPROPERTY(BlueprintAssignable)
	FOnSlotRefilled OnSlotRefilled;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gem")
	TArray<FGemData> GemSlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gem")
	TArray<float> SlotCooldowns;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gem")
	TArray<bool> bFusionReady;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gem")
	int32 PendingFusionSlot = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gem")
	int32 MaxSlot = 4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gem|Upgrade")
	float CoolDownReduction = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gem|Upgrade")
	float FusionCoolDownBonus = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gem|State")
	EGemType LastUsedGemType = EGemType::None;

	UPROPERTY()
	UProject_GemCoopCombatComponent* CombatCompRef = nullptr;

	UPROPERTY()
	UProject_GemCoopEnergySYComponent* EnergySYCompRef = nullptr;

	UPROPERTY()
	UProject_GemCoopFusionSYComponent* FusionSYCompRef = nullptr;

	UPROPERTY()
	UProject_GemCoopStatComponent* StatCompRef = nullptr;

	UPROPERTY()
	UProject_GemCoopBuffComponent* BuffCompRef = nullptr;

	UPROPERTY()
	UProject_GemCoopGemDataSubsystem* GemDataSubsystem = nullptr;

	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	UPROPERTY()
	int32 PlayerIndex = 0;

public:
	UFUNCTION(BlueprintCallable, Category = "Gem")
	bool UseGem(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Gem")
	void StartFusionAttempt(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Gem")
	void CompleteFusion(const TArray<FGemData>& FusionGems);

	UFUNCTION(BlueprintCallable, Category = "Gem")
	void CancelFusion();

	UFUNCTION(BlueprintCallable, Category = "Gem")
	void TickCooldown(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Gem")
	void ExchangeGem(int32 SlotIndex, AProject_GemCoopCharacter* Partner);

	UFUNCTION(BlueprintCallable, Category = "Gem")
	FGemData GetGemData(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Gem")
	bool IsSlotReady(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Gem")
	void SetGemSlot(int32 SlotIndex, FGemData NewGem);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	int32 PendingUseSlot = -1;

	UFUNCTION()
	void OnCastingCompleted();

	void ApplyGemEffect(int32 SlotIndex);
	void ApplyFusionEffect(const FFusionResult& Result);
	void StartCoolDown(int32 SlotIndex);
};
