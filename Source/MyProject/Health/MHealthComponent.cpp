// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Game/MGameModeBase.h"
#include "Game/MPlayerState.h"

UMHealthComponent::UMHealthComponent() {
	Health = 100.f;
	MaxHealth = 100.f;

	SetIsReplicated(true);
}


void UMHealthComponent::Heal(float HealAmount, TSubclassOf<UDamageType> HealType, class AController* InstigatedBy, AActor* HealCauser) {
	if (GetOwnerRole() == ROLE_Authority) {
		UGameplayStatics::ApplyDamage(GetOwner(), -HealAmount, InstigatedBy, HealCauser, HealType);
	}
}

void UMHealthComponent::BeginPlay() {
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner) {
		Owner->OnTakeAnyDamage.AddDynamic(this, &UMHealthComponent::OnTakeAnyDamage);
	}
}

void UMHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser) {
	// can damage?
	APawn* DamagedPawn = Cast<APawn>(DamagedActor);
	if (DamagedPawn && Instigator) {
		AMGameModeBase* GM = Cast<AMGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GM) {
			AMPlayerState* DamagedPlayer = Cast<AMPlayerState>(DamagedPawn->PlayerState);
			AMPlayerState* InstigatorPlayer = Cast<AMPlayerState>(Instigator->PlayerState);
			if (!GM->CanDealDamage(DamagedPlayer, InstigatorPlayer)) {
				return;
			}
		}
	}

	float PrevHealth = Health;
	SetHealth(Health - Damage);
	if (Health != PrevHealth) {
		MulticastTriggerHealthEvents(Health, PrevHealth, Damage, DamageType, Instigator, DamageCauser);
	}
}

void UMHealthComponent::MulticastTriggerHealthEvents_Implementation(float NewHealth, float PrevHealth, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) {
	// #MoneSafety is it safe to change health for clients here?
	// if we don't, the client's health might be different
	// (this->Health != NewHealth), it is specially confusing
	// on blueprints
	if (GetOwnerRole() != ROLE_Authority) {
		Health = NewHealth;
	}

	OnHealthChanged.Broadcast(this, NewHealth, Damage, DamageType, InstigatedBy, DamageCauser);

	if (NewHealth > PrevHealth) {
		float Healing = -Damage;
		OnHealed.Broadcast(this, NewHealth, Healing, DamageType, InstigatedBy, DamageCauser);
	} else if (NewHealth < PrevHealth) {
		OnDamaged.Broadcast(this, NewHealth, Damage, DamageType, InstigatedBy, DamageCauser);
		if (NewHealth <= 0.f && PrevHealth > 0.f) {
			OnHealthDepleted.Broadcast(this, Damage, DamageType, InstigatedBy, DamageCauser);

			// #MoneResearch is this the best place to warn kill to server?
			if (GetOwnerRole() == ROLE_Authority) {
				AMGameModeBase* GM = GetWorld()->GetAuthGameMode<AMGameModeBase>();
				if (GM) {
					GM->Killed(InstigatedBy, GetOwner()->GetInstigatorController());
				}
			}
		}
	}
}

void UMHealthComponent::SetHealth(float NewHealth) {
	float PrevHealth = Health;
	Health = FMath::Clamp(NewHealth, 0.f, MaxHealth);
}

void UMHealthComponent::SetMaxHealth(float NewMaxHealth) {
	MaxHealth = NewMaxHealth;
	Health = FMath::Clamp(0.f, MaxHealth, NewMaxHealth);
	// trigger health events when changed?
}

void UMHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// #MoneResearch can I drop replicating Health and only update that on MulticastUpdateHealth?
	// what if some player is out of net relevancy? it will probably not be corrected later, right?
	DOREPLIFETIME(UMHealthComponent, Health);
}