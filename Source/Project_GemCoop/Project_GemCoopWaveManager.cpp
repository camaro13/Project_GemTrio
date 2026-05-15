// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopWaveManager.h"
#include "Project_GemCoopMonsterCharacter.h"
#include "Project_GemCoopObjectPSubsystem.h"
#include "Project_GemCoopGameInstance.h"
#include "Project_GemCoopGameStateBase.h"
//#include "Project_GemCoopGemDropActor.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProject_GemCoopWaveManager::AProject_GemCoopWaveManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentWave = 0;
	bWaveInProgress = false;
	bAutoStartWave = true;

	TimeBetweenWaves = 5.0f;
	DefaultSpawnInterval = 0.25f;
	bUseHardcodedWaveIfNoDataTable = true;

	bDebugLog = true;
}

// Called when the game starts or when spawned
void AProject_GemCoopWaveManager::BeginPlay()
{
	Super::BeginPlay();
	
	ResetRuntimeWaveState();

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager BeginPlay. AutoStart=%d, DefaultMonsterClass=%s, SpawnPoints=%d"),
			bAutoStartWave,
			*GetNameSafe(DefaultMonsterClass),
			SpawnPoints.Num()
		);
	}

	if (bAutoStartWave)
	{
		if (bDebugLog)
		{
			UE_LOG(LogTemp, Warning, TEXT("WaveManager calls StartNextWave."));
		}

		StartNextWave();
	}
}

// Called every frame
void AProject_GemCoopWaveManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProject_GemCoopWaveManager::ResetRuntimeWaveState()
{
	CurrentWave = 0;
	bWaveInProgress = false;

	PendingSpawnCount = 0;
	PendingSpawnedCount = 0;

	SpawnedMonsters.Empty();

	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	GetWorldTimerManager().ClearTimer(NextWaveTimerHandle);
}

bool AProject_GemCoopWaveManager::GetMonsterDataByType(EMonsterType MonsterType, FMonsterData& OutData) const
{
	if (!bUseMonsterDataTable || !MonsterDataTable)
	{
		return false;
	}

	FName RowName = GetMonsterRowName(MonsterType);
	FString ContextString = TEXT("GetMonsterDataByType");

	FMonsterData* Row = MonsterDataTable->FindRow<FMonsterData>(RowName, ContextString);

	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("MonsterData row not found: %s"), *RowName.ToString());
		return false;
	}

	OutData = *Row;
	return true;
}

FName AProject_GemCoopWaveManager::GetMonsterRowName(EMonsterType MonsterType) const
{
	switch(MonsterType)
	{
	case EMonsterType::Goblin:
		return FName(TEXT("Goblin"));

	case EMonsterType::Bat:
		return FName(TEXT("Bat"));

	case EMonsterType::Skeleton:
		return FName(TEXT("Skeleton"));

	case EMonsterType::Orc:
		return FName(TEXT("Orc"));

	case EMonsterType::Elite:
		return FName(TEXT("Elite"));

	case EMonsterType::MiniBoss:
		return FName(TEXT("MiniBoss"));

	case EMonsterType::Boss:
		return FName(TEXT("Boss"));

	default:
		break;
	}
	return FName(TEXT("Goblin"));
}

void AProject_GemCoopWaveManager::StartNextWave()
{
	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartNextWave Called. CurrentWave=%d"), CurrentWave);
	}
	StartWave(CurrentWave + 1);
}

