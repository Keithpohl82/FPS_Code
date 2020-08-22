// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupActor.h"
#include "Components/SphereComponent.h"
#include "ItemPickupActor.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APickupActor::APickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);

	RootComponent = SphereComp;

	SetReplicates(true);

}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		Respawn();
	}
}

void APickupActor::Respawn()
{
	if (PickupClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PickupClass is nullptr in %s. Update the blueprint"), *GetName());

		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ItemPickup = GetWorld()->SpawnActor<AItemPickupActor>(PickupClass, GetTransform(), SpawnParams);
}

void APickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{

	Super::NotifyActorBeginOverlap(OtherActor);

	if (GetLocalRole() == ROLE_Authority && ItemPickup)
	{
		ItemPickup->ActivatPowerup(OtherActor);

		ItemPickup = nullptr;

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &APickupActor::Respawn, CooldownDuration);
	}
}

