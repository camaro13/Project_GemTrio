// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopBuffComponent.generated.h"

class UProject_GemCoopStatComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuffChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShieldChanged, float, ShieldAmount);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_GEMCOOP_API UProject_GemCoopBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProject_GemCoopBuffComponent();

	UPROPERTY(BlueprintAssignable)
	FOnBuffChanged OnBuffChanged;

	UPROPERTY(BlueprintAssignable)
	FOnShieldChanged OnShieldChanged;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buff")
    TArray<FBuffData> ActiveBuffs;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buff")
    TArray<FDebuffData> ActiveDebuffs;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buff|Total")
    float TotalATKBonus = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buff|Total")
    float TotalDEFBonus = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buff|Total")
    float TotalSPDBonus = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buff|Total")
    float TotalHealBonus = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buff|Special")
    bool bIsBurning = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buff|Special")
    float BurnDamagePerSec = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buff|Special")
    float ShieldAmount = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buff|Special")
    float DistortionValue = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buff|Special")
    float MaxDistortion = 100.f;

    UPROPERTY()
    UProject_GemCoopStatComponent* StatCompRef = nullptr;

    FStatModifier TraitModifier;


public:
    UFUNCTION(BlueprintCallable, Category = "Buff")
    void AddBuff(FBuffData Buff);

    UFUNCTION(BlueprintCallable, Category = "Buff")
    void AddDebuff(FDebuffData Debuff);

    UFUNCTION(BlueprintCallable, Category = "Buff")
    void RemoveBuff(FName BuffID);

    UFUNCTION(BlueprintCallable, Category = "Buff")
    void RemoveDebuff(FName DebuffID);

    UFUNCTION(BlueprintCallable, Category = "Buff")
    void TickBuffs(float DeltaTime);

    UFUNCTION(BlueprintPure, Category = "Buff")
    int32 GetBuffCount() const;

    UFUNCTION(BlueprintPure, Category = "Buff")
    int32 GetDebuffCount() const;

    UFUNCTION(BlueprintCallable, Category = "Buff")
    void RecalculateBonuses();

    UFUNCTION(BlueprintPure, Category = "Buff")
    FStatModifier GetCombinedModifier() const;

    UFUNCTION(BlueprintCallable, Category = "Buff")
    void ApplyShield(float Amount);

    // 피해 흡수: Damage를 참조로 받아 보호막 차감 후 잔여 피해 반환
    UFUNCTION(BlueprintCallable, Category = "Buff")
    void AbsorbDamage(float& Damage);

    UFUNCTION(BlueprintCallable, Category = "Buff")
    void AddDistortion(float Amount);

    void SetTraitModifier(const FStatModifier& Modifier);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    void TriggerDistortionExplosion();
};
