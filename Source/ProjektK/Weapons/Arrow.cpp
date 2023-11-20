// Fill out your copyright notice in the Description page of Project Settings.


#include "Arrow.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "ProjektK/Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Landscape.h"


AArrow::AArrow()
{
	PrimaryActorTick.bCanEverTick = true;

	ActorRoot = CreateDefaultSubobject<USphereComponent>(TEXT("Actor Root"));
	SetRootComponent(ActorRoot);
	ActorRoot->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	ActorRoot->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow Mesh"));
	ArrowMesh->SetupAttachment(ActorRoot);
	ArrowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovementComponent->bAutoActivate = false;
	ProjectileMovementComponent->bInterpMovement = true;
	ProjectileMovementComponent->bInterpRotation = true;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ImpactVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Impact VFX"));
	ImpactVFX->SetupAttachment(ActorRoot);
	ImpactVFX->bAutoActivate = false;
}

void AArrow::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (PlayerCharacter)
	{
		PlayerCharacter->OnBowShot.AddDynamic(this, &AArrow::ReleaseArrow);
	}
	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AArrow::CallbackOnProjectileStop);
}

void AArrow::CallbackOnProjectileStop(const FHitResult& ImpactResult)
{
	ActorRoot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArrowMesh->AddLocalOffset(FVector(0.f, 0.f, 15.f)); // Push arrow into target
	ProjectileMovementComponent->Deactivate();
	AttachToComponent(ImpactResult.GetComponent(), FAttachmentTransformRules::KeepWorldTransform, ImpactResult.BoneName);

	UGameplayStatics::ApplyDamage(ImpactResult.GetActor(), ArrowDamage, nullptr, this, UDamageType::StaticClass());

	if (!ImpactResult.GetComponent() || !ImpactResult.GetActor())
	{
		Destroy();
		return;
	}
	ApplyEffectToHitActor(ImpactResult.GetActor());
	if (ImpactResult.GetComponent()->IsSimulatingPhysics())
	{
		ImpactResult.GetComponent()->AddImpulse(GetActorForwardVector() * 50000.f);
	}
	SetLifeSpan(3.0f);
	SpawnImpactVfx(ImpactResult);
	
}

void AArrow::SpawnImpactVfx(FHitResult HitResult)
{
	EPhysicalSurface Surface = EPhysicalSurface::SurfaceType1;

	ALandscape* Landscape = Cast<ALandscape>(HitResult.GetActor());
	if (Landscape && Landscape->LandscapeMaterial)
	{
		Surface = Landscape->LandscapeMaterial->GetPhysicalMaterial()->SurfaceType; //Check if hit actor is landscape
	}

	else if (HitResult.GetComponent()->GetMaterial(0))
	{
		Surface = HitResult.GetComponent()->GetMaterial(0)->GetPhysicalMaterial()->SurfaceType; // if not then check if material on hit component exists
	}

	else 
	{
		return; // if both checks fail then return so no error occurs
	}

	switch (Surface)
	{
	case EPhysicalSurface::SurfaceType1:
		if(GrassImpact) ImpactVFX->SetAsset(GrassImpact);
		break;
	case EPhysicalSurface::SurfaceType2:
		if(DirtImpact) ImpactVFX->SetAsset(DirtImpact);
		break;
	case EPhysicalSurface::SurfaceType3:
		if(WoodImpact) ImpactVFX->SetAsset(WoodImpact);
		break;
	case EPhysicalSurface::SurfaceType4:
		if(FleshImpact) ImpactVFX->SetAsset(FleshImpact);
		break;
	case EPhysicalSurface::SurfaceType5:
		if(MetalImpact) ImpactVFX->SetAsset(MetalImpact);
		break;
	default:
		if(DirtImpact) ImpactVFX->SetAsset(DirtImpact);
		break;
	}
	ImpactVFX->Activate();
}

void AArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArrow::ReleaseArrow()
{
	if (ProjectileMovementComponent)
	{
		if (PlayerCharacter)
		{
			PlayerCharacter->OnBowShot.RemoveDynamic(this, &AArrow::ReleaseArrow);
		}
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ProjectileMovementComponent->Activate();
		ProjectileMovementComponent->Velocity = PlayerCharacter->GetActorForwardVector() * ReleaseStrength;
	}
}

float AArrow::GetReleaseStrength()
{
	return ReleaseStrength;
}

