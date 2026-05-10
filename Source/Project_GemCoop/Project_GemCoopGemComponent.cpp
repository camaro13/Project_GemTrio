// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopGemComponent.h"
#include "Project_GemCoopCombatComponent.h"
#include "Project_GemCoopEnergySYComponent.h"
#include "Project_GemCoopFusionSYComponent.h"
#include "Project_GemCoopStatComponent.h"
#include "Project_GemCoopBuffComponent.h"
#include "Project_GemCoopGemDataSubsystem.h"
#include "Project_GemCoopCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UProject_GemCoopGemComponent::UProject_GemCoopGemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	GemSlots.SetNum(4);
	SlotCooldowns.Init(0.f, 4);
	bFusionReady.Init(false, 4);
	// ...
}


// Called when the game starts
void UProject_GemCoopGemComponent::BeginPlay()
{
	Super::BeginPlay();

	CombatCompRef = GetOwner()->FindComponentByClass<UProject_GemCoopCombatComponent>();
	EnergySYCompRef = GetOwner()->FindComponentByClass<UProject_GemCoopEnergySYComponent>();
	FusionSYCompRef = GetOwner()->FindComponentByClass<UProject_GemCoopFusionSYComponent>();
	StatCompRef = GetOwner()->FindComponentByClass<UProject_GemCoopStatComponent>();
	BuffCompRef = GetOwner()->FindComponentByClass<UProject_GemCoopBuffComponent>();

	GemDataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem <UProject_GemCoopGemDataSubsystem>();
	
	if (CombatCompRef)
	{
		CombatCompRef->OnCastingCompleted.AddDynamic(this, &UProject_GemCoopGemComponent::OnCastingCompleted);
	}
	// ...
	
}


// Called every frame
void UProject_GemCoopGemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TickCooldown(DeltaTime);
	// ...
}

bool UProject_GemCoopGemComponent::UseGem(int32 SlotIndex)
{
	if (!GemSlots.IsValidIndex(SlotIndex))
	{
		return false;
	}

	if (!IsSlotReady(SlotIndex))
	{
		return false;
	}

	if (CombatCompRef && CombatCompRef->bIsCasting)
	{
		return false;
	}

	float Cost = GemSlots[SlotIndex].EnergyCost;

	if (EnergySYCompRef && !EnergySYCompRef->TryConsumeEnergy(Cost))
	{
		return false;
	}

	PendingUseSlot = SlotIndex;

	if (CombatCompRef)
	{
		CombatCompRef->StartCasting(0.5f);
	}
	return true;
}

void UProject_GemCoopGemComponent::OnCastingCompleted()
{
	if (!GemSlots.IsValidIndex(PendingUseSlot))
	{
		return;
	}

	ApplyGemEffect(PendingUseSlot);
	StartCoolDown(PendingUseSlot);
	OnGemUsed.Broadcast(PendingUseSlot, GemSlots[PendingUseSlot]);
	PendingUseSlot = -1;
}

void UProject_GemCoopGemComponent::StartFusionAttempt(int32 SlotIndex)
{
	if (!IsSlotReady(SlotIndex))
	{
		return;
	}

	bFusionReady[SlotIndex] = true;
	PendingFusionSlot = SlotIndex;

	if (FusionSYCompRef)
	{
		FGemFusionRequest Req;
		Req.PlayerIndex = PlayerIndex;
		Req.Gem = GemSlots[SlotIndex];
		Req.RequestTime = GetWorld()->GetTimeSeconds();
		FusionSYCompRef->RegisterFusionRequest(Req);
	}
	
	OnFusionAttempt.Broadcast(SlotIndex, GemSlots[SlotIndex]);
}

void UProject_GemCoopGemComponent::CompleteFusion(const TArray<FGemData>& FusionGems)
{
	TArray<EGemType> Types;

	for (const FGemData& G : FusionGems)
	{
		Types.Add(G.GemType);
	}

	if (GemDataSubsystem)
	{
		FFusionResult Result = GemDataSubsystem->LookupFusion(Types);
		if (Result.bSuccess)
		{
			ApplyFusionEffect(Result);
		}
	}

	for (int32 i = 0; i < GemSlots.Num(); i++)
	{
		if (bFusionReady[i])
		{
			SlotCooldowns[i] = GemSlots[i].Cooldown * (1.f - FusionCoolDownBonus);
			bFusionReady[i] = false;
		}
	}

	PendingFusionSlot = -1;
}

void UProject_GemCoopGemComponent::CancelFusion()
{
	for (int32 i = 1; i < bFusionReady.Num(); i++)
	{
		bFusionReady[i] = false;
		PendingFusionSlot = -1;
	}
}

void UProject_GemCoopGemComponent::TickCooldown(float DeltaTime)
{
	for (int32 i = 0; i < SlotCooldowns.Num(); i++)
	{
		if (SlotCooldowns[i] > 0.f)
		{
			SlotCooldowns[i] = FMath::Max(0.f, SlotCooldowns[i] - DeltaTime);
			OnCoolDownChanged.Broadcast(i, SlotCooldowns[i]);
		}
	}
}

void UProject_GemCoopGemComponent::ExchangeGem(int32 SlotIndex, AProject_GemCoopCharacter* Partner)
{
	if (!Partner)
	{
		return;
	}

	auto* PartnerGemCompRef = Partner->FindComponentByClass<UProject_GemCoopGemComponent>();

	if (!PartnerGemCompRef)
	{
		return;
	}

	int32 PartnerSlot = PartnerGemCompRef->PendingFusionSlot;

	if (PartnerSlot < 0)
	{
		return;
	}

	Swap(GemSlots[SlotIndex], PartnerGemCompRef->GemSlots[PartnerSlot]);
	SlotCooldowns[SlotIndex] = 0.f;
	PartnerGemCompRef->SlotCooldowns[PartnerSlot] = 0.f;

	OnGemExchanged.Broadcast(SlotIndex, PartnerSlot);
	PartnerGemCompRef->OnGemExchanged.Broadcast(PartnerSlot, SlotIndex);
}

FGemData UProject_GemCoopGemComponent::GetGemData(int32 SlotIndex) const
{
	return GemSlots.IsValidIndex(SlotIndex) ? GemSlots[SlotIndex] : FGemData();
}

bool UProject_GemCoopGemComponent::IsSlotReady(int32 SlotIndex) const
{
	return GemSlots.IsValidIndex(SlotIndex) && SlotCooldowns[SlotIndex] <= 0.f && !bFusionReady[SlotIndex];
}

void UProject_GemCoopGemComponent::SetGemSlot(int32 SlotIndex, FGemData NewGem)
{
	if (!GemSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	GemSlots[SlotIndex] = NewGem;
	SlotCooldowns[SlotIndex] = 0.f;
	OnSlotRefilled.Broadcast(SlotIndex, NewGem);
}

void UProject_GemCoopGemComponent::StartCoolDown(int32 SlotIndex)
{
	if (!GemSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	float BaseCoolDown = GemSlots[SlotIndex].Cooldown;
	SlotCooldowns[SlotIndex] = BaseCoolDown * (1.f - CoolDownReduction);
}

void UProject_GemCoopGemComponent::ApplyGemEffect(int32 SlotIndex)
{
}

void UProject_GemCoopGemComponent::ApplyFusionEffect(const FFusionResult& Result)
{
}

