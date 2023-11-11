// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerStatus : uint8
{
	EPS_Normal,
	EPS_Ready,
	EPS_Pulling,
	EPS_Shooting,
	EPS_Dead
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBowStartedPulling, float, TargetFireRate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBowShot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldUpdated, int32, GoldAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthUpdated, int32, CurrentHealthSlots, int32, CurrentHealthPoints, int32, OldHealthPoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDied);

class UNiagaraComponent;
class AArrow;

UCLASS()
class PROJEKTK_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	UPROPERTY(BlueprintReadWrite)
	EPlayerStatus PlayerStatus;
	UPROPERTY(BlueprintAssignable)
	FOnBowStartedPulling OnBowStartedPulling;
	UPROPERTY(BlueprintAssignable)
	FOnBowShot OnBowShot;
	UPROPERTY(BlueprintAssignable)
	FOnGoldUpdated OnGoldUpdated;
	UPROPERTY(BlueprintAssignable)
	FOnHealthUpdated OnHealthUpdated;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDied OnPlayerDied;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UNiagaraComponent* InvincibilityParticle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void HandleMovementInput(float X, float Y);
	UFUNCTION(BlueprintCallable)
	void ChangeStatus(EPlayerStatus NewStatus);

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* PullMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DodgeMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* ShootMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages")
	UAnimMontage* HitMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages")
	TSubclassOf<UCameraShakeBase> HitCameraShake;

	UFUNCTION()
	void CallbackPullMontageFinished(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void CallbackShootMontageFinished(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION(BlueprintCallable)
	void ShootArrow();
	UFUNCTION(BlueprintCallable)
	void SpawnArrowInHand();
	UFUNCTION()
	void RemoveArrowInHand();
	UFUNCTION()
	void RotateCharacterTowardsCursor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float RotationInterpSpeed = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float FireRate = 1.f;
	UPROPERTY(BlueprintReadWrite)
	bool bRequestingPulling = false;
	UFUNCTION(BlueprintCallable)
	void IncreaseFireRate(float DeltaFireRate);

	UPROPERTY(BlueprintReadOnly)
	AActor* Bow;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> BowClass;
	UPROPERTY(BlueprintReadOnly)
	AArrow* Arrow;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AArrow> ArrowClass;

	UFUNCTION()
	void PredictArrowPath();

	//Health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	int32 HealthSlots = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	int32 CurrentHP = HealthSlots;
	UFUNCTION(BlueprintCallable)
	void Die();
	UFUNCTION()
	bool IsDead();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bInvincible;
	UFUNCTION(BlueprintCallable)
	void TurnOnInvincibility();
	UFUNCTION(BlueprintCallable)
	void TurnOffInvincibility();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float InvincibilityTime = 2.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Money")
	int32 Gold = 0;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	UFUNCTION(BlueprintCallable)
	void AddGold(int32 AddedGold);
	UFUNCTION(BlueprintCallable)
	void SubtractGold(int32 GoldSubtracted);
	UFUNCTION(BlueprintCallable)
	bool CanBuy(int32 Price);
	UFUNCTION(BlueprintCallable)
	void AddHealthSlot(int32 AddedSlots);
	UFUNCTION(BlueprintCallable)
	void AddHP(int32 AddedHP);
	UFUNCTION()
	TSubclassOf<AArrow> GetArrowClass();
};
