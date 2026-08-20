// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopMonsterHPWidget.h"
#include "Project_GemCoopMonsterCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UProject_GemCoopMonsterHPWidget::SetOwningMonster(AProject_GemCoopMonsterCharacter* InMonster)
{
	OwningMonster = InMonster;

	if (OwningMonster.IsValid())
	{
		UpdateHealth(OwningMonster->CurrentHP, OwningMonster->MaxHP);
	}
}

void UProject_GemCoopMonsterHPWidget::UpdateHealth(float CurrentHP, float MaxHP)
{
	const float HealthPercent = MaxHP > 0.0f ? FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f) : 0.0f;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT(
			"MonsterHPWidget UpdateHealth: "
			"PB=%s Current=%.1f Max=%.1f Percent=%.2f"
		),
		*GetNameSafe(PB_Health),
		CurrentHP,
		MaxHP,
		HealthPercent
	);

	if (!PB_Health)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("PB_Health binding failed")
		);

		return;
	}

	if (PB_Health)
	{
		PB_Health->SetPercent(HealthPercent);
	}
}

void UProject_GemCoopMonsterHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (OwningMonster.IsValid())
	{
		UpdateHealth(OwningMonster->CurrentHP, OwningMonster->MaxHP);
	}
}
