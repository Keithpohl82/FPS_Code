// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

class AItemPickupActor;

UCLASS()
class SOFREMASTERED_API APickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
	class USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere, Category = "Pickups")
	TSubclassOf<AItemPickupActor> PickupClass;

	class AItemPickupActor* ItemPickup;

	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	float CooldownDuration;

	FTimerHandle TimerHandle_RespawnTimer;

	void Respawn();


public:	
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;


};
