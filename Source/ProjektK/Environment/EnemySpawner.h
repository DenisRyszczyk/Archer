// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AEnemy;

USTRUCT(BlueprintType)
struct FEnemiesInWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AEnemy>> EnemiesInWave;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaveCleared);

UCLASS()
class PROJEKTK_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawner();
	UPROPERTY(BlueprintAssignable)
	FOnWaveCleared OnWaveCleared;
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
	int32 WaveNum = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float SpawnRate = 2.f;
	UPROPERTY()
	FTimerHandle Timer = FTimerHandle();
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TMap<int32, FEnemiesInWave> EnemyQueue;
	UPROPERTY(BlueprintReadOnly)
	TArray<AEnemy*> SpawnedEnemies;
	UPROPERTY(BlueprintReadOnly)
	int32 EnemyNumThisWave;
	UPROPERTY(BlueprintReadOnly)
	int32 KilledEnemyNumThisWave;
	UFUNCTION()
	void AddKilledEnemy();
public:	
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void StartWave(int32 NewWaveNum);

};
