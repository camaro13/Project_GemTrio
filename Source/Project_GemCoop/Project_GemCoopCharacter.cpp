// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project_GemCoopCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Project_GemCoopStatComponent.h"
#include "Project_GemCoopTraitComponent.h"
#include "Project_GemCoopBuffComponent.h"
#include "Project_GemCoopCombatComponent.h"
#include "Project_GemCoopGemComponent.h"
#include "Project_GemCoopEnergySYComponent.h"
#include "Project_GemCoopFusionSYComponent.h"
#include "Project_GemCoopGameMode.h"
#include "Project_GemCoopProjectile.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Project_GemCoop.h"
#include "Project_GemCoopPlayerController.h"

AProject_GemCoopCharacter::AProject_GemCoopCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	StatCompRef = CreateDefaultSubobject<UProject_GemCoopStatComponent>(TEXT("StatComp"));
	TraitCompRef = CreateDefaultSubobject<UProject_GemCoopTraitComponent>(TEXT("TraitComp"));
	BuffCompRef = CreateDefaultSubobject<UProject_GemCoopBuffComponent>(TEXT("BuffComp"));
	CombatCompRef = CreateDefaultSubobject<UProject_GemCoopCombatComponent>(TEXT("CombatComp"));
	GemCompRef = CreateDefaultSubobject<UProject_GemCoopGemComponent>(TEXT("GemComp"));
	EnergySYCompRef = CreateDefaultSubobject<UProject_GemCoopEnergySYComponent>(TEXT("EnergySYComp"));
	FusionSYCompRef = CreateDefaultSubobject<UProject_GemCoopFusionSYComponent>(TEXT("FusionSYComp"));

}

void AProject_GemCoopCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProject_GemCoopCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AProject_GemCoopCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProject_GemCoopCharacter::Look);
	
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AProject_GemCoopCharacter::OnDashInput);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &AProject_GemCoopCharacter::OnDashInputEnd);
		EnhancedInputComponent->BindAction(Gem1Action, ETriggerEvent::Triggered, this, &AProject_GemCoopCharacter::OnGemQ);
		EnhancedInputComponent->BindAction(Gem2Action, ETriggerEvent::Triggered, this, &AProject_GemCoopCharacter::OnGemW);
		EnhancedInputComponent->BindAction(Gem3Action, ETriggerEvent::Triggered, this, &AProject_GemCoopCharacter::OnGemE);
		EnhancedInputComponent->BindAction(UltAction, ETriggerEvent::Triggered, this, &AProject_GemCoopCharacter::OnUltimateInput);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AProject_GemCoopCharacter::OnInteractInput);
		EnhancedInputComponent->BindAction(BasicAttackAction, ETriggerEvent::Started, this, &AProject_GemCoopCharacter::OnBasicAttackInput);
	}
	else
	{
		UE_LOG(LogProject_GemCoop, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AProject_GemCoopCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AProject_GemCoopCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AProject_GemCoopCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AProject_GemCoopCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AProject_GemCoopCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AProject_GemCoopCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AProject_GemCoopCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProject_GemCoopCharacter, bIsAlive);
	DOREPLIFETIME(AProject_GemCoopCharacter, ReviveCount);
	DOREPLIFETIME(AProject_GemCoopCharacter, TeamIndex);
	DOREPLIFETIME(AProject_GemCoopCharacter, CurrentZone);
	DOREPLIFETIME(AProject_GemCoopCharacter, FormationPosition);
}

void AProject_GemCoopCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsAlive)
	{
		return;
	}

	if (BasicAttackCooldownTimer > 0.0f)
	{
		BasicAttackCooldownTimer = FMath::Max(0.0f, BasicAttackCooldownTimer - DeltaTime);
	}

	UpdateAimToMouse();

	if (bDashInput && !bDashLocked && DashStamina > 0.0f)
	{
		bIsDash = true;
	}
	else
	{
		bIsDash = false;
	}

	if (bIsDash)
	{
		DashStamina -= 10.0f * DeltaTime;

		if (DashStamina <= 0.0f)
		{
			DashStamina = 0.0f;
			bIsDash = false;
			bDashLocked = true;
			bDashInput = false;
		}
	}
	else
	{
		DashStamina += 5.0f * DeltaTime;
	}

	DashStamina = FMath::Clamp(DashStamina, 0.0f, 100.0f);

	UpdateWalkSpeedByState();
}


