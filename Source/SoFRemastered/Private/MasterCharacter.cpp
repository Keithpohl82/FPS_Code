// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "HealthComponent.h"
#include "BaseWeapon.h"
#include "MasterGameMode.h"
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "SOFPlayerController.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "../SoFRemastered.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "SPlayerState.h"


// Sets default values
AMasterCharacter::AMasterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera1P = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCam"));
	Camera1P->SetupAttachment(GetCapsuleComponent());
	Camera1P->bUsePawnControlRotation = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PawnMesh1P"));
	Mesh1P->SetupAttachment(Camera1P);
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);


	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));

	WeaponIndex = 0;
}

// Called when the game starts or when spawned
void AMasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	FPPWeaponToSpawn();
	TPPWeaponToSpawn();
	
	HealthComp->OnHealthChanged.AddDynamic(this, &AMasterCharacter::OnHealthChanged);

}



FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}



// Called every frame
void AMasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(GetLocalRole()), CurrentFPPWeapon, FColor::Blue, DeltaTime);

	ASPlayerState* PState = Cast<ASPlayerState>(GetPlayerState());
	if (PState)
	{
		TeamNum = PState->TeamNum;
		//UE_LOG(LogTemp, Warning, TEXT("SCharacter Team: %d"), TeamNum);
	}
	
	

}

//Move Forward and Backward
void AMasterCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector() * value);
}

//Move Left and Right
void AMasterCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector() * value);
}

//Starts Crouch. Hold to keep crouch
void AMasterCharacter::BeginCrouch()
{
	Crouch();
}

//Stops Crouch. Release to stand up
void AMasterCharacter::EndCrouch()
{
	UnCrouch();
}



//Calls Reload function on BaseWeapon.
void AMasterCharacter::Reload()
{
	CurrentFPPWeapon->Reload();
	Mesh1P->PlayAnimation(ReloadAnim, false);
	CurrentFPPWeapon->GunMesh1P->PlayAnimation(ReloadAnimation, false);
}


void AMasterCharacter::ServerNextWeapon_Implementation()
{
	NextWeapon();
}

bool AMasterCharacter::ServerNextWeapon_Validate()
{
	return true;
}

//Removes the current weapon and 
//spawns the next weapon in Array
void AMasterCharacter::NextWeapon()
{	
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerNextWeapon();
	}
	if (FirstPersonWeapons.Num() > 0)
		{
		WeaponIndex++;
			if (WeaponIndex == FirstPersonWeapons.Num())
				{
				// set visibility instead of destroy?
					WeaponIndex = 0;

					CurrentFPPWeapon->Destroy();
					CurrentTPPWeapon->Destroy();
					FPPWeaponToSpawn();
					TPPWeaponToSpawn();
				}		
			// set visibility instead of destroy?
			CurrentFPPWeapon->Destroy();
			CurrentTPPWeapon->Destroy();
			FPPWeaponToSpawn();
			TPPWeaponToSpawn();
		}

}



void AMasterCharacter::ServerPreviousWeapon_Implementation()
{
	PreviousWeapon();
}

bool AMasterCharacter::ServerPreviousWeapon_Validate()
{
	return true;
}

//Removes the current weapon and
//Spawns the previous weapon in the array
void AMasterCharacter::PreviousWeapon()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerPreviousWeapon();
	}
	if (FirstPersonWeapons.Num() > 1)
	{
		if (WeaponIndex > 0)
		{
			// set visibility instead of destroy?
			WeaponIndex--;

			CurrentFPPWeapon->Destroy();
			CurrentTPPWeapon->Destroy();
			FPPWeaponToSpawn();
			TPPWeaponToSpawn();
		}
		else
		{
			// set visibility instead of destroy?
			WeaponIndex = FirstPersonWeapons.Num() - 1;

			CurrentFPPWeapon->Destroy();
			CurrentTPPWeapon->Destroy();
			FPPWeaponToSpawn();
			TPPWeaponToSpawn();
		}
	}
	
}

void AMasterCharacter::ServerSpawnFPPWeapon_Implementation()
{
	FPPWeaponToSpawn();
}

bool AMasterCharacter::ServerSpawnFPPWeapon_Validate()
{
	return true;
}

