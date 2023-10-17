// Copyright Epic Games, Inc. All Rights Reserved.


#include "BaDumTssGameModeBase.h"
#include "Engine/World.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroController.h"
#include "UObject/ConstructorHelpers.h"

ABaDumTssGameModeBase::ABaDumTssGameModeBase()
{
}

void ABaDumTssGameModeBase::HeroDied(AController* Controller)
{
	//FActorSpawnParameters SpawnParameters;
	//SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, Controller->GetPawn()->GetActorTransform(), SpawnParameters);

	Controller->UnPossess();
	//Controller->Possess(SpectatorPawn);

	OnHeroDied(Controller);
}

void ABaDumTssGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABaDumTssGameModeBase::RespawnHero(AController * Controller)
{
	//if (Controller->IsPlayerController())
	//{
		// Respawn player hero
	//	const AActor* PlayerStart = FindPlayerStart(Controller);

	//	FActorSpawnParameters SpawnParameters;
	//	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	//	ABDTHeroCharacter* Hero = GetWorld()->SpawnActor<ABDTHeroCharacter>(HeroClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParameters);

	//	APawn* OldSpectatorPawn = Controller->GetPawn();
	//	Controller->UnPossess();
	//	OldSpectatorPawn->Destroy();
	//	Controller->Possess(Hero);
	//}
}
