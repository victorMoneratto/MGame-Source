// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPowerup.generated.h"

UCLASS(Abstract)
class MYPROJECT_API AMPowerup : public AActor
{
	GENERATED_BODY()
	
public:	
	AMPowerup();


	//////////////////////////////////////////////////////////////////////////
	// Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPointLightComponent* Light;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* FloorPlane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class URotatingMovementComponent* Rotating;


	//////////////////////////////////////////////////////////////////////////
	// Pickup
protected:
	virtual void OnPickup(class AMCharacter* Character);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup", Meta=(DisplayName = "Pick up"))
	void PickupEvent(class AMCharacter* Character);

	void Despawn();

	void Respawn();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	float TimeToRespawn;

	FTimerHandle TimerHandle_Respawn;

	//////////////////////////////////////////////////////////////////////////
	// Events
public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
