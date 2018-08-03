// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MExplosive.generated.h"

UCLASS(Abstract)
class MYPROJECT_API AMExplosive : public AActor {
	GENERATED_BODY()

public:
	AMExplosive();

	//////////////////////////////////////////////////////////////////////////
	// Components
protected:
	/** Explosive body */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;

	/** Health component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UMHealthComponent* Health;

	/** Health component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* ExplosionArea;

	//////////////////////////////////////////////////////////////////////////
	// Explosion
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	float ExplosionDamageAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	TSubclassOf<class UDamageType> ExplosionDamageType;

	/** Material applied on body after explosion */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	class UMaterial* ExplodedMaterial;

	/** Particles for explosion */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	class UParticleSystem* ExplosionFX;


	//////////////////////////////////////////////////////////////////////////
	// Events
protected:
	/** Register health depleted trigger */
	void BeginPlay() override;

	/** Trigger explosion when health is depleted */
	UFUNCTION()
	void OnHealthDepleted(UMHealthComponent* HealthComp, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
