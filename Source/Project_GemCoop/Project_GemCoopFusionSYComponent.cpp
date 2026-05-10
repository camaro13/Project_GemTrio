// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopFusionSYComponent.h"
#include "Project_GemCoopGemDataSubsystem.h"
#include "Project_GemCoopGameStateBase.h"
#include "Project_GemCoopGameMode.h"
#include "Project_GemCoopCharacter.h"
#include "Project_GemCoopGemComponent.h"
#include "Project_GemCoopCombatComponent.h"
#include "Project_GemCoopStatComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UProject_GemCoopFusionSYComponent::UProject_GemCoopFusionSYComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UProject_GemCoopFusionSYComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UProject_GemCoopFusionSYComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UProject_GemCoopFusionSYComponent::RegisterFusionRequest(FGemFusionRequest Request)
{
}

void UProject_GemCoopFusionSYComponent::TickFusionWindow(float DeltaTime)
{
}

void UProject_GemCoopFusionSYComponent::ResolveFusion()
{
}

bool UProject_GemCoopFusionSYComponent::IsValidFusionCombo(const TArray<EGemType>& Types) const
{
	return false;
}

void UProject_GemCoopFusionSYComponent::OnFusionWindowExpired()
{
}

void UProject_GemCoopFusionSYComponent::MulticastRPC_FusionResult_Implementation(FFusionResult Result)
{
}



void UProject_GemCoopFusionSYComponent::BroadcastFusionResult(FFusionResult Result)
{
}

bool UProject_GemCoopFusionSYComponent::CheckThresholdCondition(FFusionResult& Result)
{
	return false;
}

void UProject_GemCoopFusionSYComponent::ResetFusionWindow()
{
}

TArray<EGemType> UProject_GemCoopFusionSYComponent::ExtractTypes(const TArray<FGemFusionRequest>& Requests) const
{
	return TArray<EGemType>();
}

TArray<FGemData> UProject_GemCoopFusionSYComponent::GetPendingGems() const
{
	return TArray<FGemData>();
}

AProject_GemCoopCharacter* UProject_GemCoopFusionSYComponent::GetCharacterByIndex(int32 PlayerIndex) const
{
	return nullptr;
}

