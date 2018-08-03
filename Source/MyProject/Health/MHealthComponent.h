// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UMHealthComponent*, HealthComp, float, Health, float, Damage, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHealthDepletedSignature, UMHealthComponent*, HealthComp, float, Damage, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UMHealthComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UMHealthComponent();


	//////////////////////////////////////////////////////////////////////////
	// Health
protected:
	/** [Replicated] Current health value */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Health", Meta = (ClampMin = 0))
	float Health;

	/** Maximum health value */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", Meta = (ClampMin = 1))
	float MaxHealth;


	//////////////////////////////////////////////////////////////////////////
	// Events
public:
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount, TSubclassOf<UDamageType> HealType, class AController* InstigatedBy, AActor* HealCauser);

	/** [Server + Local] Called when health changes, either increases or decreases */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	/** [Server + Local] Called when health increases */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealed;

	/** [Server + Local] Called when health decreases */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnDamaged;

	/** [Server + Local] Called when health is depleted */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthDepletedSignature OnHealthDepleted;

protected:
	/** Register damage events */
	virtual void BeginPlay() override;

	/** [Server] Receive damage and update health */
	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
						 class AController* InstigatedBy, AActor* DamageCauser);

	/** [Multicast] trigger events health update events for everyone */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastTriggerHealthEvents(float NewHealth, float PrevHealth, float Damage, const class UDamageType* DamageType,
									  class AController* InstigatedBy, AActor* DamageCauser);

	//////////////////////////////////////////////////////////////////////////
	// Accessor and mutators
public:

	FORCEINLINE bool IsAlive() const { return Health > 0.f; }

	void SetHealth(float NewHealth);
	void SetMaxHealth(float NewMaxHealth);


	//////////////////////////////////////////////////////////////////////////
	// Replication
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
