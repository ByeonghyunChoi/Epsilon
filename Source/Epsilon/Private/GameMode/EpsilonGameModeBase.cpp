// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/EpsilonGameModeBase.h"
#include "Characters/EpsilonCharacter.h"
#include "Player/EpsilonPlayerController.h"

AEpsilonGameModeBase::AEpsilonGameModeBase()
{
	PlayerControllerClass = AEpsilonPlayerController::StaticClass();
	DefaultPawnClass = AEpsilonCharacter::StaticClass();
}
