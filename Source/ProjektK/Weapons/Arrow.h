// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Arrow.generated.h"

class UProjectileMovementComponent;
class APlayerCharacter;
class USphereComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class PROJEKTK_API AArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	AArrow();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USphereComponent* ActorRoot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* ArrowMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UNiagaraComponent* ImpactVFX;
protected:
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* PlayerCharacter = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float ReleaseStrength = 500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float ArrowDamage = 10.f;
	UFUNCTION()
	void CallbackOnProjectileStop(const FHitResult& ImpactResult);
	UFUNCTION(BlueprintImplementableEvent)
	void ApplyEffectToHitActor(AActor* ActorToApplyEffect);

	//VFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UNiagaraSystem* GrassImpact;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UNiagaraSystem* DirtImpact;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UNiagaraSystem* WoodImpact;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UNiagaraSystem* FleshImpact;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UNiagaraSystem* MetalImpact;
	UFUNCTION(BlueprintCallable)
	void SpawnImpactVfx(FHitResult HitResult);
public:	
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void ReleaseArrow();
	UFUNCTION()
	float GetReleaseStrength();
};
