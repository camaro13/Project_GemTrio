// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopGameInstance.h"
#include "Project_GemCoopSaveGame.h"
#include "Project_GemCoopCharacter.h"
#include "Project_GemCoopStatComponent.h"
#include "Project_GemCoopGemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UProject_GemCoopGameInstance::Init()
{
	Super::Init();

	LoadGameData();
}

void UProject_GemCoopGameInstance::LoadGameData()
{
	const FString SlotName = TEXT("Project_GemCoopSave_Slot0");

	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		SaveData = Cast<UProject_GemCoopSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	}
	else
	{
		SaveData = Cast<UProject_GemCoopSaveGame>(UGameplayStatics::CreateSaveGameObject(UProject_GemCoopSaveGame::StaticClass()));
	}

	if (!SaveData)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadGameData failed. SaveData is null."));
		return;
	}

	TotalGoldCurrency = SaveData->TotalGold;
	PermanentUpgrades = SaveData->UpgradeLevels;
	GemCodexData = SaveData->GemCodex;
	AchievementData = SaveData->Achievements;
	LocalPlayerTrait = SaveData->PlayerTrait;

	LoadGemInventoryFromSaveData();

	UE_LOG(LogTemp, Warning, TEXT("Game loaded. Slot=%s"), *SaveData->SaveSlotName);
}

void UProject_GemCoopGameInstance::SaveGameToSlot()
{
	if (!SaveData)
	{
		SaveData = Cast<UProject_GemCoopSaveGame>(UGameplayStatics::CreateSaveGameObject(UProject_GemCoopSaveGame::StaticClass()));
	}

	if (!SaveData)
	{
		return;
	}

	SaveData->TotalGold = TotalGoldCurrency;
	SaveData->UpgradeLevels = PermanentUpgrades;
	SaveData->GemCodex = GemCodexData;
	SaveData->Achievements = AchievementData;
	SaveData->PlayerTrait = LocalPlayerTrait;
	SaveData->LastSelectedTrait = LocalPlayerTrait;

	SaveGemInventoryToSaveData();

	UGameplayStatics::SaveGameToSlot(SaveData, SaveData->SaveSlotName, 0);

	UE_LOG(LogTemp, Warning, TEXT("Game saved. Slot=%s"), *SaveData->SaveSlotName);
}

void UProject_GemCoopGameInstance::SaveGameResult(FGameResult Result)
{
	LastGameResult = Result;

	int32 GoldReward = 0;
	if (Result.CoopScore >= 90.f)
	{
		GoldReward = 1000;
	}
	else if (Result.CoopScore >= 75.f)
	{
		GoldReward = 600;
	}
	else if (Result.CoopScore >= 55.f)
	{
		GoldReward = 300;
	}
	else
	{
		GoldReward = 100;
	}

	GoldReward += Result.ReachedWave * 20;

	AddGold(GoldReward);

	if (Result.UpgradePointReward > 0)
	{
		AddGold(Result.UpgradePointReward * 200);
	}

	CheckAchievements(Result);
	SaveGameToSlot();
}

void UProject_GemCoopGameInstance::CheckAchievements(const FGameResult& Result)
{
	if (Result.bVictory)
	{
		UnlockAchievement(FName("ACH_Survive15Min"));
	}

	int32 TotalFusions = GetSaveData()->TotalFusionSuccess + Result.FusionSuccessCount;

	if (TotalFusions >= 50)
	{
		UnlockAchievement(FName("ACH_FusionMaster"));
	}

	if (Result.CoopScore >= 90.f)
	{
		UnlockAchievement(FName("ACH_CoopGradeS"));
	}
}

bool UProject_GemCoopGameInstance::ApplyUpgrade(FName UpgradeID)
{
	int32 CurrentLevel = GetUpgradeLevel(UpgradeID);
	int32 MaxLevel = GetUpgradeMaxLevel(UpgradeID);

	if (CurrentLevel >= MaxLevel)
	{
		return false;
	}

	int32 Cost = GetUpgradeCost(UpgradeID);
	
	if (!SpendGold(Cost))
	{
		return false;
	}

	PermanentUpgrades.FindOrAdd(UpgradeID)++;

	TArray<AActor*> Chars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProject_GemCoopCharacter::StaticClass(), Chars);

	for (AActor* A : Chars)
	{
		auto* C = Cast<AProject_GemCoopCharacter>(A);

		if (!C || !C->IsLocallyControlled())
		{
			continue;
		}

		C->StatCompRef->ApplyPermanentUpgrade(UpgradeID, 1);
	}

	OnUpgradeApplied.Broadcast(UpgradeID, PermanentUpgrades[UpgradeID]);
	SaveGameToSlot();

	return true;
}

int32 UProject_GemCoopGameInstance::GetUpgradeLevel(FName UpgradeID)
{
	int32* Level = PermanentUpgrades.Find(UpgradeID);
	
	return Level ? *Level : 0;
}

