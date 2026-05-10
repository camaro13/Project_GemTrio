// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopObjectPSubsystem.h"
#include "Project_GemCoopMonsterCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

void UProject_GemCoopObjectPSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UProject_GemCoopObjectPSubsystem::Deinitialize()
{
	for (auto& Pair : MonsterPool)
	{
		for (AActor* A : Pair.Value)
		{
			if (IsValid(A))
			{
				A->Destroy();
			}
		}
	}

	MonsterPool.Empty();
	
	for (AActor* A : GemDropPool)
	{
		if (IsValid(A))
		{
			A->Destroy();
		}
	}

	GemDropPool.Empty();

	Super::Deinitialize();
}

AActor* UProject_GemCoopObjectPSubsystem::SpawnFromPool(TSubclassOf<AActor> ActorClass, FVector Location, FRotator Rotation)
{
	if (!ActorClass)
	{
		return nullptr;
	}

	TArray<AActor*>& Pool = MonsterPool.FindOrAdd(ActorClass);

	for (AActor* A : Pool)
	{
		if (!IsValid(A))
		{
			continue;
		}

		if (A->IsHidden())
		{
			A->SetActorLocation(Location);
			A->SetActorRotation(Rotation);
			A->SetActorHiddenInGame(false);
			A->SetActorEnableCollision(true);

			if (auto* Monster = Cast<AProject_GemCoopMonsterCharacter>(A))
			{
				Monster->CurrentHP = Monster->MaxHP;
			}

			return A;
		}
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* NewActor = GetWorld()->SpawnActor<AActor>(ActorClass, Location, Rotation, Params);

	if (NewActor)
	{
		Pool.Add(NewActor);
	}

	return NewActor;
}

void UProject_GemCoopObjectPSubsystem::ReturnToPool(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return;
	}

	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);

	if (auto* Prim = Actor->FindComponentByClass<UPrimitiveComponent>())
	{
		Prim->SetPhysicsLinearVelocity(FVector::ZeroVector);
	}

	if (auto* Char = Cast<ACharacter>(Actor))
	{
		Char->GetCharacterMovement()->StopMovementImmediately();

		if (auto* AIC = Cast<AAIController>(Char->GetController()))
		{
			AIC->StopMovement();
		}
	}
}

void UProject_GemCoopObjectPSubsystem::PrewarmPool(TSubclassOf<AActor> ActorClass, int32 Count)
{
	if (!ActorClass)
	{
		return;
	}

	TArray<AActor*>& Pool = MonsterPool.FindOrAdd(ActorClass);

	for (int32 i = 0; i < Count; i++)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* A = GetWorld()->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

		if (A)
		{
			A->SetActorHiddenInGame(true);
			A->SetActorEnableCollision(false);
			Pool.Add(A);
		}
	}
}

UParticleSystemComponent* UProject_GemCoopObjectPSubsystem::GetEffectFromPool()
{
	for (UParticleSystemComponent* E : EffectPool)
	{
		if (E && !E->IsActive())
		{
			E->Activate(true);
			return E;
		}
	}

	auto* NewEffect = NewObject<UParticleSystemComponent>(GetWorld());

	if (NewEffect)
	{
		EffectPool.Add(NewEffect);
	}

	return NewEffect;
}

void UProject_GemCoopObjectPSubsystem::ReturnEffectToPool(UParticleSystemComponent* Effect)
{
	if (Effect)
	{
		Effect->Deactivate();
	}
}

FString UProject_GemCoopObjectPSubsystem::GetPoolStats() const
{
	FString Out = TEXT("=== Object Pool Stats ===\n");

	for (auto& Pair : MonsterPool)
	{
		int32 Total = Pair.Value.Num();
		int32 Active = 0;

		for (AActor* A : Pair.Value)
		{
			if (IsValid(A) && !A->IsHidden())
			{
				Active++;
			}
		}

		Out += FString::Printf(TEXT(" [%s] Total:%d Idle:%d\n"), *Pair.Key->GetName(), Total, Active, Total - Active);
	}

	return Out;
}