// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameOver, bool, bVictory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoopGradeUpdated, FString, Grade, float, Score);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossDefeated, int32, BossIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPartyUltimateActivated, AProject_GemCoopCharacter*, Instigator);


class AProject_GemCoopWaveManager;
class AProject_GemCoopCharacter;
class AProject_GemCoopGameStateBase;
class AProject_GemCoopGameInstance;
/**
 *  Simple GameMode for a third person game
 */

USTRUCT(BlueprintType)
struct FGameResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bVictory = false;

	UPROPERTY()
	ECoopGrade CoopGrade = ECoopGrade::F;

	UPROPERTY()
	float CoopScore = 0.0f;

	UPROPERTY()
	float SurvivedTime = 0.0f;

	UPROPERTY()
	int32 ReachedWave = 0;

	UPROPERTY()
	int32 GoldReward = 0;

	UPROPERTY()
	int32 UpgradePointReward = 0;

	UPROPERTY()
	int32 TotalKillCount = 0;

	UPROPERTY()
	int32 FusionSuccessCount = 0;

	UPROPERTY()
	int32 GemExchangeCount = 0;

	UPROPERTY()
	int32 TeamDeathCount = 0;
};

UCLASS(abstract)
class AProject_GemCoopGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	AProject_GemCoopGameMode();

	UPROPERTY(BlueprintAssignable)
	FOnGameOver OnGameOver;

	UPROPERTY(BlueprintAssignable)
	FOnCoopGradeUpdated OnCoopGradeUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnBossDefeated OnBossDefeated;

	UPROPERTY(BlueprintAssignable)
	FOnPartyUltimateActivated OnPartyUltimateActivated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	int32 CurrentWave = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	float WaveTimer = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	float TotalGameTime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	bool bIsGameOver = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	AProject_GemCoopWaveManager* WaveManagerRef = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coop")
	float CoopScore = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coop")
	int32 FusionSuccessCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coop")
	int32 FusionAttemptCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coop")
	float FormationMaintainTime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coop")
	int32 GemExchangeCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coop")
	int32 RescueCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coop")
	int32 TeamDeathCount = 0;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartNextWave();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void EndGame(bool bVictory);

	UFUNCTION(BlueprintCallable, Category = "Game")
	float CalculateCoopGrade();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnPlayerDeath(int32 TeamIndex);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnPlayerPermanentDeath(int32 TeamIndex);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RecordFusionResult(bool bSuccess);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnFusionSuccess(FName FusionID, float Multiplier);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnGemExchanged();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void ActivatePartyUltimate(AProject_GemCoopCharacter* Instigate);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnWaveClearedCallback(int32 WaveNumber);

	UFUNCTION()
	void OnBossSpawnedCallback(AActor* BossActor, int32 BossIndex);

	UFUNCTION()
	void OnBossDefeatedCallback(AProject_GemCoopMonsterCharacter* Monster);

	void CheckVictoryCondition();
	int32 AlivePlayerCount() const;

	int32 CurrentBossIndex = 0;

	bool bBoss1Defeated = false;
	bool bBoss2Defeated = false;
	bool bBoss3Defeated = false;
};