int32 UProject_GemCoopGameInstance::GetUpgradeCost(FName UpgradeID)
{
	static TMap<FName, int32> BaseCosts =
	{
		{"HP_Upgrade", 100},
		{"ATK_Upgrade", 100},
		{"DEF_Upgrade", 100},
		{"SPD_Upgrade", 80},
		{"CooldownReduction", 120},
		{"MaxEnergy_Upgrade", 150},
		{"Crit_Upgrade", 90},
		{"ReviveCount_Up", 200},
	};

	int32 Base = BaseCosts.Contains(UpgradeID) ? BaseCosts[UpgradeID] : 100;

	return Base * (GetUpgradeLevel(UpgradeID) + 1);
}

void UProject_GemCoopGameInstance::RegisterGemToCodex(FName GemID, FGemCodexEntry Entry)
{
	bool bIsNew = !GemCodexData.Contains(GemID);

	if (GemCodexData.Contains(GemID))
	{
		GemCodexData[GemID].CollectCount++;
	}
	else
	{
		Entry.CollectCount = 1;
		GemCodexData.Add(GemID, Entry);
	}

	if (bIsNew)
	{
		OnGemCodexUpdated.Broadcast(GemID);

		if (GemCodexData.Num() >= 20)
		{
			UnlockAchievement(FName("ACH_Codex20"));
		}
	}

	SaveGameToSlot();
}

bool UProject_GemCoopGameInstance::IsGemCollected(FName GemID) const
{
	return GemCodexData.Contains(GemID);
}

void UProject_GemCoopGameInstance::AddGold(int32 Amount)
{
	TotalGoldCurrency += Amount;
	if (SaveData)
	{
		SaveData->TotalGold = TotalGoldCurrency;
	}

	OnGoldChanged.Broadcast(TotalGoldCurrency);
	SaveGameToSlot();
}

bool UProject_GemCoopGameInstance::SpendGold(int32 Amount)
{
	if (TotalGoldCurrency < Amount)
	{
		return false;
	}

	TotalGoldCurrency -= Amount;

	if (SaveData)
	{
		SaveData->TotalGold = TotalGoldCurrency;
	}

	OnGoldChanged.Broadcast(TotalGoldCurrency);
	SaveGameToSlot();

	return true;
}

void UProject_GemCoopGameInstance::UnlockAchievement(FName AchID)
{
	if (AchievementData.FindOrAdd(AchID))
	{
		return;
	}

	OnAchievementUnlocked.Broadcast(AchID);
	SaveGameToSlot();
}

void UProject_GemCoopGameInstance::ApplyPermanentUpgradesToCharacter(AProject_GemCoopCharacter* Character)
{
	if (!Character || !Character->StatCompRef)
	{
		return;
	}

	for (auto& Pair : PermanentUpgrades)
	{
		for (int32 i = 0; i < Pair.Value; i++)
		{
			Character->StatCompRef->ApplyPermanentUpgrade(Pair.Key, 1);
		}
	}
}

void UProject_GemCoopGameInstance::AddGemToInventory(const FGemData& GemData, int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	if (GemData.GemID.IsNone() || GemData.GemType == EGemType::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddGemToInventory failed. Invalid GemData."));
		return;
	}

	int32 ExistingIndex = FindGemStackIndex(GemData.GemID);

	if (OwnedGemInventory.IsValidIndex(ExistingIndex))
	{
		OwnedGemInventory[ExistingIndex].Count += Amount;

		UE_LOG(LogTemp, Warning, TEXT("Gem inventory stacked. GemID=%s Count=%d"),
			*GemData.GemID.ToString(),
			OwnedGemInventory[ExistingIndex].Count
		);
	}
	else
	{
		FOwnedGemStack NewStack;
		NewStack.GemID = GemData.GemID;
		NewStack.GemType = GemData.GemType;
		NewStack.Grade = GemData.Grade;
		NewStack.Count = Amount;
		NewStack.GemData = GemData;

		OwnedGemInventory.Add(NewStack);

		UE_LOG(LogTemp, Warning, TEXT("Gem added to inventory. GemID=%s Count=%d"),
			*GemData.GemID.ToString(),
			Amount
		);
	}

	OnGemInventoryChanged.Broadcast();
}

bool UProject_GemCoopGameInstance::RemoveGemFromInventory(FName GemID, int32 Amount)
{
	if (GemID.IsNone() || Amount <= 0)
	{
		return false;
	}

	int32 Index = FindGemStackIndex(GemID);

	if (!OwnedGemInventory.IsValidIndex(Index))
	{
		return false;
	}

	if (OwnedGemInventory[Index].Count < Amount)
	{
		return false;
	}

	OwnedGemInventory[Index].Count -= Amount;

	if (OwnedGemInventory[Index].Count <= 0)
	{
		OwnedGemInventory.RemoveAt(Index);
	}

	OnGemInventoryChanged.Broadcast();

	UE_LOG(LogTemp, Warning, TEXT("Gem removed from inventory. GemID=%s Amount=%d"),
		*GemID.ToString(),
		Amount
	);

	return true;
}

