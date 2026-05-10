// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopGemDataSubsystem.h"

void UProject_GemCoopGemDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    AllGemTypes = { EGemType::Ruby, EGemType::Sapphire, EGemType::Emerald, EGemType::Amethyst, EGemType::Topaz };

    /*static ConstructorHelpers::FObjectFinder<UDataTable> GemTableObj(TEXT("/Game/Data/DT_GemData.DT_GemData"));

    if (GemTableObj.Succeeded())
    {
        GemDataTable = GemTableObj.Object;
    }

    static ConstructorHelpers::FObjectFinder<UDataTable> FusionTableObj(TEXT("/Game/Data/DT_FusionRecipe.DT_FusionRecipe"));

    if (FusionTableObj.Succeeded())
    {
        FusionRecipeTable = FusionTableObj.Object;
    }

    static ConstructorHelpers::FObjectFinder<UDataTable> GradeDropTableObj(TEXT("/Game/Data/DT_GradeDropRate.DT_GradeDropRate"));

    if (GradeDropTableObj.Succeeded())
    {
        GradeDropTable = GradeDropTableObj.Object;
    }*/

    GemDataTable = LoadObject<UDataTable>(
        nullptr,
        TEXT("/Game/GemBlast/Data/DT_GemData.DT_GemData")
    );

    FusionRecipeTable = LoadObject<UDataTable>(
        nullptr,
        TEXT("/Game/GemBlast/Data/DT_FusionRecipe.DT_FusionRecipe")
    );

    GradeDropTable = LoadObject<UDataTable>(
        nullptr,
        TEXT("/Game/GemBlast/Data/DT_GradeDropRate.DT_GradeDropRate")
    );

    if (!GemDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("GemDataSubSystem: DT_GemData not found."));
    }

    if (!FusionRecipeTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("GemDataSubSystem: DT_FusionRecipe not found."));
    }

    if (!GradeDropTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("GemDataSubSystem: DT_GradeDropRate not found."));
    }
}

FName UProject_GemCoopGemDataSubsystem::GetEnumCleanName(EGemType GemType) const
{
    FString EnumString = UEnum::GetValueAsString(GemType);
    EnumString.RemoveFromStart(TEXT("EGemType::"));
    return FName(*EnumString);
}

FName UProject_GemCoopGemDataSubsystem::GetEnumCleanName(EGemGrade Grade) const
{
    FString EnumString = UEnum::GetValueAsString(Grade);
    EnumString.RemoveFromStart(TEXT("EGemGrade::"));
    return FName(*EnumString);
}

FName UProject_GemCoopGemDataSubsystem::GetEnumCleanName(EMonsterType MonsterType) const
{
    FString EnumString = UEnum::GetValueAsString(MonsterType);
    EnumString.RemoveFromStart(TEXT("EMonsterType"));
    return FName(*EnumString);
}

void UProject_GemCoopGemDataSubsystem::SortGemTypes(TArray<EGemType>& GemTypes) const
{
    GemTypes.Sort([](const EGemType& A, const EGemType& B)
        {
            return static_cast<uint8>(A) < static_cast<uint8>(B);
        });
}

bool UProject_GemCoopGemDataSubsystem::AreSameGemTypeSet(const TArray<EGemType>& A, const TArray<EGemType>& B) const
{
    if (A.Num() != B.Num())
    {
        return false;
    }

    for (int32 i = 0; i < A.Num(); ++i)
    {
        if (A[i] != B[i])
        {
            return false;
        }
    }

    return true;
}

FGemData UProject_GemCoopGemDataSubsystem::GetGemData(FName GemID) const
{
    if (!GemDataTable || GemID.IsNone())
    {
        return FGemData{};
    }

    const FString ContextString = TEXT("GetGemData");
    const FGemData* Row = GemDataTable->FindRow<FGemData>(GemID, ContextString);
    
    if (!Row)
    {
        UE_LOG(LogTemp, Warning, TEXT("GemDataSubsystem: Gem row not found: %s"), *GemID.ToString());
        return FGemData{};
    }

    return *Row;
}

