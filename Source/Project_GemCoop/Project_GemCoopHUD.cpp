// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopHUD.h"
#include "Project_GemCoopMainHUDWidget.h"
#include "Blueprint/UserWidget.h"

AProject_GemCoopHUD::AProject_GemCoopHUD()
{
}

void AProject_GemCoopHUD::BeginPlay()
{
	Super::BeginPlay();

	ShowMainHUD();
}

void AProject_GemCoopHUD::ShowMainHUD()
{
	if (MainHUDWidget)
	{
		MainHUDWidget->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	if (!MainHUDWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("GemCoopHUD: MainHUDWidgetClass is not assigned."));
		return;
	}

	APlayerController* PC = GetOwningPlayerController();

	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("GemCoopHUD: OwningPlayerController missing."));
		return;
	}

	MainHUDWidget = CreateWidget<UProject_GemCoopMainHUDWidget>(PC, MainHUDWidgetClass);

	if (!MainHUDWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("GemCoopHUD: Failed to create MainHUDWidget."));
		return;
	}

	MainHUDWidget->AddToViewport();
}

void AProject_GemCoopHUD::HideMainHUD()
{
	if (MainHUDWidget)
	{
		MainHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
