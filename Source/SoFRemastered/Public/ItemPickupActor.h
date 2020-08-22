// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemPickupActor.generated.h"

UCLASS()
class SOFREMASTERED_API AItemPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPickupActor();

protected:
	


	UPROPERTY(EditDefaultsOnly, Category = "Items")
	class UParticleSystem* ItemMesh;

	//Time Between Ticks
	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	float PowerupInterval;

	//Total times we apply the powerup effect
	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	int TotalNumTicks;



	FTimerHandle TimerHandle_PowerupTick;

	int32 TicksProcessed;

	UFUNCTION()
	void OnTickPowerup();

	UPROPERTY(ReplicatedUsing=OnRep_ItemPickupActive)
	bool bIsItemPickupActive;

	UFUNCTION()
	void OnRep_ItemPickupActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickups")
	void OnItemPickupStateChanged(bool bNewIsActive);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ActivatPowerup(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickups")
	void OnActivated(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickups")
	void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickups")
	void OnExpired();

};
