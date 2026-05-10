// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopGameMode.h"
#include "Project_GemCoopGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int32, NewAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpgradeApplied, FName, UpgradeID, int32, Level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGemCodexUpdated, FName, GemID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAchievementUnlocked, FName, AchID);

class UProject_GemCoopSaveGame;
class UProject_GemCoopGemDataSubsystem;
class UProject_GemCoopTraitDSubsystem;
class AProject_GemCoopCharacter;
/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API UProject_GemCoopGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable)
	FOnGoldChanged OnGoldChanged;

	UPROPERTY(BlueprintAssignable)
	FOnUpgradeApplied OnUpgradeApplied;

	UPROPERTY(BlueprintAssignable)
	FOnGemCodexUpdated OnGemCodexUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnAchievementUnlocked OnAchievementUnlocked;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save")
	UProject_GemCoopSaveGame* SaveData = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Session")
	FString CurrentSessionInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Session")
	bool bIsOnlineSession = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Result")
	FGameResult LastGameResult;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	EGemTrait LocalPlayerTrait = EGemTrait::Offense;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency")
	int32 TotalGoldCurrency = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Upgrade")
	TMap<FName, int32> PermanentUpgrades;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Codex")
	TMap<FName, FGemCodexEntry> GemCodexData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Achievement")
	TMap<FName, bool> AchievementData;

	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveGameToSlot();

	UFUNCTION(BlueprintCallable, Category = "Save")
	void LoadGameData();

	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveGameResult(FGameResult Result);

	UFUNCTION(BlueprintCallable, Category = "Currency")
	void AddGold(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Currency")
	bool SpendGold(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	bool ApplyUpgrade(FName UpgradeID);

	UFUNCTION(BlueprintPure, Category = "Upgrade")
	int32 GetUpgradeLevel(FName UpgradeID);

	UFUNCTION(BlueprintPure, Category = "Upgrade")
	int32 GetUpgradeCost(FName UpgradeID);

	UFUNCTION(BlueprintCallable, Category = "Codex")
	void RegisterGemToCodex(FName GemID, FGemCodexEntry Entry);

	UFUNCTION(BlueprintPure, Category = "Codex")
	bool IsGemCollected(FName GemID) const;

	UFUNCTION(BlueprintCallable, Category = "Achievement")
	void UnlockAchievement(FName AchID);

	UFUNCTION(BlueprintCallable, Category = "Character")
	void ApplyPermanentUpgradesToCharacter(AProject_GemCoopCharacter* Character);

	virtual void Init() override;


public:
	void CheckAchievements(const FGameResult& Result);
	int32 GetUpgradeMaxLevel(FName UpgradeID) const;
	UProject_GemCoopSaveGame* GetSaveData() { return SaveData; }
};
