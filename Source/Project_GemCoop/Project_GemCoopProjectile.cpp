// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopProjectile.h"
#include "Project_GemCoopCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AProject_GemCoopProjectile::AProject_GemCoopProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	SetRootComponent(CollisionComp);

	CollisionComp->InitSphereRadius(16.f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CollisionComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProject_GemCoopProjectile::OnProjectileOverlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1800.f;
	ProjectileMovement->MaxSpeed = 1800.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->SetIsReplicated(true);

	InitialLifeSpan = LifeTime;
}

// Called when the game starts or when spawned
void AProject_GemCoopProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (CollisionComp)
	{
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProject_GemCoopProjectile::OnProjectileOverlap);
	}
}

void AProject_GemCoopProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AProject_GemCoopProjectile, Damage);
	//DOREPLIFETIME(AProject_GemCoopProjectile, MoveDirection);
}

// Called every frame
void AProject_GemCoopProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProject_GemCoopProjectile::InitializeProjectile(float InDamage, FVector InDirection, AProject_GemCoopCharacter* InOwnerCharacter)
{
	Damage = InDamage;
	
	MoveDirection = InDirection; 
	MoveDirection.Z = 0.0f;

	if (!MoveDirection.IsNearlyZero())
	{
		MoveDirection.Normalize();
	}
	else
	{
		MoveDirection = FVector::ForwardVector;
	}

	OwnerCharacter = InOwnerCharacter;

	SetOwner(InOwnerCharacter);
	SetInstigator(InOwnerCharacter);

	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = MoveDirection * ProjectileMovement->InitialSpeed;
	}

	SetActorRotation(MoveDirection.Rotation());
}

void AProject_GemCoopProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!OtherActor || OtherActor == this || OtherActor == OwnerCharacter)
	{
		return;
	}

	UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerCharacter ? OwnerCharacter->GetController() : nullptr, this, UDamageType::StaticClass());

	Destroy();
}