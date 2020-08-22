// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemPickupActor.h"
#include "TimerManager.h"
#include "Particles/ParticleSystem.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AItemPickupActor::AItemPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UParticleSystem>(TEXT("Item Mesh"));

	PowerupInterval = 0.0f;
	TotalNumTicks = 0.0f;

	bIsItemPickupActive = false;

	SetReplicates(true);

}



void AItemPickupActor::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TicksProcessed >= TotalNumTicks)
	{
		OnExpired();
		bIsItemPickupActive = false;
		OnRep_ItemPickupActive();
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);

	}
}

void AItemPickupActor::OnRep_ItemPickupActive()
{
	OnItemPickupStateChanged(bIsItemPickupActive);
}

// Called every frame
void AItemPickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemPickupActor::ActivatPowerup(AActor* ActiveFor)
{
	OnActivated(ActiveFor);
	bIsItemPickupActive = true;
	OnRep_ItemPickupActive();

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &AItemPickupActor::OnTickPowerup, PowerupInterval, true, 0.0f);
	}
	else
	{
		OnTickPowerup();
	}
}

void AItemPickupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemPickupActor, bIsItemPickupActive);
}