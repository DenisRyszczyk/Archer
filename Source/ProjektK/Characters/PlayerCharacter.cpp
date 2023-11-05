// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjektK/Weapons/Arrow.h"
#include "NiagaraComponent.h"

APlayerCharacter::APlayerCharacter()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	InvincibilityParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Invincibility Particle"));
	InvincibilityParticle->SetupAttachment(GetMesh());
	InvincibilityParticle->bAutoActivate = false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnGoldUpdated.Broadcast(Gold);
	FActorSpawnParameters Params;
	Params.Owner = this;
	Bow = GetWorld()->SpawnActor<AActor>(BowClass, GetActorTransform(), Params);
	if (Bow)
	{
		Bow->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "bow_socket");
	}
}

void APlayerCharacter::HandleMovementInput(float X, float Y)
{
	FVector RightDirection = FVector(0, 1, 0);//UKismetMathLibrary::GetRightVector(FRotator(0.f, GetControlRotation().Yaw, GetControlRotation().Roll));
	FVector ForwardDirection = FVector(1, 0, 0);//UKismetMathLibrary::GetForwardVector(FRotator(0.f, GetControlRotation().Yaw, 0.f));

	APawn::AddMovementInput(RightDirection, X);
	APawn::AddMovementInput(ForwardDirection, Y);
}

void APlayerCharacter::ChangeStatus(EPlayerStatus NewStatus)
{
	if (PlayerStatus == EPlayerStatus::EPS_Shooting && NewStatus == EPlayerStatus::EPS_Pulling)
	{
		bAnotherPullQueued = true;
		return;                   // If shooting montage plays, que another pull animation.
	}

	bAnotherPullQueued = false;
	switch (NewStatus)
	{
	case EPlayerStatus::EPS_Normal:
		if (PlayerStatus == EPlayerStatus::EPS_Ready)
		{
			ShootArrow();
			break;
		}
		else if(GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->StopAllMontages(0.5f);
			RemoveArrowInHand();
		}
		PlayerStatus = NewStatus;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		break;

	case EPlayerStatus::EPS_Pulling:
		PlayerStatus = NewStatus;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
		if (GetMesh()->GetAnimInstance() && PullMontage)
		{
			GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::CallbackPullMontageFinished);
			GetMesh()->GetAnimInstance()->Montage_Play(PullMontage, FireRate);
			OnBowStartedPulling.Broadcast(FireRate);
		}
		GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &APlayerCharacter::CallbackPullMontageFinished);
		break;

	case EPlayerStatus::EPS_Ready:
		PlayerStatus = NewStatus;
		break;
	case EPlayerStatus::EPS_Shooting:
		PlayerStatus = NewStatus;
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		break;
	case EPlayerStatus::EPS_Dead:
		PlayerStatus = NewStatus;
		Die();
		break;
	}
}

void APlayerCharacter::CallbackPullMontageFinished(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != PullMontage)
	{
		return;
	}
	
	if (!bInterrupted)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, "Ready to shoot");
		PlayerStatus = EPlayerStatus::EPS_Ready; // when pull montage is finished set status to ready.
	}
}

void APlayerCharacter::CallbackShootMontageFinished(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != ShootMontage)
	{
		return;
	}

	if (bAnotherPullQueued)
	{
		PlayerStatus = EPlayerStatus::EPS_Pulling;
		ChangeStatus(PlayerStatus);
	}
	else
	{
		ChangeStatus(EPlayerStatus::EPS_Normal);
	}
}

void APlayerCharacter::ShootArrow()
{
	if (PlayerStatus != EPlayerStatus::EPS_Ready)
	{
		return;
	}
	if (GetMesh()->GetAnimInstance() && ShootMontage)
	{
		GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::CallbackShootMontageFinished);
		GetMesh()->GetAnimInstance()->Montage_Play(ShootMontage, FireRate);
		GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &APlayerCharacter::CallbackShootMontageFinished);
		OnBowShot.Broadcast();
		if (Arrow)
		{
			Arrow->SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, Arrow->GetActorLocation().Z) + (GetActorForwardVector() * 95.f));
			Arrow = nullptr;
		}
	}
	ChangeStatus(EPlayerStatus::EPS_Shooting);	
}

