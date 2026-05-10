// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopObjectPSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API UProject_GemCoopObjectPSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	//UPROPERTY()
	TMap<TSubclassOf<AActor>, TArray<AActor*>> MonsterPool;

	UPROPERTY()
	TArray<UParticleSystemComponent*> EffectPool;

	UPROPERTY()
	TArray<AActor*> GemDropPool;

	UPROPERTY(EditAnywhere)
	int32 PoolInitSize = 50;

	UFUNCTION(BlueprintCallable)
	AActor* SpawnFromPool(TSubclassOf<AActor> ActorClass, FVector Location, FRotator Rotation);

	UFUNCTION(BlueprintCallable)
	void ReturnToPool(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void PrewarmPool(TSubclassOf<AActor> ActorClass, int32 Count);

	UFUNCTION(BlueprintCallable)
	UParticleSystemComponent* GetEffectFromPool();

	UFUNCTION(BlueprintCallable)
	void ReturnEffectToPool(UParticleSystemComponent* Effect);

	UFUNCTION(BlueprintPure)
	FString GetPoolStats() const;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};
