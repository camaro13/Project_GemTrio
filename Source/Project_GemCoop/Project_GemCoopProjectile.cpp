// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProject_GemCoopProjectile::AProject_GemCoopProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	SetRootComponent(CollisionComp);

	CollisionComp->InitSphereRadius(16.f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProject_GemCoopProjectile::OnProjectileOverlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1800.f;
	ProjectileMovement->MaxSpeed = 1800.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	InitialLifeSpan = LifeTime;
}

// Called when the game starts or when spawned
void AProject_GemCoopProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(LifeTime);
}

// Called every frame
void AProject_GemCoopProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProject_GemCoopProjectile::InitializeProjectile(float InDamage, FVector Direction, AActor* InOwner)
{
	Damage = InDamage;
	DamageOwner = InOwner;

	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = Direction.GetSafeNormal() * ProjectileMovement->InitialSpeed;
	}
}

void AProject_GemCoopProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* OwnerPawn = Cast<APawn>(DamageOwner);
	AController* OwnerController = OwnerPawn ? OwnerPawn->GetController() : nullptr;

	UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, DamageOwner, nullptr);

	Destroy();
}

