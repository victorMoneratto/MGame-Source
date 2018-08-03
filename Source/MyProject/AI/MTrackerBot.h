// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MTrackerBot.generated.h"

UCLASS(Abstract)
class MYPROJECT_API AMTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	AMTrackerBot();

	//////////////////////////////////////////////////////////////////////////
	// Components
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UMHealthComponent* Health;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class USphereComponent* Sphere;


	//////////////////////////////////////////////////////////////////////////
	// Path
protected:
	FVector GetNextPathPoint();

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Path")
	float AcceptanceRadiusSqr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Path")
	float ForceIntensity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Path")
	bool bAccelerationChange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Path")
	float SpeedLimit;


	//////////////////////////////////////////////////////////////////////////
	// Explosion
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	float DeathExplosionDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	float DeathExplosionRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	class UParticleSystem* ExplosionFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	class USoundCue* ExplosionSound;

	

	//////////////////////////////////////////////////////////////////////////
	// Self-Destruct
protected:
	UPROPERTY(ReplicatedUsing=OnRep_TriggerSelfDestruct, BlueprintReadOnly, Category = "SelfDestruct")
	uint32 bTriggeredSelfDestruct : 1;
	
	void TriggerSelfDestruct();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SelfDestruct")
	class USoundCue* SelfDestructTriggerSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SelfDestruct", Meta = (ClampMin = 1))
	int32 NumSelfDestructPulses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SelfDestruct", Meta = (ClampMin = 1))
	float TimePulsingSelfDestruct;

	UFUNCTION()
	void SelfDestructPulse();

	FTimerHandle TimerHandle_SelfDestructPulse;


	//////////////////////////////////////////////////////////////////////////
	// Events
public:
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UFUNCTION()
	void OnDamaged(UMHealthComponent* HealthComp, float InHealth, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnHealthDepleted(UMHealthComponent* HealthComp, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	virtual void BeginPlay() override;

	//////////////////////////////////////////////////////////////////////////
	// Power
protected:
	UFUNCTION()
	void FindGroupForPowerLevel();

	FTimerHandle TimerHandle_FindGroupForPowerLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Power")
	float PowerLevelDamageBonus;

	UPROPERTY(ReplicatedUsing=OnRep_PowerLevel, BlueprintReadOnly, Category = "Power")
	int32 PowerLevel;

	UPROPERTY(BlueprintReadOnly, Category = "Power")
	int32 MaxPowerLevel;

	//////////////////////////////////////////////////////////////////////////
	// Visual
protected:
	UMaterialInstanceDynamic* MaterialInstance;


	//////////////////////////////////////////////////////////////////////////
	// Replication
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION()
	void OnRep_TriggerSelfDestruct();

	UFUNCTION()
	void OnRep_PowerLevel();
};
