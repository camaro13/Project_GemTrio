// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopGemComponent.h"
#include "Project_GemCoopCharacter.h"
#include "Project_GemCoopMonsterCharacter.h"
#include "Project_GemCoopCombatComponent.h"
#include "Project_GemCoopEnergySYComponent.h"
#include "Project_GemCoopFusionSYComponent.h"
#include "Project_GemCoopStatComponent.h"
#include "Project_GemCoopBuffComponent.h"
#include "Project_GemCoopGemDataSubsystem.h"
#include "Project_GemCoopGameInstance.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UProject_GemCoopGemComponent::UProject_GemCoopGemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	MaxSlots = 3;

	GemSlots.SetNum(MaxSlots);
	SlotCooldowns.Init(0.0f, MaxSlots);
	bFusionReady.Init(false, MaxSlots);

	PendingFusionSlot = -1;
	PendingUseSlot = -1;

	CoolDownReduction = 0.0f;
	FusionCoolDownBonus = 0.1f;
	DefaultGemCastTime = 0.5f;
	GemTargetRange = 2500.f;
	bDebugLog = true;

	bUseGemDataTable = true;

	DefaultSlotGemIDs.Empty();
	DefaultSlotGemIDs.Add(TEXT("Ruby_Common"));
	DefaultSlotGemIDs.Add(TEXT("Sapphire_Common"));
	DefaultSlotGemIDs.Add(TEXT("Emerald_Common"));
}

// Called when the game starts
void UProject_GemCoopGemComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AProject_GemCoopCharacter>(GetOwner());

	CombatComp = GetOwner() ? GetOwner()->FindComponentByClass<UProject_GemCoopCombatComponent>() : nullptr;
	EnergySYComp = GetOwner() ? GetOwner()->FindComponentByClass<UProject_GemCoopEnergySYComponent>() : nullptr;
	FusionSYComp = GetOwner() ? GetOwner()->FindComponentByClass<UProject_GemCoopFusionSYComponent>() : nullptr;
	StatComp = GetOwner() ? GetOwner()->FindComponentByClass<UProject_GemCoopStatComponent>() : nullptr;
	BuffComp = GetOwner() ? GetOwner()->FindComponentByClass<UProject_GemCoopBuffComponent>() : nullptr;

	const int32 DesiredSlotCount = FMath::Max(3, GemSlots.Num());

	if (SlotCooldowns.Num() != DesiredSlotCount)
	{
		SlotCooldowns.SetNum(DesiredSlotCount);
	}

	if (SlotCooldownEndTimes.Num() != DesiredSlotCount)
	{
		SlotCooldownEndTimes.SetNum(DesiredSlotCount);
	}

	for (int32 i = 0; i < SlotCooldowns.Num(); ++i)
	{
		SlotCooldowns[i] = 0.0f;
	}

	for (int32 i = 0; i < SlotCooldownEndTimes.Num(); ++i)
	{
		SlotCooldownEndTimes[i] = 0.0f;
	}

	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		GemDataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UProject_GemCoopGemDataSubsystem>();
	}

	if (OwnerCharacter)
	{
		PlayerIndex = OwnerCharacter->TeamIndex;
	}

	if (CombatComp)
	{
		CombatComp->OnCastingCompleted.AddDynamic(this, &UProject_GemCoopGemComponent::OnCastingCompleted);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GemComponent BeginPlay: CombatComp missing."));
	}
	
	InitializeGemSlots();

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("GemComponent BeginPlay. Slots=%d"), GemSlots.Num());
	}
}

// Called every frame
void UProject_GemCoopGemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	const int32 DesiredSlotCount = FMath::Max(3, GemSlots.Num());

	if (SlotCooldowns.Num() != DesiredSlotCount)
	{
		SlotCooldowns.SetNum(DesiredSlotCount);
	}

	for (int32 i = 0; i < SlotCooldowns.Num(); ++i)
	{
		SlotCooldowns[i] = GetSlotCooldownRemaining(i);
	}

	//TickCooldowns(DeltaTime);
	// ...
}

