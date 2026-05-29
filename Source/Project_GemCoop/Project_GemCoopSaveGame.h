// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FSaveOwnedGemEntry
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
	FName GemID = NAME_None;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
	EGemType GemType = EGemType::None;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
	EGemGrade Grade = EGemGrade::Common;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;
};

/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API UProject_GemCoopSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(SaveGame)
	FString SaveSlotName = TEXT("GemBlastSaveSlot0");

	UPROPERTY(SaveGame)
	int32 TotalGold = 0;

	UPROPERTY(SaveGame)
	TMap<FName, int32> UpgradeLevels;

	UPROPERTY(SaveGame)
	TMap<FName, FGemCodexEntry> GemCodex;

	UPROPERTY(SaveGame)
	TMap<FName, bool> Achievements;

	UPROPERTY(SaveGame)
	float TotalPlayTime = 0.f;

	UPROPERTY(SaveGame)
	int32 TotalMonstersKilled = 0;

	UPROPERTY(SaveGame)
	int32 TotalBossesKilled = 0;
	
	UPROPERTY(SaveGame)
	int32 TotalFusionSuccess = 0;

	UPROPERTY(SaveGame)
	float HighestSurvivalTime = 0.f;

	UPROPERTY(SaveGame)
	int32 HighestWaveReached = 0;

	UPROPERTY(SaveGame)
	EGemTrait LastSelectedTrait = EGemTrait::Offense;

	UPROPERTY(SaveGame)
	EGemTrait PlayerTrait = EGemTrait::Offense;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "GemInventory")
	TArray<FSaveOwnedGemEntry> SavedGemInventory;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "GemInventory")
	TArray<FName> SavedEquippedGemIDs;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FGemCoopUserSettings SavedUserSettings;

	UFUNCTION(BlueprintCallable)
	void AddGem(FName GemID, EGemGrade Grade);

	UFUNCTION(BlueprintPure)
	bool IsAchievementUnlocked(FName AchievementID) const;

	UFUNCTION(BlueprintCallable)
	void UnlockAchievement(FName AchievementID);

	UFUNCTION(BlueprintCallable)
	void UpdateHighScore(float SurvivalTime, int32 Wave);

	UFUNCTION(BlueprintPure)
	int32 GetGemCodexCompletionPercent() const;

	/*UFUNCTION(BlueprintCallable)
	void AccumulateStats(FGameResult Result);*/

private:
	static constexpr int32 TotalGemCount = 23;
};
