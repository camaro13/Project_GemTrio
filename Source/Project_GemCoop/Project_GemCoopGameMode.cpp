// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project_GemCoopGameMode.h"
#include "Project_GemCoopWaveManager.h"
#include "Project_GemCoopCharacter.h"
#include "Project_GemCoopGameStateBase.h"
#include "Project_GemCoopGameInstance.h"
#include "Project_GemCoopBuffComponent.h"
#include "Project_GemCoopMonsterCharacter.h"
#include "Kismet/GameplayStatics.h"

AProject_GemCoopGameMode::AProject_GemCoopGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AProject_GemCoopGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Managers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProject_GemCoopWaveManager::StaticClass(), Managers);

	if (Managers.Num() > 0)
	{
		WaveManagerRef = Cast<AProject_GemCoopWaveManager>(Managers[0]);
	}

	if (WaveManagerRef)
	{
		WaveManagerRef->OnWaveCleared.AddDynamic(this, &AProject_GemCoopGameMode::OnWaveClearedCallback);
		//WaveManagerRef->OnBossSpawned.AddDynamic(this, &AProject_GemCoopGameMode::OnBossSpawnedCallback);
	}
}

void AProject_GemCoopGameMode::StartNextWave()
{
	CurrentWave++;
	WaveTimer = 0.f;

	if (WaveManagerRef)
	{
		WaveManagerRef->StartWave(CurrentWave);
	}

	auto* GS = GetGameState<AProject_GemCoopGameStateBase>();

	if (GS)
	{
		GS->CurrentWaveNumber = CurrentWave;
	}
}

void AProject_GemCoopGameMode::OnPlayerDeath(int32 TeamIndex)
{
	TeamDeathCount++;

	if (AlivePlayerCount() == 0)
	{
		EndGame(false);
	}
}

void AProject_GemCoopGameMode::OnPlayerPermanentDeath(int32 TeamIndex)
{
	if (AlivePlayerCount() == 0)
	{
		EndGame(false);
	}
}

void AProject_GemCoopGameMode::RecordFusionResult(bool bSuccess)
{
	FusionAttemptCount++;

	if (bSuccess)
	{
		FusionSuccessCount++;

		if (FusionSuccessCount % 5 == 0)
		{
			CalculateCoopGrade();
		}
	}
}

void AProject_GemCoopGameMode::OnFusionSuccess(FName FusionID, float Multiplier)
{
	RecordFusionResult(true);
	if (Multiplier >= 3.0f)
	{
		CoopScore = FMath::Min(CoopScore + 2.f, 100.f);
	}
}

void AProject_GemCoopGameMode::EndGame(bool bVictory)
{
	if (bIsGameOver)
	{
		return;
	}

	bIsGameOver = true;

	float FinalScore = CalculateCoopGrade();

	FGameResult Result;
	Result.bVictory = bVictory;
	Result.CoopScore = FinalScore;
	Result.ReachedWave = CurrentWave;
	Result.SurvivedTime = TotalGameTime;
	Result.FusionSuccessCount = FusionSuccessCount;
	Result.GemExchangeCount = GemExchangeCount;

	if (FinalScore >= 90.f) Result.UpgradePointReward = 3;
	else if (FinalScore >= 75.f) Result.UpgradePointReward = 2;
	else if (FinalScore >= 55.f) Result.UpgradePointReward = 1;
	else                         Result.UpgradePointReward = 0;

	auto* GI = Cast<UProject_GemCoopGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SaveGameResult(Result);
	}

	OnGameOver.Broadcast(bVictory);

	FTimerHandle EndTimer;
	GetWorldTimerManager().SetTimer(EndTimer, [this]() {
		UGameplayStatics::OpenLevel(this, FName("ResultScreen"));
		}, 3.f, false);

}