//bool UProject_GemCoopGemComponent::UseGem(int32 SlotIndex)
//{
//	AActor* OwnerActor = GetOwner();
//
//	if (!OwnerActor)
//	{
//		return false;
//	}
//
//	if (!OwnerActor->HasAuthority())
//	{
//		return false;
//	}
//
//	return UseGem_Internal(SlotIndex);
//	/*if (SlotIndex < 0 || SlotIndex >= MaxSlots)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("UseGem failed. Invalid slot: %d"), SlotIndex);
//		return false;
//	}
//
//	if (!GemSlots.IsValidIndex(SlotIndex))
//	{
//		return false;
//	}
//
//	if (!IsSlotReady(SlotIndex))
//	{
//		if (bDebugLog)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("UseGem failed. Slot not ready: %d"), SlotIndex);
//		}
//		return false;
//	}
//
//	if (!CombatComp)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("UseGem failed. CombatComp missing."));
//		return false;
//	}
//
//	if (CombatComp->bIsCasting)
//	{
//		if (bDebugLog)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("UseGem failed. Already casting."));
//		}
//		return false;
//	}
//
//	if (!EnergySYComp)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("UseGem failed. EnergySYComp missing."));
//		return false;
//	}
//
//	FGemData& Gem = GemSlots[SlotIndex];
//
//	if (Gem.GemType == EGemType::None)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("UseGem failed. Empty slot: %d"), SlotIndex);
//		return false;
//	}
//
//	EnergySYComp->SyncWithGameState();
//
//	if (!EnergySYComp->HasEnoughEnergy(Gem.EnergyCost))
//	{
//		EnergySYComp->OnEnergyInsufficient.Broadcast(Gem.EnergyCost, EnergySYComp->CachedSharedEnergy);
//
//		UE_LOG(LogTemp, Warning, TEXT("UseGem failed. Not enough energy. Cost=%.1f Current=%.1f"),
//			Gem.EnergyCost,
//			EnergySYComp->CachedSharedEnergy
//		);
//
//		return false;
//	}
//
//	PendingUseSlot = SlotIndex;
//
//	CombatComp->StartCasting(DefaultGemCastTime);
//
//	if (bDebugLog)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("UseGem started casting. Slot=%d Gem=%s Cost=%.1f"),
//			SlotIndex,
//			*Gem.GemID.ToString(),
//			Gem.EnergyCost
//		);
//	}
//
//	return true;*/
//}

//void UProject_GemCoopGemComponent::OnCastingCompleted()
//{
//	if (!GemSlots.IsValidIndex(PendingUseSlot))
//	{
//		PendingUseSlot = -1;
//		return;	
//	}
//
//	if (!EnergySYComp)
//	{
//		PendingUseSlot = -1;
//		return;
//	}
//
//	FGemData Gem = GemSlots[PendingUseSlot];
//
//	if (!EnergySYComp->TryConsumeEnergy(Gem.EnergyCost))
//	{
//		if (bDebugLog)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Casting completed but energy consume failed."));
//		}
//
//		PendingUseSlot = -1;
//		return;
//	}
//
//	ApplyGemEffect(PendingUseSlot);
//	StartCooldown(PendingUseSlot);
//
//	OnGemUsed.Broadcast(PendingUseSlot, GemSlots[PendingUseSlot]);
//
//	if (bDebugLog)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Gem Used. Slot=%d Gem=%s"),
//			PendingUseSlot,
//			*GemSlots[PendingUseSlot].GemID.ToString()
//		);
//	}
//
//	PendingUseSlot = -1;
//}

void UProject_GemCoopGemComponent::TickCooldowns(float DeltaTime)
{
	for (int32 i = 0; i < SlotCooldowns.Num(); ++i)
	{
		if (SlotCooldowns[i] > 0.0f)
		{
			SlotCooldowns[i] = FMath::Max(0.0f, SlotCooldowns[i] - DeltaTime);
			OnCoolDownChanged.Broadcast(i, SlotCooldowns[i]);
		}
	}
}

bool UProject_GemCoopGemComponent::IsSlotReady(int32 SlotIndex) const
{
	return GemSlots.IsValidIndex(SlotIndex) && SlotCooldowns.IsValidIndex(SlotIndex) && bFusionReady.IsValidIndex(SlotIndex) && SlotCooldowns[SlotIndex] <= 0.0f && !bFusionReady[SlotIndex];
}

FGemData UProject_GemCoopGemComponent::GetGemData(int32 SlotIndex) const
{
	return GemSlots.IsValidIndex(SlotIndex) ? GemSlots[SlotIndex] : FGemData();
}

