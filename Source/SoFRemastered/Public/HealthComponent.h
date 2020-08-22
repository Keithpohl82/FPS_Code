// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


//HealthChanged = HealthDelta
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FOnHealthChangedSignature, UHealthComponent*, HealthComp, float, Health, float, Armor, float, HealthChanged, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCause);

UCLASS( ClassGroup=(SHOOTER), meta=(BlueprintSpawnableComponent) )
class SOFREMASTERED_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "HealthComponent")
	float Health;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "HealthComponent")
	float Armor;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	float DefaultHealth = 100;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	float DefaultArmor = 100;

	float ActualDamage;

	UPROPERTY(EditDefaultsOnly)
	float ArmorDamageReduction = 0.75;

	UFUNCTION(BlueprintCallable)
	float GetArmorAmount();

	UFUNCTION(BlueprintCallable)
	float GetHealthAmount();


	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	bool bIsDead;


public:	

	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature OnHealthChanged;

	

};
