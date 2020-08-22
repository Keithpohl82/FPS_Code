// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterGameMode.h"
#include "SOFPlayerController.h"
#include "SPlayerState.h"
#include "MasterCharacter.h"
#include "TimerManager.h"
#include "../SoFRemastered.h"
#include "Kismet/GameplayStatics.h"
#include "SOFGameState.h"
#include "Engine/Engine.h"




void AMasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AMasterGameMode::StartPlay()
{
	Super::StartPlay();
}

void AMasterGameMode::PlayerRespawn()
{
	
	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &AMasterGameMode::RestartDeadPlayer, 5.0f, false);
}

void AMasterGameMode::RestartDeadPlayer()
{
	for (FConstPlayerControllerIterator It = GetWorld()-> GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}
	}
}

void AMasterGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	ASPlayerState* State = NewPlayer->GetPlayerState<ASPlayerState>();
	APawn* PlayerPawn = NewPlayer->GetPawn();
	AMasterCharacter* PlayerChar = Cast<AMasterCharacter>(NewPlayer);
	ASOFPlayerController* Controller = Cast<ASOFPlayerController>(NewPlayer);

	if (ensure (State))
	{
		if (RedPlayerCount <= BluePlayerCount)
		{
			RedPlayerCount++;

			State->TeamNum = RED_TEAM;

			if (PlayerChar)
			{
				PlayerChar->TeamNum = RED_TEAM;
			}
			if (Controller)
			{
				Controller->TeamNum = RED_TEAM;
			}
		}
		else
		{
			BluePlayerCount++;

			State->TeamNum = BLUE_TEAM;

			if (PlayerChar)
			{
				PlayerChar->TeamNum = BLUE_TEAM;
			}
			if (Controller)
			{
				Controller->TeamNum = BLUE_TEAM;
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Red: %d, Blue: %d"), RedPlayerCount, BluePlayerCount);
	}


}