void UProject_GemCoopGemComponent::SetGemSlot(int32 SlotIndex, FGemData NewGem)
{
	if (!GemSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	GemSlots[SlotIndex] = NewGem;

	if (SlotCooldowns.IsValidIndex(SlotIndex))
	{
		SlotCooldowns[SlotIndex] = 0.0f;
	}

	if (bFusionReady.IsValidIndex(SlotIndex))
	{
		bFusionReady[SlotIndex] = false;
	}

	OnSlotRefilled.Broadcast(SlotIndex, NewGem);
}

//void UProject_GemCoopGemComponent::StartCooldown(int32 SlotIndex)
//{
//	if (!GemSlots.IsValidIndex(SlotIndex) || !SlotCooldowns.IsValidIndex(SlotIndex))
//	{
//		return;
//	}
//
//	float Cooldown = FMath::Max(0.0f, GemSlots[SlotIndex].Cooldown * (1.f - CoolDownReduction));
//
//	SlotCooldowns[SlotIndex] = Cooldown;
//
//	OnCoolDownChanged.Broadcast(SlotIndex, Cooldown);
//}

//void UProject_GemCoopGemComponent::ApplyGemEffect(int32 SlotIndex)
//{
//	if (!GetOwner() || !GetOwner()->HasAuthority())
//	{
//		return;
//	}
//	
//	if (!GemSlots.IsValidIndex(SlotIndex))
//	{
//		return;
//	}
//
//	if (!OwnerCharacter)
//	{
//		return;
//	}
//
//	FGemData& Gem = GemSlots[SlotIndex];
//
//	if (!StatComp)
//	{
//		return;
//	}
//
//	const float DefaultBuffDuration = 5.f;
//
//	float EffectVal = FMath::Max(0.0f, Gem.EffectValue);
//
//	switch (Gem.GemType)
//	{
//	case EGemType::Ruby:
//	{
//		AActor* Target = FindTargetInFront();
//
//		if (Target && StatComp)
//		{
//			float Damage = StatComp->FinalATK * EffectVal;
//			ApplyDamageToTarget(Target, Damage, EGemType::Ruby);
//
//			if (UProject_GemCoopBuffComponent* TargetBuff = Target->FindComponentByClass<UProject_GemCoopBuffComponent>())
//			{
//				FDebuffData Burn;
//				Burn.DebuffID = TEXT("Burn");
//				Burn.DisplayName = FText::FromString(TEXT("Burn"));
//				Burn.DamagePerSec = Damage * 0.1f;
//				Burn.Duration = 3.f;
//				TargetBuff->AddDebuff(Burn);
//			}
//		}
//		break;
//		/*const float DamageAmount = StatComp->FinalATK * Gem.EffectValue;
//		const float AttackRange = 1200.f;
//
//		const FVector Start = OwnerCharacter->GetActorLocation() + FVector(0.0f, 0.0f, 60.f);
//		const FVector End = Start + OwnerCharacter->GetActorForwardVector() * AttackRange;
//
//		FHitResult HitResult;
//
//		FCollisionQueryParams Params;
//		Params.AddIgnoredActor(OwnerCharacter);
//
//		const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Pawn, Params);
//
//		if (bHit && HitResult.GetActor())
//		{
//			UGameplayStatics::ApplyDamage(HitResult.GetActor(), DamageAmount, OwnerCharacter->GetController(), OwnerCharacter, UDamageType::StaticClass());
//		}
//		
//		break;*/
//	}
//
//	case EGemType::Sapphire:
//	{
//		if (BuffComp && StatComp)
//		{
//			float ShieldAmount = StatComp->FinalDEF * EffectVal;
//			BuffComp->ApplyShield(ShieldAmount);
//
//			FBuffData DefBuff;
//			DefBuff.BuffID = TEXT("SapphireDEF");
//			DefBuff.DisplayName = FText::FromString(TEXT("Sapphire DEF"));
//			DefBuff.DEFBonus = 0.5f * EffectVal;
//			DefBuff.Duration = 5.f;
//			BuffComp->AddBuff(DefBuff);
//		}
//		break;
//	}
//
//	case EGemType::Emerald:
//	{
//		if (StatComp)
//		{
//			float HealAmount = StatComp->FinalHP * 0.3f * EffectVal;
//			StatComp->ApplyHeal(HealAmount);
//
//			if (CombatComp)
//			{
//				CombatComp->RecordHeal(HealAmount);
//			}
//		}
//
//		if (BuffComp)
//		{
//			FBuffData RegenBuff;
//			RegenBuff.BuffID = TEXT("EmeraldRegen");
//			RegenBuff.DisplayName = FText::FromString(TEXT("Emerald Regen"));
//			RegenBuff.HealBonus = 0.1f;
//			RegenBuff.Duration = 5.0f;
//			BuffComp->AddBuff(RegenBuff);
//		}
//		break;
//	}
//
//	case EGemType::Topaz:
//	{
//		if (BuffComp)
//		{
//			FBuffData SpdBuff;
//			SpdBuff.BuffID = TEXT("TopazSPD");
//			SpdBuff.DisplayName = FText::FromString(TEXT("Topaz SPD"));
//			SpdBuff.SPDBonus = 0.5f * EffectVal;
//			SpdBuff.Duration = 2.f;
//			BuffComp->AddBuff(SpdBuff);
//		}
//
//		if (CombatComp)
//		{
//			CombatComp->RecordBuff();
//		}
//		break;
//	}
//
//	case EGemType::Amethyst:
//	{
//		if (BuffComp)
//		{
//			FBuffData AllBuff;
//			AllBuff.BuffID = TEXT("AmethystAll");
//			AllBuff.DisplayName = FText::FromString(TEXT("Amethyst All"));
//			AllBuff.ATKBonus = 0.25f * EffectVal;
//			AllBuff.DEFBonus = 0.25f * EffectVal;
//			AllBuff.SPDBonus = 0.25f * EffectVal;
//			AllBuff.HealBonus = 0.25f * EffectVal;
//			AllBuff.Duration = 10.f;
//			BuffComp->AddBuff(AllBuff);
//		}
//
//		if (CombatComp)
//		{
//			CombatComp->RecordBuff();
//		}
//		break;
//	}
//
//	default:
//		break;
//	}
//}

AActor* UProject_GemCoopGemComponent::FindTargetInFront() const
{
	if (!OwnerCharacter || !GetWorld())
	{
		return nullptr;
	}

	FVector Start = OwnerCharacter->GetActorLocation() + FVector(0.0f, 0.0f, 50.f);
	FVector End = Start + OwnerCharacter->GetActorForwardVector() * GemTargetRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);

	FHitResult Hit;

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params);

