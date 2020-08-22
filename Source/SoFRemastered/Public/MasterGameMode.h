// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../SoFRemastered.h"
#include "GameFramework/GameModeBase.h"
#include "MasterGameMode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);


UCLASS()
class SOFREMASTERED_API AMasterGameMode : public AGameModeBase
{
	GENERATED_BODY()

	


public:

	virtual void Tick(float DeltaTime) override;

	virtual void StartPlay() override;



	FTimerHandle TimerHandle_RespawnTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	float RespawnTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Time")
	float GameSeconds;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Time")
	float GameMinutes;

	int32 RedPlayerCount;

	int32 BluePlayerCount;

	UPROPERTY(BlueprintAssignable)
	FOnActorKilled OnActorKilled;

	void PlayerRespawn();

	void RestartDeadPlayer();

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
};
