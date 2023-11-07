// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHit);

class AUsable;

UCLASS()
class PROJEKTK_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	UPROPERTY(BlueprintAssignable)
	FOnDied OnDied;
	UPROPERTY(BlueprintAssignable)
	FOnHit OnHit;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void Die();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float Health = 10.f;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsDead();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AUsable> GoldClass;
	UPROPERTY(EditDefaultsOnly)
	int32 GoldPiecesLeftUponDying;

	//Attacking
	UFUNCTION(BlueprintCallable)
	void PerformAttack();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TArray<UAnimMontage*> AttackMontages;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	UAnimMontage* HitMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	UAnimMontage* DeathMontage;
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
