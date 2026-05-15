// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Project_GemCoopHUD.generated.h"

class UProject_GemCoopMainHUDWidget;
/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API AProject_GemCoopHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AProject_GemCoopHUD();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UProject_GemCoopMainHUDWidget> MainHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	UProject_GemCoopMainHUDWidget* MainHUDWidget = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowMainHUD();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideMainHUD();
};
