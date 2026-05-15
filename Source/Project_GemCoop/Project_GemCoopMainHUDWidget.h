// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_GemCoopMainHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class AProject_GemCoopCharacter;
class AProject_GemCoopGameStateBase;
class UProject_GemCoopEnergySYComponent;
class UProject_GemCoopGemComponent;
class UProject_GemCoopStatComponent;

/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API UProject_GemCoopMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* PB_HP;

	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* PB_Energy;

	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* PB_Ult;

	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* PB_Gem0Cooldown;

	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* PB_Gem1Cooldown;

	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* PB_Gem2Cooldown;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_HP;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_Energy;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_Ult;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_Wave;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_Gem0;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_Gem1;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_Gem2;

private:
	UPROPERTY()
	AProject_GemCoopCharacter* OwnerCharacter = nullptr;

	UPROPERTY()
	AProject_GemCoopGameStateBase* GameStateRef = nullptr;

	UPROPERTY()
	UProject_GemCoopStatComponent* StatComp = nullptr;

	UPROPERTY()
	UProject_GemCoopEnergySYComponent* EnergyComp = nullptr;

	UPROPERTY()
	UProject_GemCoopGemComponent* GemComp = nullptr;

private:
	void CacheReferences();
	void UpdateHP();
	void UpdateEnergy();
	void UpdateUltimate();
	void UpdateWave();
	void UpdateGemSlots();

	float GetGemCooldownPercent(int32 SlotIndex) const;
	FText GetGemDisplayText(int32 SlotIndex) const;
};
