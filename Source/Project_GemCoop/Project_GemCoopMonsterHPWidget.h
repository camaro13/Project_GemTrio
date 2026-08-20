// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_GemCoopMonsterHPWidget.generated.h"

class UProgressBar;
class UTextBlock;
class AProject_GemCoopMonsterCharacter;
/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API UProject_GemCoopMonsterHPWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Monster|HealthBar")
	void SetOwningMonster(AProject_GemCoopMonsterCharacter* InMonster);

	UFUNCTION(BlueprintCallable, Category = "Monster|HealthBar")
	void UpdateHealth(float CurrentHP, float MaxHP);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* PB_Health = nullptr;

private:
	TWeakObjectPtr<AProject_GemCoopMonsterCharacter> OwningMonster;
};
