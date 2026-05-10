// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Project_GemCoopTypes.generated.h"

/**
 *
 */

class AProject_GemCoopMonsterCharacter;

UENUM(BlueprintType)
enum class EGemType : uint8
{
    None		UMETA(DisplayName = "None"),
    Ruby		UMETA(DisplayName = "Ruby"),
    Sapphire	UMETA(DisplayName = "Sapphire"),
    Topaz		UMETA(DisplayName = "Topaz"),
    Emerald		UMETA(DisplayName = "Emerald"),
    Amethyst	UMETA(DisplayName = "Amethyst"),
};

UENUM(BlueprintType)
enum class EGemGrade : uint8
{
    Common		UMETA(DisplayName = "Common"),
    Flawless	UMETA(DisplayName = "Flawless"),
    Star		UMETA(DisplayName = "Star"),
    Prismatic	UMETA(DisplayName = "Prismatic"),
};

UENUM(BlueprintType)
enum class EGemTrait : uint8
{
    None		UMETA(DisplayName = "None"),
    Offense		UMETA(DisplayName = "Offense"),
    Defense		UMETA(DisplayName = "Defense"),
    Support		UMETA(DisplayName = "Support"),
};

UENUM(BlueprintType)
enum class EArenaZone : uint8
{
    Safe		UMETA(DisplayName = "Safe"),
    Combat		UMETA(DisplayName = "Combat"),
    Danger		UMETA(DisplayName = "Danger"),
};

UENUM(BlueprintType)
enum class EFormationRole : uint8
{
    Tanker		UMETA(DisplayName = "Tanker"),
    Dealer		UMETA(DisplayName = "Dealer"),
    Support		UMETA(DisplayName = "Support"),
};

UENUM(BlueprintType)
enum class EMonsterType : uint8
{
    Goblin		UMETA(DisplayName = "Goblin"),
    Orc			UMETA(DisplayName = "Orc"),
    Skeleton	UMETA(DisplayName = "Skeleton"),
    Bat			UMETA(DisplayName = "Bat"),
    Elite		UMETA(DisplayName = "Elite"),
    MiniBoss    UMETA(DisplayName = "MiniBoss"),
    Boss		UMETA(DisplayName = "Boss"),
};
//-----
UENUM(BlueprintType)
enum class ECoopGrade : uint8
{
    F		    UMETA(DisplayName = "F"),
    D			UMETA(DisplayName = "D"),
    C	        UMETA(DisplayName = "C"),
    B			UMETA(DisplayName = "B"),
    A		    UMETA(DisplayName = "A"),
    S           UMETA(DisplayName = "S"),
};

UENUM(BlueprintType)
enum class EWaveState : uint8
{
    Idle		UMETA(DisplayName = "Idle"),
    Preparing	UMETA(DisplayName = "Preparing"),
    InProgress	UMETA(DisplayName = "In Progress"),
    Cleared		UMETA(DisplayName = "Cleared"),
    GameOver	UMETA(DisplayName = "Game Over")
};

UENUM(BlueprintType)
enum class EFusionConditionType : uint8
{
    None			UMETA(DisplayName = "None"),

    Normal			UMETA(DisplayName = "Normal"),
    Contribution	UMETA(DisplayName = "Contribution"),
    Formation		UMETA(DisplayName = "Formation"),
    LowHealth		UMETA(DisplayName = "Low Health")
};

UENUM(BlueprintType)
enum class EStatEventType : uint8
{
    Damage	UMETA(DisplayName = "Damage"),
    Heal	UMETA(DisplayName = "Heal"),
    Buff	UMETA(DisplayName = "Buff")
};
//-----
USTRUCT(Atomic, BlueprintType)
struct FGemData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName GemID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGemType GemType = EGemType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGemGrade Grade = EGemGrade::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EnergyCost = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Cooldown = 3.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EffectValue = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;
};

USTRUCT(Atomic, BlueprintType)
struct FGemCodexEntry
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName GemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCollected = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGemGrade HighestGrade = EGemGrade::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CollectCount = 0;
};

USTRUCT(Atomic, BlueprintType)
struct FBuffData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName BuffID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ATKBonus = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DEFBonus = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SPDBonus = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HealBonus = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 StackCount = 1;
};