//Spawns the weapon for the first person character
void AMasterCharacter::FPPWeaponToSpawn()
{
	if (GetLocalRole() > ROLE_Authority)
	{
		ServerSpawnFPPWeapon();
	}

		FName WeaponSocket = "FPPGunSocket";

		FPPGunToSpawn = GetWorld()->SpawnActor<ABaseWeapon>(FirstPersonWeapons[WeaponIndex]);
		FPPGunToSpawn->SetOwner(this);
		FPPGunToSpawn->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
		CurrentFPPWeapon = FPPGunToSpawn;
		GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Red, FString::Printf(TEXT("FPPWeaponToSpawn")));
}



void AMasterCharacter::ServerSpawnTPPWeapon_Implementation()
{
	TPPWeaponToSpawn();
}

bool AMasterCharacter::ServerSpawnTPPWeapon_Validate()
{
	return true;
}

//Spawns the weapon for the third person character
void AMasterCharacter::TPPWeaponToSpawn()
{
	if (GetLocalRole() > ROLE_Authority)
	{
		ServerSpawnTPPWeapon();
	}
	
	FName WeaponSocket = "TPPGunSocket";
		
		GetMesh()->GetSocketTransform("TPPGunSocket");
		TPPGunToSpawn = GetWorld()->SpawnActor<ABaseWeapon>(ThirdPersonWeapons[WeaponIndex]);
		TPPGunToSpawn->SetOwner(this);
		TPPGunToSpawn->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "TPPGunSocket");
		CurrentTPPWeapon = TPPGunToSpawn;
	
}

//Calls the StartFire function on BaseWeapon
void AMasterCharacter::StartFire()
{
	if (CurrentFPPWeapon)
	{
		CurrentFPPWeapon->StartFire();
	}
}

//Calls the the StopFire function on BaseWeapon
void AMasterCharacter::StopFire()
{
	if (CurrentFPPWeapon)
	{
		CurrentFPPWeapon->StopFire();
	}
}

void AMasterCharacter::OnHealthChanged(UHealthComponent* InHealthComp, float Health, float Armor, float HealthChanged, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCause)
{
	if (Health <= 0.0f && !bPlayerDied)
	{
		bPlayerDied = true;
		
		GetMesh()->PlayAnimation(DeathAnim, false);
		
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetLifeSpan(3.0f);
		

		AMasterGameMode* GM = Cast<AMasterGameMode>(GetWorld()->GetAuthGameMode());
		GM->PlayerRespawn();

		
		

		DetachFromControllerPendingDestroy();
	}
}


//Gets the amount of Ammo in the clip of the weapon
int AMasterCharacter::GetAmmoCount() const
{
	return CurrentFPPWeapon->GetAmmo();
}

//Gets the Extra ammo amount not loaded in the weapon
int AMasterCharacter::GetExtraAmmoCount() const
{
	return CurrentFPPWeapon->ExtraAmmo;
}

// Called to bind functionality to input
void AMasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMasterCharacter::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction(TEXT("NextWeapon"), IE_Pressed, this, &AMasterCharacter::NextWeapon);
	PlayerInputComponent->BindAction(TEXT("PreviousWeapon"), IE_Pressed, this, &AMasterCharacter::PreviousWeapon);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AMasterCharacter::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &AMasterCharacter::StopFire);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &AMasterCharacter::BeginCrouch);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &AMasterCharacter::EndCrouch);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AMasterCharacter::Reload);

}

void AMasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMasterCharacter, CurrentFPPWeapon);
	DOREPLIFETIME(AMasterCharacter, CurrentTPPWeapon);
	DOREPLIFETIME(AMasterCharacter, FirstPersonWeapons);
	DOREPLIFETIME(AMasterCharacter, ThirdPersonWeapons);
	DOREPLIFETIME(AMasterCharacter, WeaponIndex);
	DOREPLIFETIME(AMasterCharacter, FPPGunToSpawn);
	DOREPLIFETIME(AMasterCharacter, TPPGunToSpawn);
	DOREPLIFETIME(AMasterCharacter, bPlayerDied);
	DOREPLIFETIME(AMasterCharacter, DeathAnim);
	DOREPLIFETIME(AMasterCharacter, TeamNum);
	


}