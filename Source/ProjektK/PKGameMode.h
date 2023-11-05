// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EngineUtils.h"
#include "PKGameMode.generated.h"

/**
 * 
 */

class AEnemySpawner;
class AUsable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountdownTick, int32, SecondsLeft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, CurrentWave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveEnded, int32, CurrentWave);

UCLASS()
class PROJEKTK_API APKGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintAssignable)
	FOnCountdownTick OnCountdownTick;
	UPROPERTY(BlueprintAssignable)
	FOnWaveStarted OnWaveStarted;
	UPROPERTY(BlueprintAssignable)
	FOnWaveEnded OnWaveEnded;

	UFUNCTION(BlueprintCallable)
	void GatherAllSpawners();

	UFUNCTION(BlueprintCallable)
	void StartWave();
	UFUNCTION(BlueprintCallable)
	void EndWave();
	UPROPERTY(BlueprintReadOnly)
	int32 Wave = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 FinishedSpawners = 0;
	UFUNCTION()
	void AddFinishedSpawner();

	template<typename T>
	void FindAllActors(UWorld* World, TArray<T*>& Out)
	{
		for (TActorIterator<T> It(World); It; ++It)
		{
			Out.Add(*It);
		}
	}
	UPROPERTY()
	TArray<AEnemySpawner*> Spawners;
	UPROPERTY(BlueprintReadOnly)
	TArray<AEnemySpawner*> SpawnersInvlovedInCurrentWave;
	UFUNCTION()
	void GatherSpawnersInvolvedInCurrentWave();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave")
	int32 BreakTime = 15.0f;
	UPROPERTY()
	int32 SecondsLeftToNewWave;

	UPROPERTY()
	FTimerHandle CountdownTimer;
	UFUNCTION(BlueprintCallable)
	void CountdownTick();

	UFUNCTION(BlueprintCallable)
	void SpawnBuffs();
	UFUNCTION(BlueprintCallable)
	void RemoveBuffs();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Buffs")
	int32 BuffsToSpawnNum = 3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Buffs")
	TArray<TSubclassOf<AUsable>> SpawnableBuffs;
	UPROPERTY(BlueprintReadOnly)
	TArray<AUsable*> SpawnedBuffs;
};
