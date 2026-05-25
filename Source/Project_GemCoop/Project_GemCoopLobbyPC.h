// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Project_GemCoopLobbyPC.generated.h"

class UUserWidget;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> RootWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	UUserWidget* RootWidget = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowRootWidget();
};