#if ENABLE_DRAW_DEBUG
	if (bDebugLog)
	{
		DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Red : FColor::White, false, 0.5f, 0, 2.f);
	}
#endif

	return bHit ? Hit.GetActor() : nullptr;
}

void UProject_GemCoopGemComponent::ApplyDamageToTarget(AActor* Target, float Damage, EGemType DamageType)
{
	if (!Target || Damage <= 0.0f)
	{
		return;
	}

	if (AProject_GemCoopMonsterCharacter* Monster = Cast<AProject_GemCoopMonsterCharacter>(Target))
	{
		Monster->LastHitGemType = DamageType;
	}

	UGameplayStatics::ApplyDamage(Target, Damage, OwnerCharacter ? OwnerCharacter->GetController() : nullptr, OwnerCharacter, nullptr);

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gem Damage Applied. Target=%s Damage=%.1f Type=%d"),
			*GetNameSafe(Target),
			Damage,
			static_cast<int32>(DamageType)
		);
	}
}

void UProject_GemCoopGemComponent::StartFusionAttempt(int32 SlotIndex)
{
	if (!GemSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	if (!IsSlotReady(SlotIndex))
	{
		return;
	}

	bFusionReady[SlotIndex] = true;
	PendingFusionSlot = SlotIndex;

	OnFusionAttempt.Broadcast(SlotIndex, GemSlots[SlotIndex]);

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fusion attempt marked. Slot=%d Gem=%s"),
			SlotIndex,
			*GemSlots[SlotIndex].GemID.ToString()
		);
	}
}

void UProject_GemCoopGemComponent::CompleteFusion(const TArray<FGemData>& FusionGems)
{
	if (!GemDataSubsystem)
	{
		return;
	}

	TArray<EGemType> Types;

	for (const FGemData& Gem : FusionGems)
	{
		Types.Add(Gem.GemType);
	}

	FFusionResult Result = GemDataSubsystem->LookupFusion(Types);

	if (Result.bSuccess || Result.bIsThreshold)
	{
		ApplyFusionEffect(Result);
	}

	for (int32 i = 0; i < GemSlots.Num(); ++i)
	{
		if (bFusionReady.IsValidIndex(i) && bFusionReady[i])
		{
			SlotCooldowns[i] = GemSlots[i].Cooldown * (1.f - FusionCoolDownBonus);
			bFusionReady[i] = false;
			OnCoolDownChanged.Broadcast(i, SlotCooldowns[i]);
		}
	}

	PendingFusionSlot = -1;
}

void UProject_GemCoopGemComponent::CancelFusion()
{
	for (int32 i = 0; i < bFusionReady.Num(); ++i)
	{
		bFusionReady[i] = false;
	}

	PendingFusionSlot = -1;
}

void UProject_GemCoopGemComponent::ApplyFusionEffect(const FFusionResult& Result)
{
	if (BuffComp)
	{
		FBuffData FusionBuff;
		FusionBuff.BuffID = Result.FusionID;
		FusionBuff.DisplayName = Result.DisplayName;
		FusionBuff.ATKBonus = 0.15f * Result.EffectMultiplier;
		FusionBuff.DEFBonus = 0.15f * Result.EffectMultiplier;
		FusionBuff.SPDBonus = 0.1f * Result.EffectMultiplier;
		FusionBuff.Duration = 5.f;
		BuffComp->AddBuff(FusionBuff);
	}

	if (CombatComp)
	{
		CombatComp->RecordBuff();
	}

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fusion Effect Applied: %s Multiplier=%.2f"),
			*Result.FusionID.ToString(),
			Result.EffectMultiplier
		);
	}
}

