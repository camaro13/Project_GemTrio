// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopTraitComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTraitChanged, EGemTrait, NewTrait);

class UProject_GemCoopBuffComponent;
class UProject_GemCoopEnergySYComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_GEMCOOP_API UProject_GemCoopTraitComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProject_GemCoopTraitComponent();

	UPROPERTY(BlueprintAssignable)
	FOnTraitChanged OnTraitChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trait")
	EGemTrait CurrentTrait = EGemTrait::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trait|Multiplier")
	float ATKMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trait|Multiplier")
	float DEFMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trait|Multiplier")
	float HPMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trait|Multiplier")
	float SPDMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trait|Multiplier")
	float HealEffectMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trait|Multiplier")
	float EnergyRegenMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trait|Multiplier")
	float CritChanceBonus = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trait")
	TArray<EGemType> PreferredGemTypes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trait")
	FLinearColor TraitColor = FLinearColor::White;

	UPROPERTY()
	UProject_GemCoopBuffComponent* BuffCompRef = nullptr;

	UPROPERTY()
	UProject_GemCoopEnergySYComponent* EnergySYCompRef = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Trait")
	void SetTrait(EGemTrait NewTrait);

	UFUNCTION(BlueprintPure, Category = "Trait")
	FStatModifier GetStatModifier() const;

	UFUNCTION(BlueprintPure, Category = "Trait")
	bool IsPreferredGem(EGemType GemType) const;

	UFUNCTION(BlueprintPure, Category = "Trait")
	float GetHealBonus() const;

	UFUNCTION(BlueprintPure, Category = "Trait")
	float GetEnergyRegenMultiplier() const;

	UFUNCTION(BlueprintPure, Category = "Trait")
	FLinearColor GetTraitColor() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void ApplyTraitMultipliers();
	void InitPreferredGems();
};
