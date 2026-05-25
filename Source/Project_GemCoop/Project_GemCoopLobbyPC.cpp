// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopLobbyPC.h"
#include "Blueprint/UserWidget.h"

AProject_GemCoopLobbyPC::AProject_GemCoopLobbyPC()
{
	bShowMouseCursor = true;
}

void AProject_GemCoopLobbyPC::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);

	ShowRootWidget();
}

void AProject_GemCoopLobbyPC::ShowRootWidget()
{
	if (RootWidget)
	{
		RootWidget->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	if (!RootWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("MenuPlayerController: RootWidgetClass is not assigned."));
		return;
	}

	RootWidget = CreateWidget<UUserWidget>(this, RootWidgetClass);

	if (!RootWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("MenuPlayerController: Failed to create RootWidget."));
		return;
	}

	RootWidget->AddToViewport();
}
