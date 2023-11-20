// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Engine/EngineTypes.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ProjektK/Environment/Usable.h"
#include "ProjektK/Weapons/Arrow.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::Die()
{
	for (int32 i = 0; i < GoldPiecesLeftUponDying; i++)
	{
		if (GoldClass)
		{
			GetWorld()->SpawnActor<AUsable>(GoldClass, GetActorTransform());
		}
	}
	DetachFromControllerPendingDestroy();
	GetCapsuleComponent()->DestroyComponent();
	if(GetMesh()->GetAnimInstance() && DeathMontage) GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);
	SetLifeSpan(3.0f);
	OnDied.Broadcast();
}

bool AEnemy::IsDead()
{
	return Health <= 0.0f;
}

void AEnemy::PerformAttack()
{
	if(!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && AActor::GetDistanceTo(UGameplayStatics::GetPlayerCharacter(this, 0)) <= 350.0f && !IsDead())
	{
		if (AttackMontages.IsEmpty())
		{
			return;
		}
		int32 MontageIndex = FMath::RandRange(0, AttackMontages.Num()-1);
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontages[MontageIndex]);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDead())
	{
		return 0.0f;
	}

	Health -= Damage;

	if (HitCameraShake && Cast<AArrow>(DamageCauser))
	{
		UGameplayStatics::PlayWorldCameraShake(this, HitCameraShake, UGameplayStatics::GetPlayerCharacter(this, 0)->GetActorLocation(), 5000.0f, 10000.0f, 1.0f, true);
	}

	OnHit.Broadcast();

	if (IsDead())
	{
		Die();
	}
	else if (GetMesh()->GetAnimInstance() && HitMontage && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitMontage);
	}
	return 0.0f;
}




