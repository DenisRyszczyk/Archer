// Fill out your copyright notice in the Description page of Project Settings.


#include "Bow.h"
#include "ProjektK/Characters/PlayerCharacter.h"

ABow::ABow()
{
	PrimaryActorTick.bCanEverTick = true;

	ActorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Actor Root"));
	SetRootComponent(ActorRoot);
	BowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Bow Mesh"));
	BowMesh->SetupAttachment(ActorRoot);
}

void ABow::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (PlayerCharacter)
	{
		PlayerCharacter->OnBowStartedPulling.AddDynamic(this, &ABow::PullBow);
		PlayerCharacter->OnBowShot.AddDynamic(this, &ABow::Shoot);
	}
}

void ABow::PullBow(float TargetFireRate)
{
	if (BowMesh->GetAnimInstance())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Pulling");
		BowMesh->GetAnimInstance()->Montage_Play(BowPullMontage, TargetFireRate * 1.5f);
	}
}

void ABow::Shoot()
{
	if (BowMesh->GetAnimInstance())
	{
		BowMesh->GetAnimInstance()->Montage_Play(BowShootMontage);
	}
}

void ABow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

