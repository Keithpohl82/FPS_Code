// Fill out your copyright notice in the Description page of Project Settings.


#include "SOFGameState.h"
#include "Net/UnrealNetwork.h"


ASOFGameState::ASOFGameState()
{


}





void ASOFGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASOFGameState, RedPoints);

	DOREPLIFETIME(ASOFGameState, BluePoints);
}