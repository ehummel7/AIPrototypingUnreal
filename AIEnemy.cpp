// Fill out your copyright notice in the Description page of Project Settings.


#include "AIEnemy.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "AIPrototyping/AIPrototypingGameMode.h"
#include "Net/UnrealNetwork.h"
#include "AIBlueprintHelperLibrary.generated.h"
#include "NavigationSystem.h"
#include "HealthComponent.h"

// Sets default values
AAIEnemy::AAIEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &AAIEnemy::HandleTakeDamage);

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AAIEnemy::OnPawnSeen);

	AIState = EAIState::Searching;
}

// Called when the game starts or when spawned
void AAIEnemy::BeginPlay()
{
	Super::BeginPlay();

	OrginalRotation = GetActorRotation();

	if (Patrol)
	{
		MoveToNextPatrolPoint();
	}
	
}

void AAIEnemy::OnPawnSeen(APawn* SeenPawn)
{

	if (SeenPawn == nullptr)
	{
		return;
	}

	if (AIState != EAIState::Attacking)
	{
		SetAIState(EAIState::Alerted);
	}

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Yellow, false, 10.0f);
	
	AController* Controller = GetController();
	if (Controller)
	{
		Controller->StopMovement();
	}

	FVector Direction = SeenPawn->GetActorLocation - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;
	SetActorRotation(NewLookAt);

	AIState = EAIState::Attacking;

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);

	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AAIEnemy::ResetOrietation, 10.0f);
}

void AAIEnemy::ResetOrietation()
{
	SetActorRotation(OrginalRotation);
}

void AAIEnemy::SetAIState(EAIState NewState)
{
	if (AIState == NewState)
	{
		return;
	}

	AIState = NewState;

	OnStateChanged(AIState);
}

void AAIEnemy::MoveToNextPatrolPoint()
{
	if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint)
	{
		CurrentPatrolPoint = FirstPatrolPoint;
	}
	else
	{
		CurrentPatrolPoint = SecondPatrolPoint;
	}

	UNavigationSystemV1::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
}

// Called every frame
void AAIEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentPatrolPoint)
	{
		FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();

		if (DistanceToGoal < 50)
		{
			MoveToNextPatrolPoint();
		}
	}
}

void AAIEnemy::HandleTakeDamage(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, 
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("Health %"), *FString::SanitizeFloat(Health))
}