void UProject_GemCoopGemComponent::ExchangeGem(int32 SlotIndex, AProject_GemCoopCharacter* Partner)
{
	if (!Partner)
	{
		return;
	}

	if (!GemSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	UProject_GemCoopGemComponent* PartnerGemComp = Partner->FindComponentByClass<UProject_GemCoopGemComponent>();

	if (!PartnerGemComp)
	{
		return;
	}

	int32 PartnerSlot = PartnerGemComp->PendingFusionSlot;

	if (!PartnerGemComp->GemSlots.IsValidIndex(PartnerSlot))
	{
		return;
	}

	Swap(GemSlots[SlotIndex], PartnerGemComp->GemSlots[PartnerSlot]);

	SlotCooldowns[SlotIndex] = 0.0f;
	PartnerGemComp->SlotCooldowns[PartnerSlot] = 0.0f;

	OnGemExchanged.Broadcast(SlotIndex, PartnerSlot);
	PartnerGemComp->OnGemExchanged.Broadcast(PartnerSlot, SlotIndex);
}

void UProject_GemCoopGemComponent::RefillDefaultGemsFallback()
{
	MaxSlots = 3;

	GemSlots.SetNum(MaxSlots);
	SlotCooldowns.SetNum(MaxSlots);
	bFusionReady.SetNum(MaxSlots);

	for (int32 i = 0; i < MaxSlots; ++i)
	{
		SlotCooldowns[i] = 0.0f;
		bFusionReady[i] = false;
	}

	auto MakeGem = [](FName ID, EGemType Type, float Cost, float Cooldown, float EffectValue, const FString& Name)
		{
			FGemData Gem;
			Gem.GemID = ID;
			Gem.GemType = Type;
			Gem.Grade = EGemGrade::Common;
			Gem.EnergyCost = Cost;
			Gem.Cooldown = Cooldown;
			Gem.EffectValue = EffectValue;
			Gem.DisplayName = FText::FromString(Name);
			Gem.Description = FText::FromString(Name);
			return Gem;
		};

	GemSlots[0] = MakeGem(TEXT("Ruby_Common"), EGemType::Ruby, 15.f, 3.f, 1.5f, TEXT("Ruby"));
	GemSlots[1] = MakeGem(TEXT("Sapphire_Common"), EGemType::Sapphire, 10.f, 4.f, 1.5f, TEXT("Sapphire"));
	GemSlots[2] = MakeGem(TEXT("Emerald_Common"), EGemType::Emerald, 5.f, 5.f, 1.f, TEXT("Emerald"));

	for (int32 i = 0; i < MaxSlots; ++i)
	{
		OnSlotRefilled.Broadcast(i, GemSlots[i]);
	}

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("Default fallback gems loaded. SlotCount=%d"), MaxSlots);
	}
}

void UProject_GemCoopGemComponent::InitializeGemSlots()
{
	MaxSlots = 3;

	GemSlots.SetNum(MaxSlots);
	SlotCooldowns.SetNum(MaxSlots);
	bFusionReady.SetNum(MaxSlots);

	for (int32 i = 0; i < MaxSlots; ++i)
	{
		SlotCooldowns[i] = 0.0f;
		bFusionReady[i] = false;
	}

	if (bUseGemDataTable && GemDataSubsystem)
	{
		RefillDefaultGemsFromDataTable();
		return;
	}

	RefillDefaultGemsFallback();
}

bool UProject_GemCoopGemComponent::SetGemSlotByID(int32 SlotIndex, FName GemID)
{
	if (!GemSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetGemSlotByID failed. Invalid SlotIndex=%d"), SlotIndex);
		return false;
	}

	if (!GemDataSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetGemSlotByID failed. GemDataSubsystem missing."));
		return false;
	}
	
	FGemData GemData = GemDataSubsystem->GetGemData(GemID);

	if (GemData.GemType == EGemType::None || GemData.GemID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("SetGemSlotByID failed. Invalid GemID=%s"), *GemID.ToString());
		return false;
	}

	GemSlots[SlotIndex] = GemData;
	SlotCooldowns[SlotIndex] = 0.0f;
	bFusionReady[SlotIndex] = false;

	OnSlotRefilled.Broadcast(SlotIndex, GemData);

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gem slot set from DataTable. Slot=%d GemID=%s Type=%d Cost=%.1f Cooldown=%.1f"),
			SlotIndex,
			*GemData.GemID.ToString(),
			static_cast<int32>(GemData.GemType),
			GemData.EnergyCost,
			GemData.Cooldown
		);
	}

	return true;
}

