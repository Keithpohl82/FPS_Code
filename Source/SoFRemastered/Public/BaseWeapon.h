// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Curves/CurveFloat.h"

#include "BaseWeapon.generated.h"

class USkeletalMeshComponent;
class UCurveFloat;
class UDamageType;
class UParticleSystem;
class AMasterCharacter;

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;
	UPROPERTY()
	FVector_NetQuantize TraceTo;
};


UCLASS()
class SOFREMASTERED_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMeshComponent* GunMesh1P;

protected:
	
	virtual void BeginPlay() override;

	void PlayFireEffects(FVector TraceEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	void Fire();

	void Recoil();

	void ResetRecoil();

	int AmountToReload;

	float RecoilTimePerShot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon Effects")
	FName EjectSocketName;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon Effects")
	FName MuzzleSocketName;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon Effects")
	FName TracerTargetName;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
	UParticleSystem* MuzzleEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
	UParticleSystem* EjectEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
	UParticleSystem* DefaultImpactEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
	UParticleSystem* FleshImpactEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
	UParticleSystem* TracerEffect;
	UPROPERTY(EditAnywhere, Category = "Weapon Sounds")
	USoundBase* MuzzleSound;
	UPROPERTY(EditAnywhere, Category = "Weapon Sounds")
	USoundBase* ImpactSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float HeadShotBonus;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	UCurveFloat* VerticalRecoil;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	UCurveFloat* HorizontalRecoil;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilRecoveryTime;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilRecoveryDelay;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilMultiplyer;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	bool isAuto;
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	float BulletEndPoint;
	UPROPERTY(BlueprintReadWrite, Category = "Ammo")
	int Ammo;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
	int MagSize = 30;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
	int NumberOfMags = 3;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	int CurrentAmmo;

	AMasterCharacter* PlayerCharacter;

	FTimerHandle TimerHandle_TimeBetweenShots;
	
	float LastFireTime;

	// Bullets fired per minuet
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	float RateOfFire;
	
	float TimeBetweenShots;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;
	
	

	

public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponType")
	bool isShotGun;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponType")
	bool isPistol;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponType")
	bool isSniper;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponType")
	bool isKnife;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Ammo")
	int ExtraAmmo;

	void StartFire();

	void StopFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReloadGun();
	void Reload();

	UFUNCTION(BlueprintPure)
	int GetAmmo();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	UFUNCTION()
	void OnRep_HitScanTrace();
};