void AProject_GemCoopWaveManager::StartWave(int32 WaveNumber)
{
	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartWave Called. Wave=%d, bWaveInProgress=%d, Hardcoded=%d"),
			WaveNumber,
			bWaveInProgress,
			bUseHardcodedWaveIfNoDataTable
		);
	}

	if (bWaveInProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartWave blocked. Wave already in progress."));
		return;
	}

	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	GetWorldTimerManager().ClearTimer(NextWaveTimerHandle);

	CurrentWave = FMath::Max(1, WaveNumber);
	bWaveInProgress = true;

	if (AProject_GemCoopGameStateBase* GS = GetWorld()->GetGameState<AProject_GemCoopGameStateBase>())
	{
		GS->SetCurrentWaveNumber(CurrentWave);
	}

	PendingSpawnCount = 0;
	PendingSpawnedCount = 0;
	SpawnedMonsters.Empty();

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wave Started: %d"), CurrentWave);
	}

	/*SpawnedMonsters.RemoveAll([](AProject_GemCoopMonsterCharacter* Monster)
		{
			return Monster == nullptr || Monster->IsPendingKillPending();
		});*/

	OnWaveStarted.Broadcast(CurrentWave);

	bool bSpawnedFromDataTable = false;

	if (WaveSpawnData)
	{
		TArray<FWaveSpawnEntry*> Rows;
		FString ContextString = TEXT("WaveSpawnData");

		WaveSpawnData->GetAllRows<FWaveSpawnEntry>(ContextString, Rows);

		for (FWaveSpawnEntry* Row : Rows)
		{
			if (!Row)
			{
				continue;
			}

			if (Row->WaveNumber != CurrentWave)
			{
				continue;
			}

			SpawnMonsterBatch(*Row);
			bSpawnedFromDataTable = true;

			break;
		}
	}

	if (!bSpawnedFromDataTable && bUseHardcodedWaveIfNoDataTable)
	{
		if (bDebugLog)
		{
			UE_LOG(LogTemp, Warning, TEXT("Using hardcoded goblin wave."));
		}

		FWaveSpawnEntry Entry = MakeHardcodedGoblinEntry(CurrentWave);
		SpawnMonsterBatch(Entry);
	}
	else if (!bSpawnedFromDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("No WaveSpawnData row and hardcoded wave disabled."));

		bWaveInProgress = false;
	}
}

void AProject_GemCoopWaveManager::SpawnMonsterBatch(FWaveSpawnEntry Entry)
{
	if (GetWorldTimerManager().IsTimerActive(SpawnTimerHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnMonsterBatch blocked. Spawn timer already active."));
		return;
	}

	FMonsterData TableMonsterData;

	if (!Entry.MonsterClass && GetMonsterDataByType(Entry.MonsterType, TableMonsterData))
	{
		Entry.MonsterClass = TableMonsterData.MonsterClass;
	}

	if (!Entry.MonsterClass)
	{
		Entry.MonsterClass = DefaultMonsterClass;
	}

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnMonsterBatch. MonsterClass=%s, BaseCount=%d"),
			*GetNameSafe(Entry.MonsterClass),
			Entry.BaseCount
		);
	}

	if (!Entry.MonsterClass)
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager: MonsterClass is null. Set DefaultMonsterClass in BP_WaveManager."));
		return;
	}

	PendingSpawnEntry = Entry;
	PendingSpawnCount = FMath::Max(1, GetScaledCount(Entry.BaseCount, CurrentWave));
	PendingSpawnedCount = 0;

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnMonsterBatch Start. Wave=%d Count=%d"),
			CurrentWave,
			PendingSpawnCount
		);
	}

	float Interval = Entry.SpawnInterval > 0.0f ? Entry.SpawnInterval : DefaultSpawnInterval;

	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AProject_GemCoopWaveManager::SpawnSingleMonsterFromPending, Interval, true, 0.0f);
}

void AProject_GemCoopWaveManager::SpawnSingleMonsterFromPending()
{
	if (PendingSpawnedCount >= PendingSpawnCount)
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

		if (bDebugLog)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawn finished. Spawned=%d / %d"),
				PendingSpawnedCount,
				PendingSpawnCount
			);
		}

		if (IsWaveCleared())
		{
			OnWaveClearedInternal();
		}

		return;
	}


	if (!PendingSpawnEntry.MonsterClass)
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	AActor* SpawnPoint = GetRandomSpawnPoint();

	FVector SpawnLocation = SpawnPoint ? SpawnPoint->GetActorLocation() : GetActorLocation() + FVector(150.f, 0.0f, 50.f);

	FRotator SpawnRotation = SpawnPoint ? SpawnPoint->GetActorRotation() : FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AProject_GemCoopMonsterCharacter* Monster = GetWorld()->SpawnActor<AProject_GemCoopMonsterCharacter>(PendingSpawnEntry.MonsterClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (!Monster)
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager: SpawnActor returned nullptr."));
		return;
	}

	FMonsterData MonsterData;

	if (!GetMonsterDataByType(PendingSpawnEntry.MonsterType, MonsterData))
	{
		MonsterData.MonsterType = PendingSpawnEntry.MonsterType;
		MonsterData.BaseHP = Monster->MaxHP;
		MonsterData.BaseATK = Monster->ATK;
		MonsterData.MoveSpeed = Monster->MoveSpeed;
		MonsterData.WeaknessGem = Monster->WeaknessGemType;
		MonsterData.DropGrade = Monster->GemDropGrade;
		MonsterData.GoldDrop = Monster->GoldDropAmount;
		MonsterData.MonsterClass = PendingSpawnEntry.MonsterClass;
	}

	MonsterData.BaseHP = GetScaledHP(MonsterData.BaseHP, CurrentWave);
	MonsterData.BaseATK = GetScaledATK(MonsterData.BaseATK, CurrentWave);

	Monster->InitializeFromData(MonsterData);
	Monster->OnMonsterDied.AddDynamic(this, &AProject_GemCoopWaveManager::OnMonsterDeath);

	SpawnedMonsters.Add(Monster);
	PendingSpawnedCount++;

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster Spawned: %s (%d / %d)"),
			*GetNameSafe(Monster),
			PendingSpawnedCount,
			PendingSpawnCount
		);
	}
}

