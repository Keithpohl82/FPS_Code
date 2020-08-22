// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MasterCharacter.generated.h"



class UCameraComponent;
class ABaseWeapon;
class ASOFPlayerController;

UCLASS()
class SOFREMASTERED_API AMasterCharacter : public ACharacter
{
	GENERATED_BODY()


	

public:
	// Sets default values for this character's properties
	AMasterCharacter();
	UPROPERTY(EditAnywhere, Category = "Camera")
	UCameraComponent* Camera1P;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	USkeletalMeshComponent* Mesh1P;

	ASOFPlayerController* PC;
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	ABaseWeapon* CurrentFPPWeapon;

	UPROPERTY(Replicated, BlueprintReadWrite)
	ABaseWeapon* CurrentTPPWeapon;

	UPROPERTY(Replicated, BlueprintReadWrite)
	ABaseWeapon* FPPGunToSpawn;

	UPROPERTY(Replicated, BlueprintReadWrite)
	ABaseWeapon* TPPGunToSpawn;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<ABaseWeapon>>FirstPersonWeapons;
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<ABaseWeapon>>ThirdPersonWeapons;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Health Component")
	int32 TeamNum = -1;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class UHealthComponent* HealthComp;

	void MoveForward(float value);

	void MoveRight(float value);

	void BeginCrouch();

	void EndCrouch();

	

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* InHealthComp, float Health, float Armor, float HealthChanged, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCause);

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Health")
	bool bPlayerDied;
	
	void Reload();

	

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerNextWeapon();
	void NextWeapon();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPreviousWeapon();
	void PreviousWeapon();

	
	
	
	void StartFire();

	void StopFire();

	


public:	

	
	

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnFPPWeapon();
	void FPPWeaponToSpawn();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnTPPWeapon();
	void TPPWeaponToSpawn();




	UFUNCTION(BlueprintPure)
	int GetAmmoCount() const;

	UFUNCTION(BlueprintPure)
	int GetExtraAmmoCount() const;



	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Weapons")
	int WeaponIndex;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimSequence* NextWeaponAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimSequence* EquipPistol;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimSequence* ReloadAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimSequence* ReloadAnimation;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Animations")
	UAnimSequence* DeathAnim;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