int32 UProject_GemCoopGameInstance::GetGemCount(FName GemID) const
{
	if (GemID.IsNone())
	{
		return 0;
	}

	int32 Index = FindGemStackIndex(GemID);

	if (!OwnedGemInventory.IsValidIndex(Index))
	{
		return 0;
	}

	return OwnedGemInventory[Index].Count;
}

TArray<FOwnedGemStack> UProject_GemCoopGameInstance::GetGemInventory() const
{
	return OwnedGemInventory;
}

void UProject_GemCoopGameInstance::ClearGemInventory()
{
	OwnedGemInventory.Empty();

	OnGemInventoryChanged.Broadcast();

	UE_LOG(LogTemp, Warning, TEXT("Gem inventory cleared."));
}

void UProject_GemCoopGameInstance::SaveGemInventoryToSaveData()
{
	if (!SaveData)
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveGemInventoryToSaveData failed. SaveData is null."));
		return;
	}

	SaveData->SavedGemInventory.Empty();

	for (const FOwnedGemStack& Stack : OwnedGemInventory)
	{
		if (Stack.GemID.IsNone() || Stack.GemType == EGemType::None || Stack.Count <= 0)
		{
			continue;
		}

		FSaveOwnedGemEntry SaveEntry;

		SaveEntry.GemID = Stack.GemID;
		SaveEntry.GemType = Stack.GemType;
		SaveEntry.Grade = Stack.Grade;
		SaveEntry.Count = Stack.Count;

		SaveData->SavedGemInventory.Add(SaveEntry);
	}

	UE_LOG(LogTemp, Warning, TEXT("Gem inventory copied to SaveData. SavedCount=%d"),
		SaveData->SavedGemInventory.Num()
	);
}

void UProject_GemCoopGameInstance::LoadGemInventoryFromSaveData()
{
	OwnedGemInventory.Empty();

	if (!SaveData)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadGemInventoryFromSaveData failed. SaveData is null."));
		return;
	}

	UProject_GemCoopGemDataSubsystem* GemDataSubsystem = nullptr;

	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		GemDataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UProject_GemCoopGemDataSubsystem>();
	}

	for (const FSaveOwnedGemEntry& SaveEntry : SaveData->SavedGemInventory)
	{
		if (SaveEntry.GemID.IsNone() || SaveEntry.GemType == EGemType::None || SaveEntry.Count <= 0)
		{
			continue;
		}

		FGemData LoadedGemData;

		if (GemDataSubsystem)
		{
			LoadedGemData = GemDataSubsystem->GetGemData(SaveEntry.GemID);
		}

		if (LoadedGemData.GemID.IsNone() || LoadedGemData.GemType == EGemType::None)
		{
			LoadedGemData.GemID = SaveEntry.GemID;
			LoadedGemData.GemType = SaveEntry.GemType;
			LoadedGemData.Grade = SaveEntry.Grade;
			LoadedGemData.DisplayName = FText::FromName(SaveEntry.GemID);
		}

		FOwnedGemStack Stack;
		Stack.GemID = SaveEntry.GemID;
		Stack.GemType = SaveEntry.GemType;
		Stack.Grade = SaveEntry.Grade;
		Stack.Count = SaveEntry.Count;
		Stack.GemData = LoadedGemData;

		OwnedGemInventory.Add(Stack);
	}

	OnGemInventoryChanged.Broadcast();

	UE_LOG(LogTemp, Warning, TEXT("Gem inventory loaded from SaveData. Count=%d"),
		OwnedGemInventory.Num()
	);
}

void UProject_GemCoopGameInstance::DebugPrintGemInventory() const
{
	UE_LOG(LogTemp, Warning, TEXT("===== Gem Inventory ====="));

	if (OwnedGemInventory.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Empty"));
		return;
	}

	for (const FOwnedGemStack& Stack : OwnedGemInventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("GemID=%s Type=%d Grade=%d Count=%d"),
			*Stack.GemID.ToString(),
			static_cast<int32>(Stack.GemType),
			static_cast<int32>(Stack.Grade),
			Stack.Count
		);
	}
}

int32 UProject_GemCoopGameInstance::GetUpgradeMaxLevel(FName UpgradeID) const
{
	static TMap<FName, int32> MaxLevels = {{ "HP_Upgrade", 10 }, { "ATK_Upgrade", 10 }, { "DEF_Upgrade", 10 },{ "SPD_Upgrade", 10 }, { "CooldownReduction", 10 }, { "Crit_Upgrade", 10 },{ "MaxEnergy_Upgrade", 5 }, { "ReviveCount_Up", 3 },};
	const int32* Max = MaxLevels.Find(UpgradeID);
	
	return Max ? *Max : 10;
}

int32 UProject_GemCoopGameInstance::FindGemStackIndex(FName GemID) const
{
	for (int32 i = 0; i < OwnedGemInventory.Num(); ++i)
	{
		if (OwnedGemInventory[i].GemID == GemID)
		{
			return i;
		}
	}

	return INDEX_NONE;
}