void AProject_GemCoopWaveManager::OnMonsterDeath(AProject_GemCoopMonsterCharacter* Monster)
{
	if (!Monster)
	{
		return;
	}

	SpawnedMonsters.Remove(Monster);

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager Received MonsterDeath: %s, AliveCount=%d"),
			*GetNameSafe(Monster),
			SpawnedMonsters.Num()
		);
	}

	OnMonsterDeathEvent.Broadcast(Monster, Monster->GoldDropAmount);

	if (IsWaveCleared())
	{
		OnWaveClearedInternal();
	}
}

bool AProject_GemCoopWaveManager::IsWaveCleared() const
{
	if (!bWaveInProgress)
	{
		return false;
	}

	if (PendingSpawnedCount < PendingSpawnCount)
	{
		return false;
	}

	for (AProject_GemCoopMonsterCharacter* Monster : SpawnedMonsters)
	{
		if (Monster && !Monster->bIsDead)
		{
			return false;
		}
	}

	return true;
}

void AProject_GemCoopWaveManager::OnWaveClearedInternal()
{
	if (!bWaveInProgress)
	{
		return;
	}

	bWaveInProgress = false;

	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wave Cleared: %d"), CurrentWave);
	}

	OnWaveCleared.Broadcast(CurrentWave);

	GetWorldTimerManager().SetTimer(NextWaveTimerHandle, this, &AProject_GemCoopWaveManager::StartNextWave, TimeBetweenWaves, false);
}

void AProject_GemCoopWaveManager::ClearAllMonsters()
{
	for (AProject_GemCoopMonsterCharacter* Monster : SpawnedMonsters)
	{
		if (IsValid(Monster))
		{
			Monster->Destroy();
		}
	}

	SpawnedMonsters.Empty();
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

float AProject_GemCoopWaveManager::GetScaledHP(float BaseHP, int32 Wave) const
{
	const float SafeBaseHP = FMath::Max(1.0f, BaseHP);
	const int32 SafeWave = FMath::Max(1, Wave);

	return SafeBaseHP * (1.0f + SafeWave * 0.30f);
}

float AProject_GemCoopWaveManager::GetScaledATK(float BaseATK, int32 Wave) const
{
	const float SafeBaseATK = FMath::Max(1.0f, BaseATK);
	const int32 SafeWave = FMath::Max(1, Wave);

	return SafeBaseATK * (1.0f + SafeWave * 0.25f);
}

int32 AProject_GemCoopWaveManager::GetScaledCount(int32 BaseCount, int32 Wave) const
{
	const int32 SafeBaseCount = FMath::Max(1, BaseCount);
	const int32 SafeWave = FMath::Max(1, Wave);

	const float Scale = 1.0f + static_cast<float>(SafeWave) * 0.5f;
	const int32 Result = FMath::CeilToInt(static_cast<float>(SafeBaseCount) * Scale);

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetScaledCount: Base=%d, Wave=%d, Scale=%.2f, Result=%d"),
			SafeBaseCount,
			SafeWave,
			Scale,
			Result
		);
	}

	return FMath::Max(1, Result);
}

AActor* AProject_GemCoopWaveManager::GetRandomSpawnPoint() const
{
	if (SpawnPoints.Num() <= 0)
	{
		return nullptr;
	}

	int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1);
	return SpawnPoints[Index];
}

