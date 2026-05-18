// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopLobbyPC.h"
#include "Project_GemCoopLobbyWidget.h"
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

	ShowLobbyWidget();
}

void AProject_GemCoopLobbyPC::ShowLobbyWidget()
{
	if (LobbyWidget)
	{
		LobbyWidget->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	if (!LobbyWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("LobbyWidgetClass is not assigned."));
		return;
	}

	LobbyWidget = CreateWidget<UProject_GemCoopLobbyWidget>(this, LobbyWidgetClass);

	if (!LobbyWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create LobbyWidget."));
		return;
	}

	LobbyWidget->AddToViewport();
}
