// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopGemDropActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Project_GemCoopCharacter.h"
#include "Project_GemCoopGameInstance.h"

// Sets default values
AProject_GemCoopGemDropActor::AProject_GemCoopGemDropActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	SetRootComponent(CollisionComp);

	CollisionComp->InitSphereRadius(80.f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CollisionComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DropAmount = 1;
	LifeTime = 20.f;
	RotateSpeed = 90.f;
	bDebugLog = true;
	bPickedUp = false;
}

// Called when the game starts or when spawned
void AProject_GemCoopGemDropActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (CollisionComp)
	{
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProject_GemCoopGemDropActor::OnDropOverlap);
	}

	SetLifeSpan(LifeTime);

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("GemDrop BeginPlay. GemID=%s Amount=%d"),
			*DropGemData.GemID.ToString(),
			DropAmount
		);
	}
}

// Called every frame
void AProject_GemCoopGemDropActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldRotation(FRotator(0.0f, RotateSpeed * DeltaTime, 0.0f));
}

void AProject_GemCoopGemDropActor::SetGemData(const FGemData& NewGemData, int32 NewAmount)
{
	DropGemData = NewGemData;
	DropAmount = FMath::Max(1, NewAmount);

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("GemDrop SetGemData. GemID=%s Amount=%d"),
			*DropGemData.GemID.ToString(),
			DropAmount
		);
	}
}

FGemData AProject_GemCoopGemDropActor::GetGemData() const
{
	return DropGemData;
}

void AProject_GemCoopGemDropActor::OnDropOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bPickedUp)
	{
		return;
	}

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	AProject_GemCoopCharacter* Character = Cast<AProject_GemCoopCharacter>(OtherActor);

	if (!Character || !Character->bIsAlive)
	{
		return;
	}

	if (DropGemData.GemID.IsNone() || DropGemData.GemType == EGemType::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("GemDrop pickup failed. Invalid DropGemData."));
		return;
	}

	UProject_GemCoopGameInstance* GI = nullptr;

	if (GetWorld())
	{
		GI = Cast<UProject_GemCoopGameInstance>(GetWorld()->GetGameInstance());
	}

	if (!GI)
	{
		UE_LOG(LogTemp, Warning, TEXT("GemDrop pickup failed. Project_GemCoopGameInstance missing."));
		return;
	}

	bPickedUp = true;

	GI->AddGemToInventory(DropGemData, DropAmount);
	GI->DebugPrintGemInventory();

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("GemDrop picked up. Player=%s GemID=%s Amount=%d"),
			*GetNameSafe(Character),
			*DropGemData.GemID.ToString(),
			DropAmount
		);
	}

	Destroy();
}