void AProject_GemCoopCharacter::BeginPlay()
{
	Super::BeginPlay();

	EnergySYCompRef->TryConsumeEnergy(20.f);

	StatCompRef = FindComponentByClass<UProject_GemCoopStatComponent>();
	TraitCompRef = FindComponentByClass<UProject_GemCoopTraitComponent>();
	BuffCompRef = FindComponentByClass<UProject_GemCoopBuffComponent>();
	CombatCompRef = FindComponentByClass<UProject_GemCoopCombatComponent>();
	GemCompRef = FindComponentByClass<UProject_GemCoopGemComponent>();
	EnergySYCompRef = FindComponentByClass<UProject_GemCoopEnergySYComponent>();
	FusionSYCompRef = FindComponentByClass<UProject_GemCoopFusionSYComponent>();

	if (!StatCompRef || !TraitCompRef || !BuffCompRef || !CombatCompRef || !GemCompRef || !EnergySYCompRef || !FusionSYCompRef)
	{
		UE_LOG(LogProject_GemCoop, Error, TEXT("Character component missing on %s"), *GetName());
		return;
	}

	EGemTrait DefaultTrait = (TeamIndex == 0) ? EGemTrait::Defense : (TeamIndex == 1) ? EGemTrait::Offense : EGemTrait::Support;

	TraitCompRef->SetTrait(DefaultTrait);
	//BuffComp->SetTraitModifier();
	//StatComp->SetModifier();
	//RecalculateFinalStats();

	StatCompRef->OnDeath.AddDynamic(this, &AProject_GemCoopCharacter::OnDeath);
	StatCompRef->OnHPChanged.AddDynamic(this, &AProject_GemCoopCharacter::OnHPChanged);
	//BuffCompRef->OnBurnTick.AddDynamic(this, &AProject_GemCoopCharacter::OnBurnDamage);
	CombatCompRef->OnCastingStarted.AddDynamic(this, &AProject_GemCoopCharacter::OnCastingStarted);
	CombatCompRef->OnCastingCancelled.AddDynamic(this, &AProject_GemCoopCharacter::OnCastingCancelled);
	CombatCompRef->OnCastingCompleted.AddDynamic(this, &AProject_GemCoopCharacter::OnCastingCompleted);
	
	bIsAlive = true;
	UpdateFormationRole();
	UpdateWalkSpeedByState();

	UE_LOG(LogProject_GemCoop, Log, TEXT("GemCoopCharacter BeginPlay Complete. HP: %.1f / %.1f"), GetCurrentHP(), GetMaxHP());
}

void AProject_GemCoopCharacter::ApplyZoneEffect(EArenaZone NewZone)
{
	if (CurrentZone == NewZone)
	{
		return;
	}

	EArenaZone OldZone = CurrentZone;
	CurrentZone = NewZone;

	if (EnergySYCompRef)
	{
		EnergySYCompRef->ApplySafeZoneBonus(false);
	}

	switch (NewZone)
	{
	case EArenaZone::Safe:
		ZoneDEFModifier = 1.0f;
		ZoneDMGModifier = 1.0f;
		EnergySYCompRef->ApplySafeZoneBonus(true);
		break;

	case EArenaZone::Combat:
		ZoneDEFModifier = 1.0f;
		ZoneDMGModifier = 1.0f;
		EnergySYCompRef->ApplySafeZoneBonus(false);
		break;

	case EArenaZone::Danger:
		ZoneDEFModifier = 0.8f;
		ZoneDMGModifier = 1.2f;
		EnergySYCompRef->ApplySafeZoneBonus(false);
		break;
	}

	OnZoneChanged.Broadcast(OldZone, NewZone);
}

