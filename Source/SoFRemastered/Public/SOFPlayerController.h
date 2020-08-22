// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SOFPlayerController.generated.h"

/**
 * 
 */
UCLASS(Config=Game)
class SOFREMASTERED_API ASOFPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:

	UPROPERTY(Config)
	FName PlayerName = "TestName";

	UPROPERTY(Replicated)
	int32 TeamNum;

};
