// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetChanged, AActor*, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFormationToggled, bool, bActive);

class UInputMappingContext;
class UUserWidget;
class UProject_GemCoopGemComponent;
class AProject_GemCoopCharacter;
class AProject_GemCoopHUD;
class AProject_GemCoopMonsterCharacter;
/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AProject_GemCoopPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

public:
	AProject_GemCoopPlayerController();

	UPROPERTY(BlueprintAssignable)
	FOnTargetChanged OnTargetChanged;

	UPROPERTY(BlueprintAssignable)
	FOnFormationToggled OnFormationToggled;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	AActor* CurrentTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	int32 PendingExchangeSlot = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bFormationActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	AProject_GemCoopCharacter* PossessedCharacter = nullptr;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	//AProject_GemCoopHUD* HUDRef = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float TargetTraceDistance = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float ExchangeSearchRadius = 500.f;

	UFUNCTION(BlueprintCallable, Category = "Input")
	void UseGemSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnDashInput();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnUltimateInput();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnFusionInput();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnExchangeInput();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnFormationInput();

	UFUNCTION(BlueprintPure, Category = "Targeting")
	FHitResult GetMouseTargetHit() const;

	UFUNCTION(Server, Reliable)
	void ServerRPC_UseGem(int32 SlotIndex);


	UFUNCTION(Server, Reliable)
	void ServerRPC_Ultimate();

	UFUNCTION(Server, Reliable)
	void ServerRPC_ExchangeGem(AProject_GemCoopCharacter* Partner, int32 SlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_FusionAttempt(int32 SlotIndex);

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	void UpdateMouseTarget();

	void OnGemQ();
	void OnGemW();
	void OnGemE();
	void OnGemR();

	void OnMouseClick();
	void OnPauseInput();
};
