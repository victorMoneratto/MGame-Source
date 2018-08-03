// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MCharacter.generated.h"

UCLASS(Abstract)
class MYPROJECT_API AMCharacter : public ACharacter {
	GENERATED_BODY()

public:
	AMCharacter(const class FObjectInitializer& ObjectInitializer);

	//////////////////////////////////////////////////////////////////////////
	// Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UMHealthComponent* Health;


	//////////////////////////////////////////////////////////////////////////
	// Health
public:
	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	bool bDead;

protected:
	UFUNCTION()
	void OnHealthDepleted(UMHealthComponent* HealthComp, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	//////////////////////////////////////////////////////////////////////////
	// Input
public:
	UFUNCTION(BlueprintCallable, Category = "Character")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void StopFire();

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	float LookUpRate;
	void LookUpAtRate(float Value);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	float TurnRate;
	void TurnAtRate(float Value);

	uint8 bIsAiming : 1;
	void StartAim();
	void StopAim();
	void AimToggle();

	void StartJump();
	void StopJump();

	void StartCrouch();
	void StopCrouch();
	void CrouchToggle();


	//////////////////////////////////////////////////////////////////////////
	// Weapon
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AMWeapon> DefaultWeaponClass;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	class AMWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName RightHandWeaponAttachPoint;

	
	//////////////////////////////////////////////////////////////////////////
	// Events
public:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//////////////////////////////////////////////////////////////////////////
	// Data accessors
public:
	UFUNCTION(BlueprintCallable, Category = "Character")
	FORCEINLINE bool IsAiming() const { return bIsAiming; };

	UFUNCTION(BlueprintCallable, Category = "Character")
	FORCEINLINE class UMHealthComponent* GetHealth() const { return Health; };

	//////////////////////////////////////////////////////////////////////////
	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