void AProject_GemCoopCharacter::Revive()
{
	if (bIsAlive)
	{
		return;
	}

	if (ReviveCount <= 0)
	{
		return;
	}

	ReviveCount--;
	bIsAlive = true;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//GetMesh()->SetSimulatePhysics(false);

	/*if (BuffCompRef)
	{
		FBuffData InvincBuff;
		InvincBuff.BuffID = TEXT("ReviveInvincible");
		InvincBuff.Duration = 3.f;
		InvincBuff.DEFBonus = 999.f;
		BuffCompRef->AddBuff(InvincBuff);
	}

	if (StatCompRef)
	{
		StatCompRef->SetCurrentHPPercent(0.5f);
	}

	UE_LOG(LogProject_GemCoop, Warning, TEXT("After Revive HP: %.1f / %.1f"),
		StatCompRef ? StatCompRef->CurrentHP : -1.f,
		StatCompRef ? StatCompRef->FinalHP : -1.f);

	UpdateWalkSpeedByState();

	OnCharacterRevive.Broadcast(TeamIndex);*/

	if (GetMesh())
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetHiddenInGame(false);
		GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		// ThirdPerson 기본 메쉬 위치/회전
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	if (StatCompRef)
	{
		StatCompRef->SetCurrentHPPercent(0.5f);
	}

	// 지금 단계에서는 무적 버프 넣지 않기
	// BuffCompRef->AddBuff(InvincBuff); 제거

	UpdateWalkSpeedByState();

	UE_LOG(LogProject_GemCoop, Warning, TEXT("Revived. HP: %.1f / %.1f, ReviveCount: %d"),
		StatCompRef ? StatCompRef->CurrentHP : -1.f,
		StatCompRef ? StatCompRef->FinalHP : -1.f,
		ReviveCount
	);

	OnCharacterRevive.Broadcast(TeamIndex);
}

void AProject_GemCoopCharacter::UpdateFormationRole()
{
	// 파티 평균 위치에서 가장 가까운 플레이어 = Support
	  // 가장 앞쪽(적 방향) 플레이어 = Tanker
	  // 나머지 = Dealer
	  // ... 역할 계산 후
	OnFormationRoleChanged.Broadcast(FormationPosition);

}

bool AProject_GemCoopCharacter::IsInFormationRange() const
{
	TArray<AProject_GemCoopCharacter*> Party = GetPartyMembers();
	
	if (Party.Num() < 2)
	{
		return false;
	}

	for (AProject_GemCoopCharacter* Other : Party)
	{
		if (Other == this)
		{
			continue;
		}

		float Dist = FVector::Dist(GetActorLocation(), Other->GetActorLocation());

		if (Dist < 3000.f || Dist > 7000.f)
		{
			return false;
		}

		FVector ToOther = (Other->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), ToOther)));

		if (Angle > 20.f)
		{
			return false;
		}
	}
	return true;
}

void AProject_GemCoopCharacter::PerformDash()
{
	bIsDash = true;
	UpdateWalkSpeedByState();
}

void AProject_GemCoopCharacter::DashEnd()
{
	bIsDash = false;
	UpdateWalkSpeedByState();
}

void AProject_GemCoopCharacter::OnBasicAttackInput()
{
	BasicAttack();
}

void AProject_GemCoopCharacter::BasicAttack()
{
	if (!bIsAlive)
	{
		return;
	}

	if (BasicAttackCooldownTimer > 0.0f)
	{
		return;
	}

	FVector MouseWorldLocation;

	if (!GetMouseWorldLocation(MouseWorldLocation))
	{
		return;
	}

	FVector AttackDirection = MouseWorldLocation - GetActorLocation();
	AttackDirection.Z = 0.0f;

	if (AttackDirection.IsNearlyZero())
	{
		return;
	}

	AttackDirection.Normalize();

	if (ProjectileClass)
	{
		FireProjectileAttack(AttackDirection);
	}
	else
	{
		MeleeAttack(AttackDirection);
	}

	BasicAttackCooldownTimer = BasicAttackCooldown;
}

void AProject_GemCoopCharacter::FireProjectileAttack(const FVector& AttackDirection)
{
	if (!ProjectileClass)
	{
		return;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	FVector SpawnLocation = GetActorLocation() + AttackDirection * 80.f + FVector(0.0f, 0.0f, 50.f);

	FRotator SpawnRotation = AttackDirection.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AProject_GemCoopProjectile* Projectile = World->SpawnActor<AProject_GemCoopProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Projectile)
	{
		Projectile->InitializeProjectile(BasicAttackDamage, AttackDirection, this);
	}
}

