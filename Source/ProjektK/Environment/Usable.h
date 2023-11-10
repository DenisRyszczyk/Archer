// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Usable.generated.h"

class USphereComponent;

USTRUCT(BlueprintType)
struct FUsableData : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText BuffName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Price;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickedUp, TSubclassOf<AUsable>, UsableClass);

UCLASS()
class PROJEKTK_API AUsable : public AActor
{
	GENERATED_BODY()
	
public:	
	AUsable();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USphereComponent* OverlapSphere;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FDataTableRowHandle Row;
	UPROPERTY(BlueprintAssignable)
	FOnPickedUp OnPickedUp;

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintImplementableEvent)
	void ApplyEffect();
	UFUNCTION()
	void HoverAndRotate(float DeltaSeconds);
	UPROPERTY()
	float T; // Used for hover and rotate function, collects time.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Moving")
	float RotationSpeed = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Moving")
	float HoverAltitude = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Moving")
	float HoverSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	bool bHoveringAndRotating = true;
public:	
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure)
	int32 GetPrice();
};
