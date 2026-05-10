// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopFusionSYComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFusionSuccess, FFusionResult, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFusionFailed, const TArray<FGemData>&, Gems);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFusionWindowOpened, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFusionWindowClosed);

class UProject_GemCoopGemDataSubsystem;
class AProject_GemCoopCharacter;
class AProject_GemCoopGameStateBase;
class AProject_GemCoopGameMode;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_GEMCOOP_API UProject_GemCoopFusionSYComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProject_GemCoopFusionSYComponent();

	UPROPERTY(BlueprintAssignable)
	FOnFusionSuccess OnFusionSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnFusionFailed OnFusionFailed;

	UPROPERTY(BlueprintAssignable)
	FOnFusionWindowOpened OnFusionWindowOpened;

	UPROPERTY(BlueprintAssignable)
	FOnFusionWindowClosed OnFusionWindowClosed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fusion")
	float FusionWindowDuration = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fusion")
	float FusionWindowTimer = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fusion")
	bool bFusionWindowOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fusion")
	TArray<FGemFusionRequest> PendingFusionGems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fusion")
	FFusionResult LastFusionResult;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fusion")
	int32 FusionCount = 0;

	UPROPERTY()
	TArray<AProject_GemCoopCharacter*> PartyCharacters;

	UPROPERTY()
	UProject_GemCoopGemDataSubsystem* GemDataSubsystem = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Fusion")
	void RegisterFusionRequest(FGemFusionRequest Request);

	UFUNCTION(BlueprintCallable, Category = "Fusion")
	void TickFusionWindow(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Fusion")
	void ResolveFusion();

	UFUNCTION(BlueprintPure, Category = "Fusion")
	bool IsValidFusionCombo(const TArray<EGemType>& Types) const;

	UFUNCTION(BlueprintCallable, Category = "Fusion")
	void OnFusionWindowExpired();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_FusionResult(FFusionResult Result);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void BroadcastFusionResult(FFusionResult Result);
	bool CheckThresholdCondition(FFusionResult& Result);
	void ResetFusionWindow();
	TArray<EGemType> ExtractTypes(const TArray<FGemFusionRequest>& Requests) const;
	TArray<FGemData> GetPendingGems() const;
	AProject_GemCoopCharacter* GetCharacterByIndex(int32 PlayerIndex) const;

};
