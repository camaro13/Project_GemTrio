// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDeath, int32, TeamIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterRevive, int32, TeamIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnZoneChanged, EArenaZone, OldZone, EArenaZone, NewZone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFormationRoleChanged, EFormationRole, NewRole);

class UProject_GemCoopStatComponent;
class UProject_GemCoopTraitComponent;
class UProject_GemCoopBuffComponent;
class UProject_GemCoopCombatComponent;
class UProject_GemCoopGemComponent;
class UProject_GemCoopEnergySYComponent;
class UProject_GemCoopFusionSYComponent;
class AProject_GemCoopProjectile;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AProject_GemCoopCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DashAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* Gem1Action;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* Gem2Action;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* Gem3Action;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* UltAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

public:

	/** Constructor */
	AProject_GemCoopCharacter();	

	UPROPERTY(BlueprintAssignable)
	FOnCharacterDeath OnCharacterDeath;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterRevive OnCharacterRevive;

	UPROPERTY(BlueprintAssignable)
	FOnZoneChanged OnZoneChanged;

	UPROPERTY(BlueprintAssignable)
	FOnFormationRoleChanged OnFormationRoleChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProject_GemCoopStatComponent* StatCompRef = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProject_GemCoopTraitComponent* TraitCompRef = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProject_GemCoopBuffComponent* BuffCompRef = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProject_GemCoopCombatComponent* CombatCompRef = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProject_GemCoopGemComponent* GemCompRef = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProject_GemCoopEnergySYComponent* EnergySYCompRef = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProject_GemCoopFusionSYComponent* FusionSYCompRef = nullptr;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsAlive = true;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	int32 ReviveCount = 3;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	int32 TeamIndex = 0;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EArenaZone CurrentZone = EArenaZone::Safe;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EFormationRole FormationPosition = EFormationRole::Dealer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float DashStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsDash = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bDashInput = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bDashLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BasicAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GemCoop|Combat")
	TSubclassOf<AProject_GemCoopProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GemCoop|Combat")
	float BasicAttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GemCoop|Combat")
	float BasicAttackCooldown = 0.35f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GemCoop|Combat")
	float BasicAttackCooldownTimer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GemCoop|Combat")
	float BasicAttackRange = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GemCoop|Combat")
	float MouseTraceDistance = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|BasicAttack")
	float BasicAttackSpawnDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Aim")
	float AimTraceDistance = 100000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Aim")
	float AimYawSendThreshold = 1.0f;

public:
	UFUNCTION(BlueprintPure, Category = "GemCoop|HP")
	float GetCurrentHP() const;

	UFUNCTION(BlueprintPure, Category = "GemCoop|HP")
	float GetMaxHP() const;

	UFUNCTION(BlueprintPure, Category = "GemCoop|HP")
	float GetHPPercent() const;

	UFUNCTION(BlueprintCallable, Category = "GemCoop|Combat")
	void OnCastingCompleted();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyZoneEffect(EArenaZone Zone);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Revive();

	UFUNCTION(BlueprintCallable, Category = "Formation")
	void UpdateFormationRole();

	UFUNCTION(BlueprintPure, Category = "Formation")
	bool IsInFormationRange() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void PerformDash();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void DashEnd();

	/*UFUNCTION(BlueprintCallable, Category = "GemCoop|Combat")
	void OnBasicAttackInput();*/

	UFUNCTION(BlueprintCallable, Category = "GemCoop|Combat")
	void BasicAttack();

	UFUNCTION(BlueprintCallable, Category = "GemCoop|Combat")
	void FireProjectileAttack(const FVector& AttackDirection);

	UFUNCTION(BlueprintCallable, Category = "GemCoop|Combat")
	void MeleeAttack(const FVector& AttackDirection);

	/*UFUNCTION(BlueprintCallable, Category = "GemCoop|Aim")
	void UpdateAimToMouse();*/

	UFUNCTION(BlueprintCallable, Category = "GemCoop|Aim")
	bool GetMouseWorldLocation(FVector& OutWorldLocation) const;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RequestBasicAttack(FVector AttackDirection);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RequestSetAimYaw(float NewYaw);

protected:
	UFUNCTION(Server, Reliable)
	void ServerRequestBasicAttack(FVector_NetQuantizeNormal AttackDirection);

	UFUNCTION(Server, Unreliable)
	void ServerSetAimYaw(float NewYaw);

	void BasicAttack_ServerOnly(FVector AttackDirection);

	bool GetMouseAimDirection(FVector& OutDirection) const;
	void UpdateAimToMouse();

protected:
	UFUNCTION()
	void OnBasicAttackInput();

private:
	float LastSentAimYaw = 0.0f;
	bool bHasSentAimYaw = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Gem")
	bool RequestUseGemSlot(int32 SlotIndex);

protected:
	UFUNCTION(Server, Reliable)
	void ServerRequestUseGemSlot(int32 SlotIndex);

	bool UseGemSlot_ServerOnly(int32 SlotIndex);

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	float ZoneDEFModifier = 1.0f;
	float ZoneDMGModifier = 1.0f;
	FTimerHandle TimerHandle;

	UFUNCTION()
	void OnDeath();

	UFUNCTION()
	void OnHPChanged(float CurrentHP, float MaxHP);

	UFUNCTION()
	void OnBurnDamage(float Damage);

	UFUNCTION()
	void OnCastingStarted(float CastTime);

	UFUNCTION()
	void OnCastingCancelled();

	void OnDashInput();
	void OnDashInputEnd();
	void OnGemQ(); 
	void OnGemW(); 
	void OnGemE(); 
	void OnFusionInput();
	void OnExchangeInput();
	void OnUltimateInput();
	void OnInteractInput();

	TArray<AProject_GemCoopCharacter*> GetPartyMembers() const;

private:
	void UpdateWalkSpeedByState();
};

