// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Project_GemCoopLobbyPC.generated.h"

class UProject_GemCoopLobbyWidget;

/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API AProject_GemCoopLobbyPC : public APlayerController
{
	GENERATED_BODY()
	
public:
	AProject_GemCoopLobbyPC();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby")
	TSubclassOf<UProject_GemCoopLobbyWidget> LobbyWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby")
	UProject_GemCoopLobbyWidget* LobbyWidget = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void ShowLobbyWidget();
};
