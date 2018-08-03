// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MTrackerBot.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Health/MHealthComponent.h"
#include "MyProject.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"
#include "Game/MGameModeBase.h"
#include "Game/MPlayerState.h"
#include "AI/MTrackerBotAIController.h"
#include "Player/Character/MCharacter.h"

int32 DebugTrackerBot = 0;
FAutoConsoleVariableRef CVar_DebugTrackerBot(
	TEXT("M.DebugTrackerBot"),
	DebugTrackerBot,
	TEXT("Debug level for MTrackerBot.\n")
	TEXT("<=0 - Off\n")
	TEXT(">=1 - Logging\n")
	TEXT(">=2 - Drawing"),
	ECVF_Cheat);


AMTrackerBot::AMTrackerBot() {
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetSimulatePhysics(true);
	RootComponent = Mesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetSphereRadius(200.f);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Sphere->SetupAttachment(RootComponent);

	Health = CreateDefaultSubobject<UMHealthComponent>(TEXT("Health"));

	AcceptanceRadiusSqr = 25.f;
	ForceIntensity = 1000.f;
	bAccelerationChange = true;
	SpeedLimit = 500.f;

	DeathExplosionDamage = 40.f;
	DeathExplosionRadius = 200.f;

	bTriggeredSelfDestruct = false;
	NumSelfDestructPulses = 4;
	TimePulsingSelfDestruct = 1.f;

	PowerLevelDamageBonus = 20.f;
	PowerLevel = 0;
	MaxPowerLevel = 3;

	AIControllerClass = AMTrackerBotAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

FVector AMTrackerBot::GetNextPathPoint() {
	FVector Result = GetActorLocation();

	APawn* TargetPawn = nullptr;
	float MinDistance = INFINITY;
	AMGameModeBase* GM = Cast<AMGameModeBase>(GetWorld()->GetAuthGameMode());
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; It++) {
		AMCharacter* Character = Cast<AMCharacter>(It->Get()->GetPawn());
		if (Character && Character->GetHealth()->IsAlive() && GM && GM->CanDealDamage(Cast<AMPlayerState>(Character->PlayerState),
																					  Cast<AMPlayerState>(PlayerState))) {
			float Distance = FVector::DistSquared(Character->GetActorLocation(), GetActorLocation());
			if (Distance < MinDistance) {
				MinDistance = Distance;
				TargetPawn = Character;
			}
		}
	}

	if (TargetPawn) {
		UNavigationPath* Path = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), TargetPawn);
		if (Path && Path->PathPoints.Num() > 1) {
			Result = Path->PathPoints[1];
		}
	}

	return Result;
}


void AMTrackerBot::TriggerSelfDestruct() {
	bTriggeredSelfDestruct = true;
	float Rate = TimePulsingSelfDestruct / NumSelfDestructPulses;
	GetWorldTimerManager().SetTimer(TimerHandle_SelfDestructPulse, this, &AMTrackerBot::SelfDestructPulse, Rate, true, 0);

	UGameplayStatics::SpawnSoundAttached(SelfDestructTriggerSound, RootComponent);
}

void AMTrackerBot::SelfDestructPulse() {
	UGameplayStatics::ApplyDamage(this, 25.f, GetInstigatorController(), this, nullptr);
}

void AMTrackerBot::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority) {
		float DistanceToTarget = FVector::DistSquared2D(GetActorLocation(), NextPathPoint);
		if (DistanceToTarget < AcceptanceRadiusSqr) {
			NextPathPoint = GetNextPathPoint();

			if (DebugTrackerBot >= MDEBUG_LEVEL_DRAW) {
				DrawDebugString(GetWorld(), GetActorLocation(), TEXT("Target reached"), nullptr, FColor::White, 5.f);
			}
		}

		FVector ForceDirection = (NextPathPoint - GetActorLocation()).GetSafeNormal(SMALL_NUMBER);
		FVector Force = ForceDirection * ForceIntensity;
		Mesh->AddForce(Force, NAME_None, bAccelerationChange);

		FVector Velocity = Mesh->GetPhysicsLinearVelocity();
		float Speed = Velocity.Size2D();
		if (Speed > SpeedLimit) {
			float Mult = SpeedLimit / Speed;
			Velocity *= Mult;
			Mesh->SetPhysicsLinearVelocity(Velocity);
		}

		if (DebugTrackerBot >= MDEBUG_LEVEL_DRAW) {
			DrawDebugSphere(GetWorld(), NextPathPoint, 50.f, 16, FColor::Yellow);
			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection * .25f * ForceIntensity,
									  50.f, FColor::Yellow, false, -1, 0, 2.f);
		}

	}
}

void AMTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor) {
	Super::NotifyActorBeginOverlap(OtherActor);

	AMGameModeBase* GM = Cast<AMGameModeBase>(GetWorld()->GetAuthGameMode());
	if (OtherActor->GetInstigatorController() && GM) {
		bool IsFriendly = GM->CanDealDamage(Cast<AMPlayerState>(OtherActor->GetInstigatorController()->PlayerState),
											Cast<AMPlayerState>(PlayerState));
		if (!bTriggeredSelfDestruct && IsFriendly) {
			TriggerSelfDestruct();
		}
	}
}


void AMTrackerBot::OnDamaged(UMHealthComponent* HealthComp, float InHealth, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) {
	if (MaterialInstance) {
		MaterialInstance->SetScalarParameterValue("LastTimeDamaged", GetWorld()->GetTimeSeconds());
	}

// 	UE_LOG(LogTemp, Warning, TEXT("Health of %s"), *FString::SanitizeFloat(InHealth));
}

void AMTrackerBot::OnHealthDepleted(UMHealthComponent* HealthComp, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) {

	UGameplayStatics::SpawnSoundAtLocation(this, ExplosionSound, GetActorLocation());
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, GetActorLocation());

	if (Role == ROLE_Authority) {
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);

		float ExplosionDamage = DeathExplosionDamage + PowerLevel * PowerLevelDamageBonus;
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), DeathExplosionRadius,
											nullptr, IgnoreActors, this, GetInstigatorController(), true);

		if (DebugTrackerBot >= MDEBUG_LEVEL_DRAW) {
			DrawDebugSphere(GetWorld(), GetActorLocation(), DeathExplosionRadius, 12, FColor::Red, false, 5.f, 0, 2.f);
		}

		Destroy();
	}
}

void AMTrackerBot::BeginPlay() {
	Super::BeginPlay();

	MaterialInstance = Mesh->CreateAndSetMaterialInstanceDynamic(0);

	Sphere->SetSphereRadius(DeathExplosionRadius);

	Health->OnDamaged.AddDynamic(this, &AMTrackerBot::OnDamaged);
	Health->OnHealthDepleted.AddDynamic(this, &AMTrackerBot::OnHealthDepleted);

	if (Role == ROLE_Authority) {
		NextPathPoint = GetNextPathPoint();
		GetWorldTimerManager().SetTimer(TimerHandle_FindGroupForPowerLevel, this,
										&AMTrackerBot::FindGroupForPowerLevel, 1.0, true);
	}
}



void AMTrackerBot::FindGroupForPowerLevel() {
	float PrevPowerLevel = PowerLevel;
	TArray<AActor*> OverlappingBots;
	Sphere->GetOverlappingActors(OverlappingBots, AMTrackerBot::StaticClass());
	PowerLevel = FMath::Min(OverlappingBots.Num(), MaxPowerLevel);
	if (PowerLevel != PrevPowerLevel) {
		OnRep_PowerLevel();
	}
}

void AMTrackerBot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMTrackerBot, bTriggeredSelfDestruct);
	DOREPLIFETIME(AMTrackerBot, PowerLevel);
}

void AMTrackerBot::OnRep_TriggerSelfDestruct() {
	if (bTriggeredSelfDestruct) {
		TriggerSelfDestruct();
	}
}

void AMTrackerBot::OnRep_PowerLevel() {
// 	UE_LOG(LogTemp, Warning, TEXT("%s %s"),
// 		   Role == ROLE_Authority ? TEXT("Server") : TEXT("Client"),
// 		   *FString::SanitizeFloat(PowerLevel));
	if (MaterialInstance) {
		float PowerLevelAlpha = PowerLevel / (float)MaxPowerLevel;
		MaterialInstance->SetScalarParameterValue("PowerLevelAlpha", PowerLevelAlpha);
	}
}
