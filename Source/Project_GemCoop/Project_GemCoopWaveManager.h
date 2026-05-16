// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopWaveManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveCleared, int32, WaveNumber);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossSpawned, AActor*, BossActor, int32, BossIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMonsterDeath, AProject_GemCoopMonsterCharacter*, Monster, int32, GoldDrop);

class AProject_GemCoopMonsterCharacter;
class UProject_GemCoopObjectPSubsystem;
class UProject_GemCoopGameInstance;
class AProject_GemCoopGemDropActor;

UCLASS()
class PROJECT_GEMCOOP_API AProject_GemCoopWaveManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProject_GemCoopWaveManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(BlueprintAssignable, Category = "Wave|Delegate")
	FOnWaveStarted OnWaveStarted;

	UPROPERTY(BlueprintAssignable, Category = "Wave|Delegate")
	FOnWaveCleared OnWaveCleared;

	UPROPERTY(BlueprintAssignable, Category = "Wave|Delegate")
	FOnMonsterDeath OnMonsterDeathEvent;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|State")
	int32 CurrentWave = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|State")
	bool bWaveInProgress = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|State")
	bool bAutoStartWave = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Spawn")
	UDataTable* WaveSpawnData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Spawn")
	TArray<AActor*> SpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Spawn")
	TSubclassOf<AProject_GemCoopMonsterCharacter> DefaultMonsterClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave|Spawn")
	TArray<AProject_GemCoopMonsterCharacter*> SpawnedMonsters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Drop")
	TSubclassOf<AProject_GemCoopGemDropActor> GemDropActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Drop")
	bool bDropGemOnMonsterDeath = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Drop")
	float GemDropChance = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Drop")
	float DangerZoneGemDropChance = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Config")
	float TimeBetweenWaves = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Config")
	float DefaultSpawnInterval = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Debug")
	bool bUseHardcodedWaveIfNoDataTable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Debug")
	bool bDebugLog = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|DataTable")
	UDataTable* MonsterDataTable = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|DataTable")
	bool bUseMonsterDataTable = true;

private:
	FTimerHandle NextWaveTimerHandle;
	FTimerHandle SpawnTimerHandle;

	int32 PendingSpawnCount = 0;
	int32 PendingSpawnedCount = 0;
	FWaveSpawnEntry PendingSpawnEntry;

public:
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StartWave(int32 WaveNumber);

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StartNextWave();

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void SpawnMonsterBatch(FWaveSpawnEntry Entry);

	UFUNCTION(BlueprintCallable, Category = "Wave")
	bool IsWaveCleared() const;

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void ClearAllMonsters();

	UFUNCTION(BlueprintCallable, Category = "Wave|Drop")
	void TrySpawnGemDrop(AProject_GemCoopMonsterCharacter* Monster);

	UFUNCTION(BlueprintCallable, Category = "Wave|Scale")
	float GetScaledHP(float BaseHP, int32 Wave) const;

	UFUNCTION(BlueprintCallable, Category = "Wave|Scale")
	float GetScaledATK(float BaseATK, int32 Wave) const;

	UFUNCTION(BlueprintCallable, Category = "Wave|Scale")
	int32 GetScaledCount(int32 BaseCount, int32 Wave) const;

protected:
	UFUNCTION()
	void SpawnSingleMonsterFromPending();

	UFUNCTION()
	void OnMonsterDeath(AProject_GemCoopMonsterCharacter* Monster);

	void OnWaveClearedInternal();

	FGemData MakeFallbackDropGem() const;

	AActor* GetRandomSpawnPoint() const;

	FWaveSpawnEntry MakeHardcodedGoblinEntry(int32 WaveNumber) const;

	void ResetRuntimeWaveState();

	bool GetMonsterDataByType(EMonsterType MonsterType, FMonsterData& OutData) const;

	FName GetMonsterRowName(EMonsterType MonsterType) const;
};
