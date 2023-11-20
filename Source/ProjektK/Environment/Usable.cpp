// Fill out your copyright notice in the Description page of Project Settings.


#include "Usable.h"
#include "ProjektK/Characters/PlayerCharacter.h"
#include "Components/SphereComponent.h"

AUsable::AUsable()
{
	PrimaryActorTick.bCanEverTick = true;

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Overlap sphere"));
	SetRootComponent(OverlapSphere);
}

void AUsable::BeginPlay()
{
	Super::BeginPlay();
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AUsable::OnOverlapped);
}

void AUsable::HoverAndRotate(float DeltaSeconds)
{
	T += DeltaSeconds;
	FVector DeltaLoc = FVector(0, 0, FMath::Sin(T * HoverSpeed) * HoverAltitude);
	FRotator DeltaRot = FRotator(0, RotationSpeed, 0);

	AddActorWorldOffset(DeltaLoc);
	AddActorLocalRotation(DeltaRot * DeltaSeconds);
}

void AUsable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bHoveringAndRotating)
	{
		HoverAndRotate(DeltaTime);
	}
}

void AUsable::OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter && PickupConditionMet(OtherActor))
	{

		if (PlayerCharacter->CanBuy(GetPrice()))
		{
			PlayerCharacter->SubtractGold(GetPrice());
			ApplyEffect();
			if (!bIrremovableFromSpawnableBuffs)
			{
				OnPickedUp.Broadcast(this->GetClass());
			}
			Destroy();
		}
	}
}

