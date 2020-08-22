// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TestCharacter.generated.h"

class ABaseWeapon;
class UCameraComponent;

UCLASS()
class SOFREMASTERED_API ATestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATestCharacter();
	
	UPROPERTY(EditAnywhere, Category = "Camera")
	UCameraComponent* Camera1P;

	UPROPERTY(EditAnywhere, Category = "Player")
	class USkeletalMeshComponent* Mesh1P;

	ABaseWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ABaseWeapon>StartingWeaponClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveRight(float value);
	void BeginCrouch();
	void EndCrouch();
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName WeaponAttachSocketName;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
