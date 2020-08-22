// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"


ASPlayerState::ASPlayerState()
{
	SetReplicates(true);
}

void ASPlayerState::AddScore(float ScoreDelta)
{
	PlayerScore += ScoreDelta;
}

int ASPlayerState::GetPlayerScore()
{
	return PlayerScore;
}



void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, TeamNum);
}