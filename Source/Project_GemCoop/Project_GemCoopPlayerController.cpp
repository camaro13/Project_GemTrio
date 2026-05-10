// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project_GemCoopPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Project_GemCoop.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "Project_GemCoopCharacter.h"
//#include "Project_GemCoopHUD.h"
#include "Project_GemCoopGameMode.h"
#include "Project_GemCoopMonsterCharacter.h"
#include "Project_GemCoopGemComponent.h"
#include "Project_GemCoopEnergySYComponent.h"
#include "Project_GemCoopGameStateBase.h"
#include "Kismet/GameplayStatics.h"


AProject_GemCoopPlayerController::AProject_GemCoopPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	bShowMouseCursor = true;
}

void AProject_GemCoopPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		}
		else {

			UE_LOG(LogProject_GemCoop, Error, TEXT("Could not spawn mobile controls widget."));

		}
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			for (UInputMappingContext* MappingContext : DefaultMappingContexts)
			{
				if (MappingContext)
				{
					Subsystem->AddMappingContext(MappingContext, 0);
				}
			}
		}
	}
	//HUDRef = Cast<AProject_GemCoopHUD>(GetHUD());
}

void AProject_GemCoopPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PossessedCharacter = Cast<AProject_GemCoopCharacter>(InPawn);
	/*if (HUDRef && PossessedCharacter)
	{
		HUDRef->SetOwnerCharacter(PossessedCharacter);
	}*/
}

void AProject_GemCoopPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	PossessedCharacter = nullptr;
}

void AProject_GemCoopPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsLocalController())
	{
		UpdateMouseTarget();
	}
}

void AProject_GemCoopPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool AProject_GemCoopPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void AProject_GemCoopPlayerController::UseGemSlot(int32 SlotIndex)
{
	if (!PossessedCharacter || !PossessedCharacter->bIsAlive)
	{
		return;
	}

	ServerRPC_UseGem(SlotIndex);
}

void AProject_GemCoopPlayerController::ServerRPC_UseGem_Implementation(int32 SlotIndex)
{
	if (PossessedCharacter && PossessedCharacter->GemCompRef)
	{
		PossessedCharacter->GemCompRef->UseGem(SlotIndex);
	}
}

void AProject_GemCoopPlayerController::OnDashInput()
{
	if (PossessedCharacter)
	{
		PossessedCharacter->OnDashInput();
	}
}

void AProject_GemCoopPlayerController::OnUltimateInput()
{
	if (!PossessedCharacter)
	{
		return;
	}

	auto* EnergySY = PossessedCharacter->EnergySYCompRef;

	if (EnergySY && EnergySY->bUltimateReady)
	{
		ServerRPC_Ultimate();
	}
}

void AProject_GemCoopPlayerController::ServerRPC_Ultimate_Implementation()
{
	auto* GM = Cast<AProject_GemCoopGameMode>(UGameplayStatics::GetGameMode(this));

	if (GM)
	{
		GM->ActivatePartyUltimate(PossessedCharacter);
	}

	if (PossessedCharacter && PossessedCharacter->EnergySYCompRef)
	{
		PossessedCharacter->EnergySYCompRef->ConsumeUltGauge();
	}
}

void AProject_GemCoopPlayerController::OnFormationInput()
{
	bFormationActive = !bFormationActive;
	OnFormationToggled.Broadcast(bFormationActive);

	auto* GS = Cast<AProject_GemCoopGameStateBase>(UGameplayStatics::GetGameState(this));

	if (GS)
	{
		GS->bFormationActive = bFormationActive;
	}
}

void AProject_GemCoopPlayerController::OnGemQ()
{
	UseGemSlot(0);
}

void AProject_GemCoopPlayerController::OnGemW()
{
	UseGemSlot(1);
}

void AProject_GemCoopPlayerController::OnGemE()
{
	UseGemSlot(2);
}

void AProject_GemCoopPlayerController::OnGemR()
{
	UseGemSlot(3);
}

void AProject_GemCoopPlayerController::OnFusionInput()
{
	int32 Slot = (PendingExchangeSlot >= 0) ? PendingExchangeSlot : 0;
	ServerRPC_FusionAttempt(Slot);
}

void AProject_GemCoopPlayerController::ServerRPC_FusionAttempt_Implementation(int32 SlotIndex)
{
	if (PossessedCharacter && PossessedCharacter->GemCompRef)
	{
		PossessedCharacter->GemCompRef->StartFusionAttempt(SlotIndex);
	}
}

void AProject_GemCoopPlayerController::OnMouseClick()
{
	FHitResult Hit = GetMouseTargetHit();

	if (Hit.bBlockingHit)
	{
		CurrentTarget = Hit.GetActor();
	}
}

void AProject_GemCoopPlayerController::OnPauseInput()
{
	/*if (HUDRef)
	{
		HUDRef->TogglePauseMenu();
	}*/
}

void AProject_GemCoopPlayerController::UpdateMouseTarget()
{
	FHitResult Hit = GetMouseTargetHit();
	AActor* NewTarget = Hit.bBlockingHit ? Hit.GetActor() : nullptr;

	bool bValid = NewTarget && (NewTarget->IsA<AProject_GemCoopMonsterCharacter>() || NewTarget->IsA<AProject_GemCoopCharacter>());

	if (!bValid)
	{
		NewTarget = nullptr;
	}

	if (CurrentTarget != NewTarget)
	{
		CurrentTarget = NewTarget;
		OnTargetChanged.Broadcast(CurrentTarget);

		if (PossessedCharacter && PossessedCharacter->GemCompRef)
		{
			PossessedCharacter->GemCompRef->CurrentTarget = CurrentTarget;
		}
	}
}

FHitResult AProject_GemCoopPlayerController::GetMouseTargetHit() const
{
	FVector Loc;
	FVector Dir;

	DeprojectMousePositionToWorld(Loc, Dir);
	FHitResult Hit;
	FCollisionQueryParams P;
	GetWorld()->LineTraceSingleByChannel(Hit, Loc, Loc + Dir * TargetTraceDistance, ECC_Pawn, P);
	return Hit;
}

void AProject_GemCoopPlayerController::OnExchangeInput()
{
	if (!PossessedCharacter || !PossessedCharacter->bIsAlive)
	{
		return;
	}

	TArray<AActor*> NearbyActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProject_GemCoopCharacter::StaticClass(), NearbyActors);

	AProject_GemCoopCharacter* ClosestPartner = nullptr;
	float MinDist = ExchangeSearchRadius;

	for (AActor* Actor : NearbyActors)
	{
		auto* Other = Cast<AProject_GemCoopCharacter>(Actor);
		if (!Other || Other == PossessedCharacter || !Other->bIsAlive)
		{
			continue;
		}

		float Dist = FVector::Dist(PossessedCharacter->GetActorLocation(), Other->GetActorLocation());

		if (Dist < MinDist)
		{
			MinDist = Dist;
			ClosestPartner = Other;
		}
	}

	if (ClosestPartner)
	{
		int32 SlotIndex = (PendingExchangeSlot >= 0) ? PendingExchangeSlot : 0;
		ServerRPC_ExchangeGem(ClosestPartner, SlotIndex);
	}
}

void AProject_GemCoopPlayerController::ServerRPC_ExchangeGem_Implementation(AProject_GemCoopCharacter* Partner, int32 SlotIndex)
{
	if (PossessedCharacter && PossessedCharacter->GemCompRef)
	{
		PossessedCharacter->GemCompRef->ExchangeGem(SlotIndex, Partner);
	}
}