FFusionResult UProject_GemCoopGemDataSubsystem::LookupFusion(TArray<EGemType> GemTypes)
{
    if (!FusionRecipeTable)
    {
        return FFusionResult{};
    }

    GemTypes.Remove(EGemType::None);
    SortGemTypes(GemTypes);
    
    if (GemTypes.Num() < 2)
    {
        return FFusionResult{};
    }
    
    TArray<FFusionResult*> Rows;
    const FString ContextString = TEXT("LookupFusion");
    FusionRecipeTable->GetAllRows<FFusionResult>(ContextString, Rows);

    for (FFusionResult* Row : Rows)
    {
        if (!Row)
        {
            continue;
        }

        TArray<EGemType> RequiredTypes = Row->RequiredGemTypes;
        RequiredTypes.Remove(EGemType::None);
        SortGemTypes(RequiredTypes);

        if (AreSameGemTypeSet(GemTypes, RequiredTypes))
        {
            FFusionResult Result = *Row;
            Result.bSuccess = true;
            return Result;
        }
    }

    return FFusionResult{};
}

bool UProject_GemCoopGemDataSubsystem::IsValidFusion(TArray<EGemType> GemTypes)
{
    const FFusionResult Result = LookupFusion(GemTypes);

    return Result.bSuccess || Result.bIsThreshold;
}
//여기부터
EGemGrade UProject_GemCoopGemDataSubsystem::RollGemGrade(EMonsterType MonsterType)
{
    if (!GradeDropTable)
    {
        return EGemGrade::Common;
    }

    FName RowName = GetEnumCleanName(MonsterType);
    FString ContextString = TEXT("RollGemGrade");
    FGradeDropRate* Row = GradeDropTable->FindRow<FGradeDropRate>(RowName, ContextString);

    if (!Row)
    {
        return EGemGrade::Common;
    }

    float Roll = FMath::FRandRange(0.f, 100.f);

    if (Roll < Row->CommonRate)
    {
        return EGemGrade::Common;
    }

    Roll -= Row->CommonRate;

    if (Roll < Row->FlawlessRate)
    {
        return EGemGrade::Flawless;
    }

    Roll -= Row->FlawlessRate;

    if (Roll < Row->StarRate)
    {
        return EGemGrade::Star;
    }

    return EGemGrade::Prismatic;
}

FGemData UProject_GemCoopGemDataSubsystem::GenerateRandomGem(EGemGrade MinGrade)
{
    if (AllGemTypes.Num() <= 0)
    {
        return FGemData{};
    }

    int32 RandIdx = FMath::RandRange(0, AllGemTypes.Num() - 1);
    EGemType RandomType = AllGemTypes[RandIdx];

    FName TypeName = GetEnumCleanName(RandomType);
    FName GradeName = GetEnumCleanName(MinGrade);

    FName GemID(*FString::Printf(TEXT("%s_%s"), *TypeName.ToString(), *GradeName.ToString()));

    return GetGemData(GemID);
}

FGemData UProject_GemCoopGemDataSubsystem::PolishGem(TArray<FGemData> Materials)
{
    if (Materials.Num() < 3)
    {
        return FGemData();
    }

    EGemGrade InputGrade = Materials[0].Grade;

    for (FGemData& Material : Materials)
    {
        if (Material.Grade != InputGrade)
        {
            return FGemData{};
        }
    }

    EGemGrade OutputGrade = EGemGrade::Common;

    switch (InputGrade)
    {
    case EGemGrade::Common:
        OutputGrade = EGemGrade::Flawless;
        break;

    case EGemGrade::Flawless:
        OutputGrade = EGemGrade::Star;
        break;

    case EGemGrade::Star:
        OutputGrade = EGemGrade::Prismatic;
        break;

    case EGemGrade::Prismatic:
    default:
        return FGemData{};
    }

    return GenerateRandomGem(OutputGrade);
}