void UProject_GemCoopGemComponent::RefillDefaultGemsFromDataTable()
{
	bool bAllLoaded = true;

	TArray<FName> SlotGemIDs = DefaultSlotGemIDs;

	if (GetWorld())
	{
		if (UProject_GemCoopGameInstance* GI = Cast<UProject_GemCoopGameInstance>(GetWorld()->GetGameInstance()))
		{
			TArray<FName> EquippedIDs = GI->GetEquippedGemIDs();

			if (EquippedIDs.Num() == 3)
			{
				SlotGemIDs = EquippedIDs;
			}
		}
	}

	for (int32 i = 0; i < MaxSlots; ++i)
	{
		FName GemID = NAME_None;

		if (DefaultSlotGemIDs.IsValidIndex(i))
		{
			GemID = SlotGemIDs[i];
		}

		if (GemID.IsNone())
		{
			bAllLoaded = false;
			continue;
		}

		bool bLoaded = SetGemSlotByID(i, GemID);

		if (!bLoaded)
		{
			bAllLoaded = false;
		}
	}

	if (!bAllLoaded)
	{
		UE_LOG(LogTemp, Warning, TEXT("RefillDefaultGemsFromDataTable failed partially. Using fallback gems."));
		RefillDefaultGemsFallback();
		return;
	}
	
	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipped gems loaded into battle slots from GameInstance."));
	}
}

//bool UProject_GemCoopGemComponent::UseGem_Internal(int32 SlotIndex)
//{
//	if (!GetOwner() || !GetOwner()->HasAuthority())
//	{
//		return false;
//	}
//
//	if (!GemSlots.IsValidIndex(SlotIndex))
//	{
//		return false;
//	}
//
//	if (!SlotCooldowns.IsValidIndex(SlotIndex))
//	{
//		return false;
//	}
//
//	if (SlotCooldowns[SlotIndex] > 0.0f)
//	{
//		return false;
//	}
//
//	const FGemData& Gem = GemSlots[SlotIndex];
//
//	if (Gem.GemID.IsNone() || Gem.GemType == EGemType::None)
//	{
//		return false;
//	}
//
//	if (!EnergySYComp)
//	{
//		EnergySYComp = GetOwner()->FindComponentByClass<UProject_GemCoopEnergySYComponent>();
//	}
//
//	if (!EnergySYComp)
//	{
//		return false;
//	}
//
//	if (!EnergySYComp->HasEnoughEnergy(Gem.EnergyCost))
//	{
//		return false;
//	}
//
//	PendingUseSlot = SlotIndex;
//
//	if (CombatComp)
//	{
//		CombatComp->StartCasting(DefaultGemCastTime);
//	}
//	else
//	{
//		OnCastingCompleted();
//	}
//
//	return true;
//}

//void UProject_GemCoopGemComponent::OnCastingCompleted()
//{
//	if (!GetOwner() || !GetOwner()->HasAuthority())
//	{
//		return;
//	}
//
//	if (!GemSlots.IsValidIndex(PendingUseSlot))
//	{
//		PendingUseSlot = -1;
//		return;
//	}
//
//	if (!EnergySYComp)
//	{
//		EnergySYComp = GetOwner()->FindComponentByClass<UProject_GemCoopEnergySYComponent>();
//	}
//
//	if (!EnergySYComp)
//	{
//		PendingUseSlot = -1;
//		return;
//	}
//
//	const FGemData Gem = GemSlots[PendingUseSlot];
//
//	if (!EnergySYComp->TryConsumeEnergy(Gem.EnergyCost))
//	{
//		PendingUseSlot = -1;
//		return;
//	}
//
//	EnergySYComp->ChargeUltGauge(Gem.EnergyCost);
//
//	ApplyGemEffect(PendingUseSlot);
//
//	if (FusionSYComp)
//	{
//		//FusionSYComp->RegisterGemUseForFusion(PendingUseSlot, GemSlots[PendingUseSlot]);
//	}
//
//	StartCooldown(PendingUseSlot);
//
//	OnGemUsed.Broadcast(PendingUseSlot, GemSlots[PendingUseSlot]);
//
//	PendingUseSlot = -1;
//}

void UProject_GemCoopGemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UProject_GemCoopGemComponent, SlotCooldownEndTimes);
}

void UProject_GemCoopGemComponent::OnRep_CooldownEndTimes()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_CooldownEndTimes. Owner=%s SlotCount=%d"),
		*GetNameSafe(GetOwner()),
		SlotCooldownEndTimes.Num()
	);
}

float UProject_GemCoopGemComponent::GetServerTimeSeconds() const
{
	if (!GetWorld())
	{
		return 0.0f;
	}

	AGameStateBase* GS = GetWorld()->GetGameState();

	if (GS)
	{
		return GS->GetServerWorldTimeSeconds();
	}

	return GetWorld()->GetTimeSeconds();
}