void AProject_GemCoopCharacter::MeleeAttack(const FVector& AttackDirection)
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, 50.f);
	FVector End = Start + AttackDirection * BasicAttackRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult HitResult;

	bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Pawn, Params);

	if (!bHit || !HitResult.GetActor())
	{
		return;
	}

	UGameplayStatics::ApplyDamage(HitResult.GetActor(), BasicAttackDamage, GetController(), this, nullptr);

	UE_LOG(LogProject_GemCoop, Warning, TEXT("MeleeAttack Hit: %s, Damage: %.1f"),
		*GetNameSafe(HitResult.GetActor()),
		BasicAttackDamage
	);
}

void AProject_GemCoopCharacter::UpdateAimToMouse()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	FVector MouseWorldLocation;

	if (!GetMouseWorldLocation(MouseWorldLocation))
	{
		return;
	}

	FVector Direction = MouseWorldLocation - GetActorLocation();
	Direction.Z = 0.0f;

	if (Direction.IsNearlyZero())
	{
		return;
	}

	FRotator TargetRotation = Direction.Rotation();
	SetActorRotation(FRotator(0.0f, TargetRotation.Yaw, 0.0f));
}

bool AProject_GemCoopCharacter::GetMouseWorldLocation(FVector& OutWorldLocation) const
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (!PC)
	{
		return false;
	}

	FHitResult HitResult;

	bool bHit = PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	if (bHit)
	{
		OutWorldLocation = HitResult.ImpactPoint;
		return true;
	}

	FVector WorldLocation;
	FVector WorldDirection;

	if (!PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		return false;
	}

	OutWorldLocation = WorldLocation + WorldDirection * MouseTraceDistance;
	return true;
}

float AProject_GemCoopCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!bIsAlive || !StatCompRef || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	float ModifiedDmg = DamageAmount * ZoneDMGModifier;

	if (BuffCompRef && BuffCompRef->ShieldAmount > 0.0f)
	{
		BuffCompRef->AbsorbDamage(ModifiedDmg);
	}

	if (ModifiedDmg <= 0.0f)
	{
		return 0.0f;
	}

	float DEF = FMath::Max(0.0f, StatCompRef->FinalDEF * ZoneDEFModifier);
	float DefReduction = DEF / (DEF + 100.f);
	float FinalDmg = FMath::Max(0.0f, ModifiedDmg * (1.f - DefReduction));

	StatCompRef->ApplyDamage(FinalDmg);

	return FinalDmg;
}

void AProject_GemCoopCharacter::OnDeath()
{
	if (!bIsAlive)
	{
		return;
	}

	bIsAlive = false;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GetMesh()->SetSimulatePhysics(true); 레그돌

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}
	/*auto* GM = Cast<AProject_GemCoopGameMode>(UGameplayStatics::GetGameMode(this));
	
	if (GM)
	{
		GM->OnPlayerDeath(TeamIndex);
	}

	OnCharacterDeath.Broadcast(TeamIndex);

	UpdateWalkSpeedByState();

	if (ReviveCount > 0)
	{
		FTimerHandle ReviveTimer;
		GetWorldTimerManager().SetTimer(ReviveTimer, this, &AProject_GemCoopCharacter::Revive, 5.f, false);
	}
	else
	{
		if (GM)
		{
			GM->OnPlayerPermanentDeath(TeamIndex);
		}
	}*/

	OnCharacterDeath.Broadcast(TeamIndex);

	UpdateWalkSpeedByState();

	if (ReviveCount > 0)
	{
		UE_LOG(LogProject_GemCoop, Warning, TEXT("Character dead. Revive scheduled. ReviveCount: %d"), ReviveCount);

		FTimerHandle ReviveTimer;
		GetWorldTimerManager().SetTimer(
			ReviveTimer,
			this,
			&AProject_GemCoopCharacter::Revive,
			5.f,
			false
		);

		return;
	}

	AProject_GemCoopGameMode* GM = Cast<AProject_GemCoopGameMode>(UGameplayStatics::GetGameMode(this));

	if (GM)
	{
		GM->OnPlayerPermanentDeath(TeamIndex);
	}
}

