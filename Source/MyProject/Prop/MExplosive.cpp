// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MExplosive.h"
#include "Health/MHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/SphereComponent.h"

AMExplosive::AMExplosive() {
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetSimulatePhysics(false);
	RootComponent = Mesh;

	Health = CreateDefaultSubobject<UMHealthComponent>(TEXT("Health"));
	Health->SetMaxHealth(60);

	ExplosionArea = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionArea"));
	ExplosionArea->SetSphereRadius(150);
	ExplosionArea->SetupAttachment(RootComponent);
	ExplosionDamageAmount = 60;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void AMExplosive::BeginPlay() {
	Super::BeginPlay();

	Health->OnHealthDepleted.AddDynamic(this, &AMExplosive::OnHealthDepleted);
}

void AMExplosive::OnHealthDepleted(UMHealthComponent* HealthComp, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) {
	if (Role == ROLE_Authority) {
		TArray<AActor*> DamageIgnoredActors;
		DamageIgnoredActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamageAmount, GetActorLocation(), ExplosionArea->GetScaledSphereRadius(),
											ExplosionDamageType, DamageIgnoredActors, DamageCauser, InstigatedBy, true);
// 		Mesh->SetSimulatePhysics(true);		
	}

	Mesh->SetVisibility(false, true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExplosionArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (GetNetMode() != NM_DedicatedServer) {
// 		if (ExplodedMaterial) {
// 			Mesh->SetMaterial(0, ExplodedMaterial);
// 		}

		if (ExplosionFX) {
			FRotator RotationFX = Mesh->GetComponentRotation();
			RotationFX.Roll = FMath::RandRange(-180, 180);
			UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, Mesh->GetComponentLocation(), RotationFX);
		}
	}
}