USTRUCT(Atomic, BlueprintType)
struct FDebuffData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName DebuffID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamagePerSec = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DEFReduction = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SPDReduction = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 3.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 StackCount = 1;
};

USTRUCT(Atomic, BlueprintType)
struct FGemSlot
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SlotIndex = -1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGemData GemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CooldownRemaining = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bFusionReady = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bLocked = false;
};

//USTRUCT(Atomic, BlueprintType)
//struct FGradeDropRate : public FTableRowBase
//{
//    GENERATED_BODY()
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    float CommonRate = 70.0f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    float FlawlessRate = 25.0f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    float StarRate = 4.0f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    float PrismaticRate = 1.0f;
//};

USTRUCT(Atomic, BlueprintType)
struct FStatModifier
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ATKMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DEFMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HPMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SPDMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HealMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CritBonus = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EnergyRegenMultiplier = 1.f;
};

USTRUCT(Atomic, BlueprintType)
struct FTraitData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGemTrait Trait = EGemTrait::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FStatModifier StatModifier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<EGemType> PreferredGems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor TraitColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;
};

USTRUCT(Atomic, BlueprintType)
struct FContributionRatio
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageRatio = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HealRatio = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BuffRatio = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EStatEventType LowestContribution = EStatEventType::Damage;
};

USTRUCT(Atomic, BlueprintType)
struct FGemFusionRequest
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PlayerIndex = -1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SlotIndex = -1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGemData Gem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RequestTime = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* RequestOwner = nullptr;
};

USTRUCT(Atomic, BlueprintType)
struct FFusionResult : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName FusionID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<EGemType> RequiredGemTypes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredGemCount = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EffectMultiplier = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EnergyBonusCost = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Cooldown = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSuccess = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsThreshold = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EFusionConditionType ConditionType = EFusionConditionType::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText tDescription;
};

USTRUCT(Atomic, BlueprintType)
struct FGemFusionPreview
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<EGemType> PendingGems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FFusionResult PredictedResult;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ParticipantCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RemainingTime = 0.f;
};

USTRUCT(Atomic, BlueprintType)
struct FMonsterData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EMonsterType MonsterType = EMonsterType::Goblin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseHP = 30.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseATK = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MoveSpeed = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGemType WeaknessGem = EGemType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGemGrade DropGrade = EGemGrade::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GoldDrop = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AProject_GemCoopMonsterCharacter> MonsterClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;
};

USTRUCT(BlueprintType)
struct FPlayerCoopStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PlayerIndex = -1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageDealt = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HealGiven = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BuffsApplied = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 FusionCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RescueCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 DeathCount = 0;
};

USTRUCT(Atomic, BlueprintType)
struct FDamageInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageAmount = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGemType DamageType = EGemType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* DamageInstigator = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* DamageTarget = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCritical = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bWeakness = false;
};

USTRUCT(BlueprintType)
struct FStatEvent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeStamp = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageVal = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HealVal = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BuffCount = 0;
};

USTRUCT(Atomic, BlueprintType)
struct FWaveSpawnEntry : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 WaveNumber = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EMonsterType MonsterType = EMonsterType::Goblin;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AProject_GemCoopMonsterCharacter> MonsterClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BaseCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnDelay = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnInterval = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsBossWave = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BossIndex = 0;
};

USTRUCT(BlueprintType)
struct FSaveGemEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName GemID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGemType GemType = EGemType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGemGrade Grade = EGemGrade::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count = 1;
};

USTRUCT(BlueprintType)
struct FAchievementData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AchievementID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUnlocked = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GoldReward = 0;
};

USTRUCT(BlueprintType)
struct FUpgradeData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName UpgradeID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxLevel = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BaseCost = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ValuePerLevel = 0.1f;
};

USTRUCT(BlueprintType)
struct FZoneEffectData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EArenaZone Zone = EArenaZone::Combat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DEFMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EnergyRegenMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float GemDropRateMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bMonsterBlocked = false;
};

class PROJECT_GEMCOOP_API Project_GemCoopTypes
{
public:
    Project_GemCoopTypes();
    ~Project_GemCoopTypes();
};