float UProject_GemCoopGemComponent::GetGemCooldownBySlot(int32 SlotIndex) const
{
	if (!GemSlots.IsValidIndex(SlotIndex))
	{
		return 0.0f;
	}

	return GemSlots[SlotIndex].Cooldown;
}

float UProject_GemCoopGemComponent::GetSlotCooldownRemaining(int32 SlotIndex) const
{
	if (!SlotCooldownEndTimes.IsValidIndex(SlotIndex))
	{
		return 0.0f;
	}

	const float Remaining = SlotCooldownEndTimes[SlotIndex] - GetServerTimeSeconds();

	return FMath::Max(0.0f, Remaining);
}

float UProject_GemCoopGemComponent::GetSlotCooldownPercent(int32 SlotIndex) const
{
	const float Cooldown = GetGemCooldownBySlot(SlotIndex);

	if (Cooldown <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(GetSlotCooldownRemaining(SlotIndex) / Cooldown, 0.0f, 1.f);
}

float UProject_GemCoopGemComponent::GetSlotCooldownFillPercent(int32 SlotIndex) const
{
	return 1.f - GetSlotCooldownPercent(SlotIndex);
}

bool UProject_GemCoopGemComponent::UseGem(int32 SlotIndex)
{
	AActor* OwnerActor = GetOwner();

	if (!OwnerActor)
	{
		return false;
	}

	if (!OwnerActor->HasAuthority())
	{
		return false;
	}

	return UseGem_Internal(SlotIndex);
}

bool UProject_GemCoopGemComponent::UseGem_Internal(int32 SlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	if (!GemSlots.IsValidIndex(SlotIndex))
	{
		return false;
	}

	const int32 DesiredSlotCount = FMath::Max(3, GemSlots.Num());

	if (SlotCooldowns.Num() != DesiredSlotCount)
	{
		SlotCooldowns.SetNum(DesiredSlotCount);
	}

	if (SlotCooldownEndTimes.Num() != DesiredSlotCount)
	{
		SlotCooldownEndTimes.SetNum(DesiredSlotCount);
	}

	const float RemainingCooldown = GetSlotCooldownRemaining(SlotIndex);

	if (RemainingCooldown > 0.0f)
	{
		return false;
	}

	const FGemData& Gem = GemSlots[SlotIndex];

	if (Gem.GemID.IsNone() || Gem.GemType == EGemType::None)
	{
		return false;
	}

	if (!EnergySYComp)
	{
		EnergySYComp = GetOwner()->FindComponentByClass<UProject_GemCoopEnergySYComponent>();
	}

	if (!EnergySYComp)
	{
		return false;
	}

	if (!EnergySYComp->HasEnoughEnergy(Gem.EnergyCost))
	{
		return false;
	}

	PendingUseSlot = SlotIndex;

	const float CastTime = DefaultGemCastTime;

	if (!CombatComp)
	{
		CombatComp = GetOwner()->FindComponentByClass<UProject_GemCoopCombatComponent>();
	}

	if (CombatComp)
	{
		CombatComp->StartCasting(CastTime);
	}
	else
	{
		OnCastingCompleted();
	}

	return true;
}

void UProject_GemCoopGemComponent::OnCastingCompleted()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (!GemSlots.IsValidIndex(PendingUseSlot))
	{
		PendingUseSlot = -1;
		return;
	}

	if (!EnergySYComp)
	{
		EnergySYComp = GetOwner()->FindComponentByClass<UProject_GemCoopEnergySYComponent>();
	}

	if (!EnergySYComp)
	{
		PendingUseSlot = -1;
		return;
	}

	const FGemData Gem = GemSlots[PendingUseSlot];

	if (!EnergySYComp->TryConsumeEnergy(Gem.EnergyCost))
	{
		PendingUseSlot = -1;
		return;
	}

	EnergySYComp->ChargeUltGauge(Gem.EnergyCost);

	ApplyGemEffect(PendingUseSlot);

	if (!FusionSYComp)
	{
		FusionSYComp = GetOwner()->FindComponentByClass<UProject_GemCoopFusionSYComponent>();
	}

	if (FusionSYComp)
	{
		//FusionSYComp->RegisterGemUseForFusion(PendingUseSlot, GemSlots[PendingUseSlot]);
	}

	StartCooldown(PendingUseSlot);

	OnGemUsed.Broadcast(PendingUseSlot, GemSlots[PendingUseSlot]);

	PendingUseSlot = -1;
}