void AProject_GemCoopCharacter::OnHPChanged(float CurrentHP, float MaxHP)
{
}

void AProject_GemCoopCharacter::OnBurnDamage(float Damage)
{
	//TakeDamage(Damage, FDamageEvent(), nullptr, nullptr);
}

void AProject_GemCoopCharacter::OnCastingStarted(float CastTime)
{
	UpdateWalkSpeedByState();
}

void AProject_GemCoopCharacter::OnCastingCancelled()
{
	UpdateWalkSpeedByState();
}

float AProject_GemCoopCharacter::GetCurrentHP() const
{
	return StatCompRef ? StatCompRef->CurrentHP : 0.0f;
}

float AProject_GemCoopCharacter::GetMaxHP() const
{
	return StatCompRef ? StatCompRef->FinalHP : 0.f;
}

float AProject_GemCoopCharacter::GetHPPercent() const
{
	if (!StatCompRef)
	{
		return 0.f;
	}

	return StatCompRef->GetCurrentHPPercent();
}

void AProject_GemCoopCharacter::OnCastingCompleted()
{
	UpdateWalkSpeedByState();
}

void AProject_GemCoopCharacter::OnDashInput()
{
	if (!bIsAlive)
	{
		return;
	}

	if (DashStamina <= 0.0f || bDashLocked)
	{
		return;
	}

	if (CombatCompRef && CombatCompRef->bIsCasting)
	{
		CombatCompRef->CancelCasting();
	}

	bDashInput = true;
	bIsDash = true;

	UpdateWalkSpeedByState();
}

void AProject_GemCoopCharacter::OnDashInputEnd()
{
	bDashInput = false;
	bDashLocked = false;
	
	if (DashStamina > 10.f)
	{
		bDashLocked = false;
	}

	UpdateWalkSpeedByState();
}

void AProject_GemCoopCharacter::OnGemQ()
{
	if (GemCompRef)
	{
		GemCompRef->UseGem(0);
	}
}

void AProject_GemCoopCharacter::OnGemW()
{
	if (GemCompRef)
	{
		GemCompRef->UseGem(1);
	}
}

void AProject_GemCoopCharacter::OnGemE()
{
	if (GemCompRef)
	{
		GemCompRef->UseGem(2);
	}
}

void AProject_GemCoopCharacter::OnFusionInput()
{
	if (GemCompRef)
	{
		GemCompRef->StartFusionAttempt(0);
	}
}

void AProject_GemCoopCharacter::OnExchangeInput()
{
}

void AProject_GemCoopCharacter::OnUltimateInput()
{
	if (!EnergySYCompRef)
	{
		return;
	}

	if (!EnergySYCompRef->bUltimateReady)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ultimate not ready. Gauge: %.1f%%"),
			EnergySYCompRef->GetUltGaugePercent() * 100.0f
		);
		return;
	}

	EnergySYCompRef->ConsumeUltGauge();

	UE_LOG(LogTemp, Warning, TEXT("Ultimate Activated!"));
}

void AProject_GemCoopCharacter::OnInteractInput()
{
}

TArray<AProject_GemCoopCharacter*> AProject_GemCoopCharacter::GetPartyMembers() const
{
	TArray<AProject_GemCoopCharacter*> Result;
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(), AProject_GemCoopCharacter::StaticClass(), Found);
	for (AActor* A : Found)
		if (auto* C = Cast<AProject_GemCoopCharacter>(A))
			Result.Add(C);
	return Result;
}

void AProject_GemCoopCharacter::UpdateWalkSpeedByState()
{
	if (!GetCharacterMovement())
	{
		return;
	}

	float BaseSpeed = StatCompRef ? StatCompRef->FinalSPD : 500.f;

	if (!bIsAlive)
	{
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		return;
	}

	if (bIsDash)
	{
		GetCharacterMovement()->MaxWalkSpeed = 900.f;
		return;
	}

	if (CombatCompRef && CombatCompRef->bIsCasting)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * 0.5f;
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
}
