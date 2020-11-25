// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIEnemy.generated.h"

class UPawnSensingComponent;
class UHealthComponent;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Searching,

	Alerted,

	Attacking
};


UCLASS()
class AIPROTOTYPING_API AAIEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//AI VISION AND REACTION
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	FRotator OrginalRotation;

	void ResetOrietation();

	FTimerHandle TimerHandle_ResetOrientation;


	EAIState AIState;

	void SetAIState(EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChanged(EAIState NewState);

	UPROPERTY(EditInstanceOnly, Category = "AI")
	bool Patrol;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "Patrol"))
	AActor* FirstPatrolPoint;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "Patrol"))
	AActor* SecondPatrolPoint;

	AActor* CurrentPatrolPoint;

	void MoveToNextPatrolPoint();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	//AI HEALTH
	UHealthComponent* HealthComp;

	void HandleTakeDamage(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, 
		const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};

