// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "MasterCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "../SoFRemastered.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Net/UnrealNetwork.h"



static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDbugWeaponDrawing(TEXT("DrawDebugWeapons"), 
	DebugWeaponDrawing, 
	TEXT("Draw Debug Lines for weapons"), 
		ECVF_Cheat);

// Sets default values
ABaseWeapon::ABaseWeapon()
{
	GunMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("1st Person Gun Mesh"));
	GunMesh1P->CastShadow = false;
	RootComponent = GunMesh1P;

	SetReplicates(true);
	bReplicates = true;
	
			
	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
	EjectSocketName = "AmmoEject";

	RateOfFire = 600;
	BulletEndPoint = 10000;

	ExtraAmmo = NumberOfMags * MagSize;

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
	
	Ammo = MagSize;
}

//Draws a line from Camera to crosshair and check to see if hit and apply damage
void ABaseWeapon::Fire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}

	if (Ammo > 0)
	{
		Ammo--;

		AActor* MyOwner = GetOwner();

		if (MyOwner)
		{
			

			FVector EyeLocation;
			FRotator EyeRotation;

			
			APawn* PawnOwner = Cast<APawn>(GetOwner());
			AController* ControllerOwner = PawnOwner->GetController();
			ControllerOwner->GetPlayerViewPoint(EyeLocation, EyeRotation);

			FVector ShotDirection = EyeRotation.Vector();

			FVector TraceEnd = EyeLocation + (ShotDirection * BulletEndPoint);

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(MyOwner);
			QueryParams.AddIgnoredActor(this);
			
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = true;

			FVector TracerEndPoint = TraceEnd;

			EPhysicalSurface SurfaceType = SurfaceType_Default;

			FHitResult Hit;
			if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
			{
				AActor* HitActor = Hit.GetActor();

				SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

				float ActualDamage = BaseDamage;
				if (SurfaceType == SURFACE_FLESHVULNERABLE)
				{
					ActualDamage *= HeadShotBonus;
				}

				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

				PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

				TracerEndPoint = Hit.ImpactPoint;

				

			}

			if (DebugWeaponDrawing > 0)
			{
				DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 10.0f, 0, 1.0f);
			}
		
			PlayFireEffects(TracerEndPoint);

			if (GetLocalRole() == ROLE_Authority)
			{
				HitScanTrace.TraceTo = TracerEndPoint;
				HitScanTrace.SurfaceType = SurfaceType;
			}

			LastFireTime = GetWorld()->TimeSeconds;
		}
	}
}


void ABaseWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ABaseWeapon::ServerFire_Validate()
{
	return true;
}


//Recoil Sets which point on the curve to call. Adjust curves to change recoil
void ABaseWeapon::Recoil()
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			//VerticalRecoil->GetFloatValue(RecoilRecoveryDelay);
			FRotator PitchRotation = PC->GetControlRotation();
			RecoilTimePerShot = RecoilTimePerShot + TimeBetweenShots;
			float NewPitch = VerticalRecoil->GetFloatValue(RecoilTimePerShot) * RecoilMultiplyer;
			float NewYaw = HorizontalRecoil->GetFloatValue(RecoilTimePerShot) * RecoilMultiplyer;
			PC->SetControlRotation(PitchRotation.Add(NewPitch, NewYaw, 0.0));
			
		}
		
	}
	
}

//Resets the recoil to starting point of the curve
void ABaseWeapon::ResetRecoil()
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			//VerticalRecoil->GetFloatValue(RecoilRecoveryDelay);
			FRotator PitchRotation = PC->GetControlRotation();
			RecoilTimePerShot = 0;
			float NewPitch = VerticalRecoil->GetFloatValue(RecoilTimePerShot);
			float NewYaw = HorizontalRecoil->GetFloatValue(RecoilTimePerShot);
			PC->SetControlRotation(PitchRotation.Add(NewPitch, NewYaw, 0.0));
		}
	}
}

void ABaseWeapon::OnRep_HitScanTrace()
{
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

//Reload the current weapon
void ABaseWeapon::Reload()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerReloadGun();
	}
	if (ExtraAmmo < MagSize)
	{
		
		AmountToReload = ExtraAmmo;
		ExtraAmmo = 0;
		Ammo = Ammo + AmountToReload;
	}
	else
	{
		AmountToReload = MagSize - Ammo;

		ExtraAmmo = ExtraAmmo - AmountToReload;

		Ammo = MagSize;
	}
	
}


//Gets Ammo for HUD
int ABaseWeapon::GetAmmo()
{
	return Ammo;
}

//Sets timer for weapon fire
void ABaseWeapon::StartFire()
{
	if (isAuto)
	{
		float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ABaseWeapon::Fire, TimeBetweenShots, true, FirstDelay);
	}
	else
	{
		float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ABaseWeapon::Fire, TimeBetweenShots, false, FirstDelay);
		
	}
	
	
}

//Stops timer for automatic weapon fire
void ABaseWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
	ResetRecoil();
}

void ABaseWeapon::ServerReloadGun_Implementation()
{
	Reload();
}

bool ABaseWeapon::ServerReloadGun_Validate()
{
	return true;
}

//Plays the weapon effects
void ABaseWeapon::PlayFireEffects(FVector TraceEnd)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, GunMesh1P, MuzzleSocketName);
		UGameplayStatics::SpawnEmitterAttached(EjectEffect, GunMesh1P, EjectSocketName);
		UGameplayStatics::SpawnSoundAttached(MuzzleSound, GunMesh1P, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = GunMesh1P->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}
	Recoil();
}

void ABaseWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = GunMesh1P->GetSocketLocation(MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ImpactSound, ImpactPoint, ShotDirection.Rotation());

	}
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseWeapon, HitScanTrace, COND_SkipOwner);
	DOREPLIFETIME(ABaseWeapon, ExtraAmmo);
	DOREPLIFETIME(ABaseWeapon, MagSize);
	DOREPLIFETIME(ABaseWeapon, NumberOfMags);
}