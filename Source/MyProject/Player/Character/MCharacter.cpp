// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "Player/Character/MCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Weapon/MWeapon.h"
#include "Health/MHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/Character/MCharacterMovementComponent.h"
#include "Game/Horde/MHordeMode.h"

AMCharacter::AMCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)) {
	PrimaryActorTick.bCanEverTick = true;

	// #MoneTodo This is a quick implementation of yaw rotation,
	// the character's feet will slide when looking around
	bUseControllerRotationYaw = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPCameraBoom"));
	CameraBoom->SocketOffset = FVector(0, 80, 0);
	CameraBoom->SetRelativeLocation(FVector(0, 0, 160));
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetupAttachment(GetMesh());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPCamera"));
	Camera->SetupAttachment(CameraBoom);

	Health = CreateDefaultSubobject<UMHealthComponent>(TEXT("Health"));

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	RightHandWeaponAttachPoint = TEXT("RightHandWeapon");

	LookUpRate = 50.f;
	TurnRate = 50.f;
}

void AMCharacter::OnHealthDepleted(UMHealthComponent* HealthComp, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) {
	if (Role == ROLE_Authority) {
		bDead = true;

		// #MoneRefactor to weapon function, including replicating movement
		CurrentWeapon->StopFire();
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->GetMesh()->SetSimulatePhysics(true);

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();
		SetLifeSpan(10.f); // #MoneHardcode
	}
}

void AMCharacter::StartFire() {
	if (CurrentWeapon) {
		CurrentWeapon->StartFire();
	}
}

void AMCharacter::StopFire() {
	if (CurrentWeapon) {
		CurrentWeapon->StopFire();
	}
}

void AMCharacter::MoveForward(float Value) {
	FRotator MoveRot = FRotator::ZeroRotator;
	MoveRot.Yaw = GetActorRotation().Yaw;

	AddMovementInput(MoveRot.RotateVector(FVector::ForwardVector) * Value);
}

void AMCharacter::MoveRight(float Value) {
	FRotator MoveRot = FRotator::ZeroRotator;
	MoveRot.Yaw = GetActorRotation().Yaw;

	AddMovementInput(MoveRot.RotateVector(FVector::RightVector) * Value);
}

void AMCharacter::LookUpAtRate(float Value) {
	AddControllerPitchInput(Value * LookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMCharacter::TurnAtRate(float Value) {
	AddControllerYawInput(Value * TurnRate * GetWorld()->GetDeltaSeconds());
}

void AMCharacter::StartAim() {
	bIsAiming = true;
}

void AMCharacter::StopAim() {
	bIsAiming = false;
}

void AMCharacter::AimToggle() {
	bIsAiming = !bIsAiming;
}

void AMCharacter::StartJump() {
	Jump();
}

void AMCharacter::StopJump() {
	StopJumping();
}

void AMCharacter::StartCrouch() {
	Crouch();
}

void AMCharacter::StopCrouch() {
	UnCrouch();
}

void AMCharacter::CrouchToggle() {
	if (CanCrouch()) {
		Crouch();
	} else {
		UnCrouch();
	}
}

void AMCharacter::BeginPlay() {
	Super::BeginPlay();

	if (Role == ROLE_Authority) {
		if (DefaultWeaponClass) {
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Instigator = this;
			SpawnParams.Owner = this;

			CurrentWeapon = GetWorld()->SpawnActor<AMWeapon>(DefaultWeaponClass, FTransform::Identity, SpawnParams);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, RightHandWeaponAttachPoint);

			Health->OnHealthDepleted.AddDynamic(this, &AMCharacter::OnHealthDepleted);
		}

	}
}

void AMCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AMCharacter::SetupPlayerInputComponent(UInputComponent* PIC) {
	Super::SetupPlayerInputComponent(PIC);

	PIC->BindAxis("MoveForward", this, &AMCharacter::MoveForward);
	PIC->BindAxis("MoveRight", this, &AMCharacter::MoveRight);

	PIC->BindAxis("LookUp", this, &AMCharacter::AddControllerPitchInput);
	PIC->BindAxis("Turn", this, &AMCharacter::AddControllerYawInput);

	PIC->BindAxis("LookUpAtRate", this, &AMCharacter::LookUpAtRate);
	PIC->BindAxis("TurnAtRate", this, &AMCharacter::TurnAtRate);

	PIC->BindAction("Fire", IE_Pressed, this, &AMCharacter::StartFire);
	PIC->BindAction("Fire", IE_Released, this, &AMCharacter::StopFire);

	PIC->BindAction("Aim", IE_Pressed, this, &AMCharacter::StartAim);
	PIC->BindAction("Aim", IE_Released, this, &AMCharacter::StopAim);
	PIC->BindAction("AimToggle", IE_Pressed, this, &AMCharacter::AimToggle);

	PIC->BindAction("Jump", IE_Pressed, this, &AMCharacter::StartJump);
	PIC->BindAction("Jump", IE_Released, this, &AMCharacter::StopJump);

	PIC->BindAction("CrouchToggle", IE_Pressed, this, &AMCharacter::CrouchToggle);
	PIC->BindAction("Crouch", IE_Pressed, this, &AMCharacter::StartCrouch);
	PIC->BindAction("Crouch", IE_Released, this, &AMCharacter::StopCrouch);
}

void AMCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMCharacter, CurrentWeapon);

	DOREPLIFETIME(AMCharacter, bDead);
}