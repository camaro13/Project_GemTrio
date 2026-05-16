// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h" 
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopGemDropActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECT_GEMCOOP_API AProject_GemCoopGemDropActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProject_GemCoopGemDropActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GemDrop")
	FGemData DropGemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GemDrop")
	int32 DropAmount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GemDrop")
	float LifeTime = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float RotateSpeed = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDebugLog = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GemDrop")
	bool bPickedUp = false;

public:
	UFUNCTION(BlueprintCallable, Category = "GemDrop")
	void SetGemData(const FGemData& NewGemData, int32 NewAmount = 1);

	UFUNCTION(BlueprintPure, Category = "GemDrop")
	FGemData GetGemData() const;

protected:
	UFUNCTION()
	void OnDropOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
