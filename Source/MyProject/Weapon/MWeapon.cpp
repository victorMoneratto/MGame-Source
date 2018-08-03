// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "Weapon/MWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "MyProject.h"

AMWeapon::AMWeapon() {
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	RootComponent = Mesh;

	MuzzleAttachPoint = "Muzzle";

	RoundsPerMinute = 300.f;
	LastFireTime = 0.f;

	bAutomaticFire = true;

	MinNetUpdateFrequency = 33.f;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void AMWeapon::StartFire() {
// 	UE_LOG(LogTemp, Warning, TEXT("%s StarFire"), Role == ROLE_Authority ? TEXT("Server") : TEXT("Client"));
	if (Role < ROLE_Authority) {
		WeaponRandomStream.GenerateNewSeed();
		ServerStartFire(WeaponRandomStream.GetCurrentSeed());
	}

	// #MoneTodo we shouldn't just loop on this timer to handle automatic fire,
	// for correctness we should delay Fire by <TimeRemaining = NextFireTime - CurrentTime> 
	// and let Fire deal with refiring
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float NextFireTime = LastFireTime + TimeBetweenRounds;
	if (bAutomaticFire) {
		float RemainingTime = FMath::Max(NextFireTime - CurrentTime, 0.f);
		GetWorldTimerManager().SetTimer(TimerHandle_Burst, this, &AMWeapon::Fire, TimeBetweenRounds, true, RemainingTime);
	} else {
		Fire();
	}
}

void AMWeapon::ServerStartFire_Implementation(int32 ClientSeed) {
	WeaponRandomStream.Initialize(ClientSeed);
	StartFire();
}

bool AMWeapon::ServerStartFire_Validate(int32 ClientSeed) {
	return true;
}

void AMWeapon::ServerStopFire_Implementation() {
	StopFire();
}

bool AMWeapon::ServerStopFire_Validate() {
	return true;
}

void AMWeapon::StopFire() {
	if (Role < ROLE_Authority) {
		ServerStopFire();
	}
	GetWorldTimerManager().ClearTimer(TimerHandle_Burst);
}

void AMWeapon::Fire() {
	// #MoneTemporary this feels temporary, we should have a better
	// and clear structure for how firing or really any action
	LastFireTime = GetWorld()->GetTimeSeconds();
}

FHitResult AMWeapon::WeaponTrace(const FVector& Start, const FVector& End, bool bIgnoreSelfAndOwner /*= true*/) {
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.TraceTag = TEXT("Weapon");
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.bTraceAsyncScene = true; // #MoneWhy async scene?
	if (bIgnoreSelfAndOwner) {
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(Instigator);
	}

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, COLLISION_WEAPON, QueryParams);
	return Hit;
}

FVector AMWeapon::GetAdjustedAim() const {
	FVector Result = FVector::ZeroVector;

	APlayerController* PC = Instigator ? Cast<APlayerController>(Instigator->Controller) : nullptr;
	if (PC) {
		FVector Unused;
		FRotator ViewRot;
		PC->GetPlayerViewPoint(Unused, ViewRot);
		Result = ViewRot.Vector();
	} else if (Instigator) {
		Result = Instigator->GetControlRotation().Vector();
	}

	return Result;
}

FVector AMWeapon::GetShotStartLocation(const FVector& AimDir) const {
	FVector Result = FVector::ZeroVector;
	FVector MuzzleLoc = GetMuzzleLocation();

	APlayerController* PC = Instigator ? Cast<APlayerController>(Instigator->Controller) : nullptr;
	if (PC) {
		FVector ViewLoc;
		FRotator Unused;
		PC->GetPlayerViewPoint(ViewLoc, Unused);

		// Sort of project MuzzleLoc onto an AimDir line starting from ViewLoc
		Result = ViewLoc + AimDir * (MuzzleLoc - ViewLoc | AimDir);
	} else {
		Result = MuzzleLoc;
	}

	return Result;
}

void AMWeapon::PlayFireFX() {
	UGameplayStatics::SpawnEmitterAttached(MuzzleFX, GetMesh(), MuzzleAttachPoint, FVector::ZeroVector,
										   FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale);
	APlayerController* PC = Instigator ? Cast<APlayerController>(Instigator->Controller) : nullptr;
	if (PC && PC->IsLocalController()) {
		PC->ClientPlayCameraShake(FireCameraShake);
	}
}

void AMWeapon::BeginPlay() {
	Super::BeginPlay();

	TimeBetweenRounds = 60.f / RoundsPerMinute;
}

FVector AMWeapon::GetMuzzleLocation() const {
	return GetMesh()->GetSocketLocation(MuzzleAttachPoint);
}

FRotator AMWeapon::GetMuzzleRotation() const {
	return GetMesh()->GetSocketRotation(MuzzleAttachPoint);
}

int32 DebugMWeapon = 0;
FAutoConsoleVariableRef CVar_DebugWeapon(
	TEXT("M.DebugWeapon"),
	DebugMWeapon,
	TEXT("Debug level for MWeapon.\n")
	TEXT("<=0 - Off\n")
	TEXT(">=1 - Logging\n")
	TEXT(">=2 - Drawing"),
	ECVF_Cheat);