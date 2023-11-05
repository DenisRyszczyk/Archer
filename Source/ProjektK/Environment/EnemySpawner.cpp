// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "ProjektK/Characters/Enemy.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemySpawner::AddKilledEnemy()
{
	KilledEnemyNumThisWave++;

	if (KilledEnemyNumThisWave >= EnemyNumThisWave)
	{
		OnWaveCleared.Broadcast();
	}
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::StartWave(int32 NewWaveNum)
{
	WaveNum = NewWaveNum;
	if (EnemyQueue.Find(WaveNum) && !EnemyQueue.Find(WaveNum)->EnemiesInWave.IsEmpty())
	{
		KilledEnemyNumThisWave = 0;
		EnemyNumThisWave = EnemyQueue.Find(WaveNum)->EnemiesInWave.Num();
		GetWorld()->GetTimerManager().SetTimer(Timer, this, &AEnemySpawner::SpawnEnemy, SpawnRate, true);
	}
}

void AEnemySpawner::SpawnEnemy()
{
	if (!EnemyQueue.Find(WaveNum)->EnemiesInWave.IsEmpty())
	{
		TSubclassOf<AEnemy> SpawnClass = EnemyQueue.Find(WaveNum)->EnemiesInWave[0];
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		if (SpawnClass)
		{
			AEnemy* SpawnedEnemy = GetWorld()->SpawnActor<AEnemy>(SpawnClass, GetActorTransform(), Params);
			if (SpawnedEnemy)
			{
				SpawnedEnemy->OnDied.AddDynamic(this, &AEnemySpawner::AddKilledEnemy);
			}
		}
		
		EnemyQueue.Find(WaveNum)->EnemiesInWave.RemoveAt(0);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(Timer);
	}
}