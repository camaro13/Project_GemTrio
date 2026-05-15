// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopMainHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Project_GemCoopCharacter.h"
#include "Project_GemCoopGameStateBase.h"
#include "Project_GemCoopStatComponent.h"
#include "Project_GemCoopEnergySYComponent.h"
#include "Project_GemCoopGemComponent.h"

void UProject_GemCoopMainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CacheReferences();
}

void UProject_GemCoopMainHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!OwnerCharacter || !StatComp || !EnergyComp || !GemComp || !GameStateRef)
	{
		CacheReferences();
	}

	UpdateHP();
	UpdateEnergy();
	UpdateUltimate();
	UpdateWave();
	UpdateGemSlots();
}

void UProject_GemCoopMainHUDWidget::CacheReferences()
{
	OwnerCharacter = Cast<AProject_GemCoopCharacter>(GetOwningPlayerPawn());

	if (OwnerCharacter)
	{
		StatComp = OwnerCharacter->FindComponentByClass<UProject_GemCoopStatComponent>();
		EnergyComp = OwnerCharacter->FindComponentByClass<UProject_GemCoopEnergySYComponent>();
		GemComp = OwnerCharacter->FindComponentByClass<UProject_GemCoopGemComponent>();
	}

	UWorld* World = GetWorld();

	if (World)
	{
		GameStateRef = World->GetGameState<AProject_GemCoopGameStateBase>();
	}
}

void UProject_GemCoopMainHUDWidget::UpdateHP()
{
	if (!StatComp)
	{
		return;
	}

	float CurrentHP = StatComp->CurrentHP;
	float MaxHP = StatComp->FinalHP;
	float HPPercent = MaxHP > 0.0f ? CurrentHP / MaxHP : 0.0f;

	if (PB_HP)
	{
		PB_HP->SetPercent(HPPercent);
	}

	if (TXT_HP)
	{
		TXT_HP->SetText(FText::FromString(FString::Printf(TEXT("HP %.0f / %.0f"), CurrentHP, MaxHP)));
	}
}

void UProject_GemCoopMainHUDWidget::UpdateEnergy()
{
	if (!GameStateRef)
	{
		return;
	}

	float CurrentEnergy = GameStateRef->SharedEnergy;
	float MaxEnergy = GameStateRef->MaxSharedEnergy;
	float EnergyPercent = MaxEnergy > 0.0f ? CurrentEnergy / MaxEnergy : 0.0f;

	if (PB_Energy)
	{
		PB_Energy->SetPercent(EnergyPercent);
	}

	if (TXT_Energy)
	{
		TXT_Energy->SetText(FText::FromString(FString::Printf(TEXT("Energy %.0f / %.0f"), CurrentEnergy, MaxEnergy)));
	}
}

void UProject_GemCoopMainHUDWidget::UpdateUltimate()
{
	if (!EnergyComp)
	{
		return;
	}

	float UltPercent = EnergyComp->GetUltGaugePercent();

	if (PB_Ult)
	{
		PB_Ult->SetPercent(UltPercent);
	}

	if (TXT_Ult)
	{
		FString ReadyText = EnergyComp->bUltimateReady ? TEXT("Ready") : TEXT("");
		TXT_Ult->SetText(FText::FromString(FString::Printf(TEXT("ULT %.0f%% %s"), UltPercent * 100.0f, *ReadyText)));
	}
}

void UProject_GemCoopMainHUDWidget::UpdateWave()
{
	if (!GameStateRef)
	{
		return;
	}

	if (TXT_Wave)
	{
		TXT_Wave->SetText(FText::FromString(FString::Printf(TEXT("Wave %d"), GameStateRef->CurrentWaveNumber)));
	}
}

void UProject_GemCoopMainHUDWidget::UpdateGemSlots()
{
	if (!GemComp)
	{
		return;
	}

	if (PB_Gem0Cooldown)
	{
		PB_Gem0Cooldown->SetPercent(GetGemCooldownPercent(0));
	}

	if (PB_Gem1Cooldown)
	{
		PB_Gem1Cooldown->SetPercent(GetGemCooldownPercent(1));
	}

	if (PB_Gem2Cooldown)
	{
		PB_Gem2Cooldown->SetPercent(GetGemCooldownPercent(2));
	}

	if (TXT_Gem0)
	{
		TXT_Gem0->SetText(GetGemDisplayText(0));
	}

	if (TXT_Gem1)
	{
		TXT_Gem1->SetText(GetGemDisplayText(1));
	}

	if (TXT_Gem2)
	{
		TXT_Gem2->SetText(GetGemDisplayText(2));
	}
}

float UProject_GemCoopMainHUDWidget::GetGemCooldownPercent(int32 SlotIndex) const
{
	if (!GemComp)
	{
		return 0.0f;
	}

	if (!GemComp->GemSlots.IsValidIndex(SlotIndex) || !GemComp->SlotCooldowns.IsValidIndex(SlotIndex))
	{
		return 0.0f;
	}

	float Cooldown = GemComp->GemSlots[SlotIndex].Cooldown;
	float Remaining = GemComp->SlotCooldowns[SlotIndex];

	if (Cooldown <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(Remaining / Cooldown, 0.0f, 1.0f);
}

FText UProject_GemCoopMainHUDWidget::GetGemDisplayText(int32 SlotIndex) const
{
	if (!GemComp || !GemComp->GemSlots.IsValidIndex(SlotIndex))
	{
		return FText::FromString(TEXT("-"));
	}

	FGemData& Gem = GemComp->GemSlots[SlotIndex];

	if (Gem.GemType == EGemType::None)
	{
		return FText::FromString(TEXT("Empty"));
	}

	float Remaining = GemComp->SlotCooldowns.IsValidIndex(SlotIndex) ? GemComp->SlotCooldowns[SlotIndex] : 0.0f;

	if (Remaining > 0.0f)
	{
		return FText::FromString(FString::Printf(TEXT("%s %.1f"), *Gem.DisplayName.ToString(), Remaining));
	}

	return Gem.DisplayName;
}