float AProject_GemCoopGameMode::CalculateCoopGrade()
{
	float FusionRate = (FusionAttemptCount > 0) ? (float)FusionSuccessCount / FusionAttemptCount : 0.f;
	float FusionScore = FusionRate * 100.f * 0.30f;

	float FormationRate = (TotalGameTime > 0.f) ? FormationMaintainTime / TotalGameTime : 0.f;
	float FormationScore = FormationRate * 100.f * 0.25f;

	float ExchangeScore = FMath::Min(GemExchangeCount * 5.f, 100.f) * 0.20f;

	float RescueScore = FMath::Min(RescueCount * 10.f, 100.f) * 0.15f;

	float DeathPenalty = FMath::Min(TeamDeathCount * 5.f, 50.f) * 0.10f;

	CoopScore = FMath::Clamp(FusionScore + FormationScore + ExchangeScore + RescueScore - DeathPenalty, 0.f, 100.f);

	FString Grade;

	if (CoopScore >= 90.f)
	{
		Grade = "S";
	}
	else if (CoopScore >= 75.f)
	{
		 Grade = "A";
	}
	else if (CoopScore >= 55.f)
	{
		 Grade = "B";
	}
	else
	{
		Grade = "C";
	}

	OnCoopGradeUpdated.Broadcast(Grade, CoopScore);
	return CoopScore;
}

void AProject_GemCoopGameMode::OnGemExchanged()
{
	GemExchangeCount++;
}

void AProject_GemCoopGameMode::ActivatePartyUltimate(AProject_GemCoopCharacter* Instigate)
{
	TArray<AActor*> AllChars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProject_GemCoopCharacter::StaticClass(), AllChars);

	for (AActor* Actor : AllChars)
	{
		auto* Char = Cast<AProject_GemCoopCharacter>(Actor);
		if (!Char || !Char->bIsAlive)
		{
			continue;
		}

		FBuffData UltBuff;
		UltBuff.BuffID = "PartyUltimate";
		UltBuff.Duration = 10.f;
		UltBuff.ATKBonus = 1.0f;
		Char->BuffCompRef->AddBuff(UltBuff);

		FBuffData InvincBuff;
		InvincBuff.BuffID = "UltInvincible";
		InvincBuff.Duration = 2.f;
		InvincBuff.DEFBonus = 999.f;
		Char->BuffCompRef->AddBuff(InvincBuff);
	}

	OnPartyUltimateActivated.Broadcast(Instigate);
}



void AProject_GemCoopGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsGameOver)
	{
		return;
	}

	TotalGameTime += DeltaTime;
	WaveTimer += DeltaTime;

	auto* GS = GetGameState<AProject_GemCoopGameStateBase>();

	if (GS && GS->bFormationActive)
	{
		FormationMaintainTime += DeltaTime;
	}

	CheckVictoryCondition();

	if (GS)
	{
		GS->GameElapsedTime = TotalGameTime;
	}
}

void AProject_GemCoopGameMode::OnWaveClearedCallback(int32 WaveNumber)
{
	CurrentWave = WaveNumber;
	StartNextWave();
}

void AProject_GemCoopGameMode::OnBossSpawnedCallback(AActor* BossActor, int32 BossIndex)
{
	CurrentBossIndex = BossIndex;

	if (auto* Boss = Cast<AProject_GemCoopMonsterCharacter>(BossActor))
	{
		Boss->OnMonsterDied.AddDynamic(this, &AProject_GemCoopGameMode::OnBossDefeatedCallback);
	}
}

void AProject_GemCoopGameMode::OnBossDefeatedCallback(AProject_GemCoopMonsterCharacter* Monster)
{
	CoopScore = FMath::Min(CoopScore + 10.f, 100.f);
	OnBossDefeated.Broadcast(CurrentBossIndex);
}

void AProject_GemCoopGameMode::CheckVictoryCondition()
{
	const float VictoryTime = 900.f;
	if (TotalGameTime >= VictoryTime)
	{
		EndGame(true);
	}
}

int32 AProject_GemCoopGameMode::AlivePlayerCount() const
{
	TArray<AActor*> Chars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProject_GemCoopCharacter::StaticClass(), Chars);
	int32 Count = 0;

	for (AActor* A : Chars)
	{
		auto* C = Cast<AProject_GemCoopCharacter>(A);
		if (C && C->bIsAlive) Count++;
	}

	return Count;
}
