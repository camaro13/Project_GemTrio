// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Math/Color.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopTraitDSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API UProject_GemCoopTraitDSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Data")
	UDataTable* TraitDataTable = nullptr;

	UPROPERTY(EditAnywhere, Category="Data")
	TMap<EGemTrait, FLinearColor> TraitColorMap;

	UFUNCTION(BlueprintCallable)
	FTraitData GetTraitData(EGemTrait Trait) const;

	UFUNCTION(BlueprintCallable)
	TArray<EGemType> GetPreferredGems(EGemTrait Trait) const;

	UFUNCTION(BlueprintCallable)
	FLinearColor GetTraitColor(EGemTrait Trait) const;

	UFUNCTION(BlueprintCallable)
	FStatModifier GetTraitStatModifier(EGemTrait Trait) const;

	UFUNCTION(BlueprintCallable)
	FText GetTraitDescription(EGemTrait Trait) const;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	FName GetEnumCleanName(EGemTrait Trait) const;
};