FWaveSpawnEntry AProject_GemCoopWaveManager::MakeHardcodedGoblinEntry(int32 WaveNumber) const
{
	FWaveSpawnEntry Entry;

	Entry.WaveNumber = WaveNumber;
	Entry.MonsterType = EMonsterType::Goblin;
	Entry.MonsterClass = DefaultMonsterClass;
	Entry.BaseCount = 1;
	Entry.SpawnDelay = 0.0f;
	Entry.SpawnInterval = DefaultSpawnInterval;
	Entry.bIsBossWave = false;
	Entry.BossIndex = 0;

	return Entry;
}

//void AProject_GemCoopWaveManager::TickBossTimer(float DeltaTime)
//{
//	TotalElapsedTime += DeltaTime;
//
//	if (!bBoss1Triggered && TotalElapsedTime >= 300.f)
//	{
//		TriggerBoss(1);
//		bBoss1Triggered = true;
//	}
//
//	if (!bBoss2Triggered && TotalElapsedTime >= 600.f)
//	{
//		TriggerBoss(2);
//		bBoss2Triggered = true;
//	}
//
//	if (!bBoss3Triggered && TotalElapsedTime >= 900.f)
//	{
//		TriggerBoss(3);
//		bBoss3Triggered = true;
//	}
//}
//
//void AProject_GemCoopWaveManager::TriggerBoss(int32 BossIndex)
//{
//	TSubclassOf<AProject_GemCoopMonsterCharacter> SpawnClass = (BossIndex == 1) ? MiniBossClass : BossClass;
//
//	AActor* CenterSpawn = SpawnPoints[0];
//
//	AActor* Spawned = ObjectPoolSubsystem->SpawnFromPool(SpawnClass, CenterSpawn->GetActorLocation(), FRotator::ZeroRotator);
//
//	auto* Boss = Cast<AProject_GemCoopMonsterCharacter>(Spawned);
//	
//	if (!Boss)
//	{
//		return;
//	}
//
//	Boss->MaxHP = GetScaledHP(Boss->MaxHP, CurrentWave) * 1.4f;
//	Boss->CurrentHP = Boss->MaxHP;
//	Boss->ATK = GetScaledATK(Boss->ATK, CurrentWave);
//
//	Boss->OnMonsterDied.AddDynamic(this, &AProject_GemCoopWaveManager::OnMonsterDeathF);
//
//	SpawnedMonsters.Add(Boss);
//	OnBossSpawned.Broadcast(Boss, BossIndex);
//}
//
//void AProject_GemCoopWaveManager::OnMonsterDeathF(AProject_GemCoopMonsterCharacter* Monster)
//{
//	SpawnedMonsters.Remove(Monster);
//
//	DropReward(Monster);
//
//	ObjectPoolSubsystem->ReturnToPool(Monster);
//
//	OnMonsterDeath.Broadcast(Monster, Monster->GoldDropAmount);
//
//	if (IsWaveCleared())
//	{
//		OnWaveClearedF();
//	}
//}
//
//void AProject_GemCoopWaveManager::DropReward(AProject_GemCoopMonsterCharacter* Monster)
//{
//	int32 ScaledGold = FMath::RoundToInt(Monster->GoldDropAmount * (1.f + CurrentWave * 0.2f));
//
//	auto* GI = Cast<UProject_GemCoopGameInstance>(GetGameInstance());
//	if (GI)
//	{
//		GI->AddGold(ScaledGold);
//	}
//
//	float DropRoll = FMath::FRand();
//	float DropChance = (Monster->GetCurrentZone() == EArenaZone::Danger) ? 0.4f : 0.2f;
//
//	/*if (DropRoll < DropChance)
//	{
//		FActorSpawnParameters Params;
//		AProject_GemCoopGemDropActor* GemDrop = GetWorld()->SpawnActor<AProject_GemCoopGemDropActor>(GemDropActorClass, Monster->GetActorLocation(), FRotator::ZeroRotator, Params);
//		
//		if (GemDrop)
//		{
//			GemDrop->SetGemGrade(Monster->GemDropGrade);
//		}
//	}*/
//}
//
//void AProject_GemCoopWaveManager::OnWaveClearedF()
//{
//	bWaveInProgress = false;
//	OnWaveCleared.Broadcast(CurrentWave);
//
//	GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AProject_GemCoopWaveManager::StartNextWave, 5.f, false);
//}