// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MPowerup.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/PointLightComponent.h"
#include "MyProject.h"
#include "Player/Character/MCharacter.h"


AMPowerup::AMPowerup() {
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Sphere->SetSphereRadius(50.f);
	RootComponent = Sphere;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeLocation(FVector(0, 0, 40));
	Mesh->SetupAttachment(RootComponent);

	FloorPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	FloorPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FloorPlane->SetRelativeLocation(FVector(0, 0, 1));
	FloorPlane->SetupAttachment(RootComponent);

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetAttenuationRadius(65.f);
	Light->SetCastShadows(false);
	Light->SetRelativeLocation(FVector(0, 0, 40));
	Light->SetupAttachment(RootComponent);

	Rotating = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Rotating"));

	TimeToRespawn = 15.f;
}

void AMPowerup::NotifyActorBeginOverlap(AActor* OtherActor) {
	Super::NotifyActorBeginOverlap(OtherActor);

	OnPickup(Cast<AMCharacter>(OtherActor));
	Despawn();
	GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &AMPowerup::Respawn, TimeToRespawn);
}

void AMPowerup::OnPickup(AMCharacter* Character) {
	PickupEvent(Character);
}

void AMPowerup::Despawn() {
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetVisibility(false);
	Light->SetVisibility(false);
}

void AMPowerup::Respawn() {
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetVisibility(true);
	Light->SetVisibility(true);
}
