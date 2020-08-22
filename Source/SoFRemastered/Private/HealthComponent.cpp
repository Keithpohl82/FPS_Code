// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "MasterGameMode.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	SetIsReplicatedByDefault(true);
	Health = DefaultHealth;
	Armor = DefaultArmor;
	bIsDead = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
			
		}

	}
	

}

float UHealthComponent::GetArmorAmount()
{
	return Armor / DefaultArmor;
}

float UHealthComponent::GetHealthAmount()
{
	return Health / DefaultHealth;
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	ActualDamage = ArmorDamageReduction * Damage;

	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	if (Armor > 0.0f)
	{
		Armor = Armor - ActualDamage;
		
	}
	else
	{
		Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
		

		bIsDead = Health <= 0.0f;

		if (bIsDead)
		{
			AMasterGameMode* GM = Cast<AMasterGameMode>(GetWorld()->GetAuthGameMode());
			if (GM)
			{
				GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
				
			}
		}

		OnHealthChanged.Broadcast(this, Health, Armor, Damage, DamageType, InstigatedBy, DamageCauser);
		
	}
	
}



void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
	DOREPLIFETIME(UHealthComponent, Armor);
	DOREPLIFETIME(UHealthComponent, DefaultHealth);
	DOREPLIFETIME(UHealthComponent, DefaultArmor);
}