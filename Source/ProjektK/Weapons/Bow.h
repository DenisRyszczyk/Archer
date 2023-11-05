// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bow.generated.h"

class APlayerCharacter;

UCLASS()
class PROJEKTK_API ABow : public AActor
{
	GENERATED_BODY()
	
public:	
	ABow();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* ActorRoot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* BowMesh;
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* BowPullMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* BowShootMontage;

	UFUNCTION(BlueprintCallable)
	void PullBow(float TargetFireRate);
	UFUNCTION(BlueprintCallable)
	void Shoot();

	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* PlayerCharacter = nullptr;

public:	
	virtual void Tick(float DeltaTime) override;

};
