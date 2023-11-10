// Fill out your copyright notice in the Description page of Project Settings.


#include "PKGameMode.h"
#include "Environment/EnemySpawner.h"
#include "Environment/Usable.h"
#include "Kismet/GameplayStatics.h"

void APKGameMode::GatherAllSpawners()
{
	FindAllActors(GetWorld(), Spawners);
	SecondsLeftToNewWave = BreakTime;
}

void APKGameMode::StartWave()
{
	if (Spawners.IsEmpty())
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(CountdownTimer);
	SecondsLeftToNewWave = BreakTime; // Renew countdown timer
	Wave++;
	GatherSpawnersInvolvedInCurrentWave();
	RemoveBuffs();

	for (auto Spawner : SpawnersInvlovedInCurrentWave)
	{
		Spawner->OnWaveCleared.RemoveDynamic(this, &APKGameMode::AddFinishedSpawner);
		Spawner->OnWaveCleared.AddDynamic(this, &APKGameMode::AddFinishedSpawner);
		Spawner->StartWave(Wave);
	}
	OnWaveStarted.Broadcast(Wave);
	
}

void APKGameMode::EndWave()
{
	FinishedSpawners = 0;
	SpawnersInvlovedInCurrentWave.Empty();
	GetWorld()->GetTimerManager().SetTimer(CountdownTimer, this, &APKGameMode::CountdownTick, 1.0f, true);
	SpawnBuffs();
	OnWaveEnded.Broadcast(Wave);
}

bool APKGameMode::CanStartWaveEarly()
{
	return GetWorld()->GetTimerManager().IsTimerActive(CountdownTimer);
}

void APKGameMode::AddFinishedSpawner()
{
	FinishedSpawners++;
	if (FinishedSpawners >= SpawnersInvlovedInCurrentWave.Num())
	{
		EndWave();
	}
}

void APKGameMode::GatherSpawnersInvolvedInCurrentWave()
{
	if (Spawners.IsEmpty())
	{
		return;
	}

	for (auto Spawner : Spawners)
	{
		if (Spawner->EnemyQueue.Find(Wave))
		{
			SpawnersInvlovedInCurrentWave.Add(Spawner);
		}
		
	}
}

void APKGameMode::CountdownTick()
{
	if (SecondsLeftToNewWave <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimer);
		StartWave();
		return;
	}
	OnCountdownTick.Broadcast(SecondsLeftToNewWave);
	SecondsLeftToNewWave--;
}

void APKGameMode::SpawnBuffs()
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (SpawnableBuffs.IsEmpty())
	{
		return;
	}

	for (int32 i = 0; i < BuffsToSpawnNum; i++)
	{
		if(!SpawnableBuffs.IsEmpty())
		{
			int32 RandomBuff = FMath::RandRange(0, SpawnableBuffs.Num()-1);
			TSubclassOf<AUsable> NewBuffClass = SpawnableBuffs[RandomBuff];
				
			SpawnedBuffsBuffer.Add(NewBuffClass);
			SpawnableBuffs.RemoveSingle(NewBuffClass); // Adds random buff to buffer in order not to let a buff spawn multiple times even if there is only 1 instance in array.
		
			if (NewBuffClass)
			{

				AUsable* NewBuff = GetWorld()->SpawnActor<AUsable>(NewBuffClass, FVector(0, i * 500 - BuffsToSpawnNum / 2 * 500, 0), FRotator(0), Params);
				if (NewBuff)
				{
					NewBuff->OnPickedUp.AddDynamic(this, &APKGameMode::RemoveBuffFromSpawnableBuffs);
					SpawnedBuffs.Add(NewBuff);
				}
			}
		}
	}

	SpawnableBuffs.Append(SpawnedBuffsBuffer);
	SpawnedBuffsBuffer.Empty(); // Add contents of buffer back into array.
}

void APKGameMode::RemoveBuffs()
{
	if (SpawnedBuffs.IsEmpty())
	{
		return;
	}
	for (auto Buff : SpawnedBuffs)
	{
		if(Buff) Buff->Destroy();
	}
	SpawnedBuffs.Empty();
}

void APKGameMode::RemoveBuffFromSpawnableBuffs(TSubclassOf<AUsable> UsableClass)
{
	if (SpawnableBuffs.Contains(UsableClass))
	{
		SpawnableBuffs.RemoveSingle(UsableClass);
	}
}
