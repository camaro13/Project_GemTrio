// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopGemDataSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FGradeDropRate : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float CommonRate = 70.f;

	UPROPERTY(EditAnywhere)
	float FlawlessRate = 25.f;

	UPROPERTY(EditAnywhere)
	float StarRate = 4.f;

	UPROPERTY(EditAnywhere)
	float PrismaticRate = 1.f;
};

/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API UProject_GemCoopGemDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Data")
	UDataTable* GemDataTable = nullptr;

	UPROPERTY(EditAnywhere, Category="Data")
	UDataTable* FusionRecipeTable = nullptr;

	UPROPERTY(EditAnywhere, Category="Data")
	UDataTable* GradeDropTable = nullptr;

	UPROPERTY(EditAnywhere, Category="Data")
	TArray<EGemType> AllGemTypes;

	UFUNCTION(BlueprintCallable)
	FGemData GetGemData(FName GemID) const;

	UFUNCTION(BlueprintCallable)
	FFusionResult LookupFusion(TArray<EGemType> GemTypes);

	UFUNCTION(BlueprintCallable)
	bool IsValidFusion(TArray<EGemType> GemTypes);

	UFUNCTION(BlueprintCallable)
	FGemData GenerateRandomGem(EGemGrade MinGrade);

	UFUNCTION(BlueprintCallable)
	EGemGrade RollGemGrade(EMonsterType MonsterType);

	UFUNCTION(BlueprintCallable)
	FGemData PolishGem(TArray<FGemData> Materials);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	FName GetEnumCleanName(EGemType GemType) const;
	FName GetEnumCleanName(EGemGrade Grade) const;
	FName GetEnumCleanName(EMonsterType MonsterType) const;

	void SortGemTypes(TArray<EGemType>& GemTypes) const;
	bool AreSameGemTypeSet(const TArray<EGemType>& A, const TArray<EGemType>& B) const;
};