void APlayerCharacter::SpawnArrowInHand()
{
	if (Arrow)
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.Owner = this;
	Arrow = GetWorld()->SpawnActor<AArrow>(ArrowClass, GetActorTransform(), Params);
	if (Arrow)
	{
		Arrow->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "arrow_socket");
	}
}

void APlayerCharacter::RemoveArrowInHand()
{
	if (Arrow)
	{
		Arrow->Destroy();
		Arrow = nullptr;
	}
}

void APlayerCharacter::RotateCharacterTowardsCursor()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector CursorLocation = FVector(0);
	FHitResult HitResult = FHitResult();
	if (!Controller)
	{
		return; // Abort if no controller found.
	}

	PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, false, HitResult);
	CursorLocation = HitResult.Location;
	
	FRotator FinalRotation = FRotator(0);
	FinalRotation = FRotator(0.f, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CursorLocation).Yaw, 0.f); // Calculate final rotation.

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), FinalRotation, DeltaTime, RotationInterpSpeed)); // Slowly turn player toward cursor location.
}

void APlayerCharacter::IncreaseFireRate(float DeltaFireRate)
{
	FireRate += DeltaFireRate;
}

void APlayerCharacter::PredictArrowPath()
{
	if (!Arrow || !Bow)
	{
		return;
	}
	FPredictProjectilePathParams Params;
	Params.ProjectileRadius = 10.0f;
	Params.TraceChannel = ECollisionChannel::ECC_WorldDynamic;
	Params.ActorsToIgnore.Add(this);
	Params.ActorsToIgnore.Add(Bow);
	Params.ActorsToIgnore.Add(Arrow);
	Params.bTraceWithCollision = true;
	Params.StartLocation = FVector(FVector(GetActorLocation().X, GetActorLocation().Y, Arrow->GetActorLocation().Z) + GetActorForwardVector() * 95.0f);
	Params.LaunchVelocity = GetActorForwardVector() * Arrow->GetReleaseStrength();
	Params.SimFrequency = 30.0f;
	Params.MaxSimTime = 2.0f;
	Params.DrawDebugType = EDrawDebugTrace::ForOneFrame;

	FPredictProjectilePathResult Results;
	UGameplayStatics::PredictProjectilePath(this, Params, Results);
}

void APlayerCharacter::Die()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	DisableInput(PlayerController);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
}

void APlayerCharacter::TurnOnInvincibility()
{
	bInvincible = true;
	if(InvincibilityParticle) InvincibilityParticle->Activate();
	FTimerHandle Timer = FTimerHandle();
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &APlayerCharacter::TurnOffInvincibility, InvincibilityTime);
}

void APlayerCharacter::TurnOffInvincibility()
{
	bInvincible = false;
	if(InvincibilityParticle) InvincibilityParticle->Deactivate();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PlayerStatus != EPlayerStatus::EPS_Normal && PlayerStatus != EPlayerStatus::EPS_Dead)
	{
		RotateCharacterTowardsCursor();
	}
	if (PlayerStatus == EPlayerStatus::EPS_Ready)
	{
		PredictArrowPath();
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float APlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bInvincible)
	{
		return 0.0f;
	}
	if (CurrentHP <= 0)
	{
		return 0.0f;
	}
	CurrentHP--;
	OnHealthUpdated.Broadcast(HealthSlots, CurrentHP);
	if (CurrentHP <= 0)
	{
		ChangeStatus(EPlayerStatus::EPS_Dead);
		return 0.0f;
	}

	TurnOnInvincibility();
	return 0.0f;
}

void APlayerCharacter::AddGold(int32 AddedGold)
{
	Gold += AddedGold;
	OnGoldUpdated.Broadcast(Gold);
}

void APlayerCharacter::SubtractGold(int32 GoldSubtracted)
{
	Gold -= GoldSubtracted;
	OnGoldUpdated.Broadcast(Gold);
}

bool APlayerCharacter::CanBuy(int32 Price)
{
	return Gold >= Price;
}

void APlayerCharacter::AddHealthSlot(int32 AddedSlots)
{
	HealthSlots += AddedSlots;
	AddHP(AddedSlots);
}

void APlayerCharacter::AddHP(int32 AddedHP)
{
	CurrentHP = FMath::Clamp(CurrentHP + AddedHP, 0, HealthSlots);
	OnHealthUpdated.Broadcast(HealthSlots, CurrentHP);
}

