// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, float, CurrentHP, float, MaxHP);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_GEMCOOP_API UProject_GemCoopStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProject_GemCoopStatComponent();

	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable)
	FOnHPChanged OnHPChanged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Base")
	float BaseHP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Base")
	float BaseATK = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Base")
	float BaseDEF = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Base")
	float BaseSPD = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Base")
	float CritChance = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Base")
	float RegenRate = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Current")
    float CurrentHP = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Final")
    float FinalHP = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Final")
    float FinalATK = 20.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Final")
    float FinalDEF = 10.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Final")
    float FinalSPD = 600.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Upgrade")
    float UpgradeHP = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Upgrade")
    float UpgradeATK = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Upgrade")
    float UpgradeDEF = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Upgrade")
    float UpgradeSPD = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Upgrade")
    float UpgradeCrit = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Modifier")
    FStatModifier CurrentModifier;

public:
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void RecalculateFinalStats();

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void ApplyDamage(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void ApplyHeal(float Amount);

    UFUNCTION(BlueprintPure, Category = "Stats")
    bool IsDead() const;

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetCurrentHPPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void SetCurrentHPPercent(float HPPercent);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void ApplyPermanentUpgrade(FName UpgradeID, int32 Level);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void SetModifier(const FStatModifier& NewModifier);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    void HandleDeath();
    bool bIsDead = false;
};