void UProject_GemCoopGemComponent::StartCooldown(int32 SlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (!GemSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	const int32 DesiredSlotCount = FMath::Max(3, GemSlots.Num());

	if (SlotCooldowns.Num() != DesiredSlotCount)
	{
		SlotCooldowns.SetNum(DesiredSlotCount);
	}

	if (SlotCooldownEndTimes.Num() != DesiredSlotCount)
	{
		SlotCooldownEndTimes.SetNum(DesiredSlotCount);
	}

	const float Cooldown = GetGemCooldownBySlot(SlotIndex);

	if (Cooldown <= 0.0f)
	{
		SlotCooldowns[SlotIndex] = 0.0f;
		SlotCooldownEndTimes[SlotIndex] = 0.0f;
		return;
	}

	SlotCooldownEndTimes[SlotIndex] = GetServerTimeSeconds() + Cooldown;
	SlotCooldowns[SlotIndex] = Cooldown;

	OnRep_CooldownEndTimes();

	if (GetOwner())
	{
		GetOwner()->ForceNetUpdate();
	}
}

void UProject_GemCoopGemComponent::ApplyGemEffect(int32 SlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (!GemSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	OwnerCharacter = Cast<AProject_GemCoopCharacter>(GetOwner());

	if (!OwnerCharacter)
	{
		return;
	}

	const FGemData& Gem = GemSlots[SlotIndex];

	if (Gem.GemID.IsNone() || Gem.GemType == EGemType::None)
	{
		return;
	}

	StatComp = OwnerCharacter->FindComponentByClass<UProject_GemCoopStatComponent>();
	BuffComp = OwnerCharacter->FindComponentByClass<UProject_GemCoopBuffComponent>();

	if (!StatComp)
	{
		return;
	}

	const float DefaultBuffDuration = 5.f;

	switch (Gem.GemType)
	{
	case EGemType::Ruby:
	{
		const float AttackRange = 1200.f;
		const float DamageAmount = StatComp->FinalATK * Gem.EffectValue;

		const FVector StartLocation = OwnerCharacter->GetActorLocation() + FVector(0.0f, 0.0f, 60.f);

		const FVector AttackDirection = OwnerCharacter->GetActorForwardVector();
		const FVector EndLocation = StartLocation + AttackDirection * AttackRange;

		FHitResult HitResult;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(OwnerCharacter);
		QueryParams.bTraceComplex = false;

		const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Pawn, QueryParams);

		if (bHit && HitResult.GetActor())
		{
			AActor* HitActor = HitResult.GetActor();

			UGameplayStatics::ApplyDamage(HitActor, DamageAmount, OwnerCharacter->GetController(), OwnerCharacter, UDamageType::StaticClass());
		}

		break;
	}

	case EGemType::Sapphire:
	{
		if (!BuffComp)
		{
			return;
		}

		FBuffData BuffData;
		BuffData.BuffID = TEXT("Buff_Sapphire_Defense");
		BuffData.DisplayName = FText::FromString(TEXT("Sapphire Defense"));
		BuffData.Duration = DefaultBuffDuration;
		BuffData.DEFBonus = 0.5f * Gem.EffectValue;

		BuffComp->AddBuff(BuffData);

		if (CombatComp)
		{
			CombatComp->RecordBuff();
		}

		break;
	}

	case EGemType::Emerald:
	{
		const float HealAmount = StatComp->FinalHP * 0.3f * Gem.EffectValue;

		StatComp->ApplyHeal(HealAmount);

		break;
	}

	case EGemType::Topaz:
	{
		const float BonusHP = 30.f * Gem.EffectValue;

		StatComp->AddTemporaryMaxHP(BonusHP, true);

		if (BuffComp)
		{
			FBuffData BuffData;
			BuffData.BuffID = TEXT("Buff_Topaz_Speed");
			BuffData.DisplayName = FText::FromString(TEXT("Topaz Speed"));
			BuffData.Duration = DefaultBuffDuration;
			BuffData.SPDBonus = 0.15f * Gem.EffectValue;

			BuffComp->AddBuff(BuffData);

			if (CombatComp)
			{
				CombatComp->RecordBuff();
			}
		}

		break;
	}

	case EGemType::Amethyst:
	{
		if (!BuffComp)
		{
			return;
		}

		FBuffData BuffData;
		BuffData.BuffID = TEXT("Buff_Amethyst_Amplify");
		BuffData.DisplayName = FText::FromString(TEXT("Amethyst Amplify"));
		BuffData.Duration = DefaultBuffDuration;

		BuffData.ATKBonus = 0.2f * Gem.EffectValue;
		BuffData.DEFBonus = 0.2f * Gem.EffectValue;
		BuffData.SPDBonus = 0.1f * Gem.EffectValue;
		BuffData.HealBonus = 0.1f * Gem.EffectValue;

		BuffComp->AddBuff(BuffData);

		const float HealAmount = StatComp->FinalHP * 0.1f * Gem.EffectValue;
		StatComp->ApplyHeal(HealAmount);

		if (CombatComp)
		{
			CombatComp->RecordBuff();
		}

		break;
	}
	default:
		break;
	}
}