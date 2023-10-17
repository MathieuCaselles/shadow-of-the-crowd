// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/Characters/BBEGBeacon/SpawnerBeacon/BDTSpawnerBeacon.h"

#include "BDTGameState.h"

void ABDTSpawnerBeacon::BeginPlay()
{
	Super::BeginPlay();
	GameStatePtr = Cast<ABDTGameState>(GetWorld()->GetGameState());
	if (GetLocalRole() != ROLE_Authority || GameStatePtr == nullptr)
		return;
	
	IndexGameStateSpawner =  GameStatePtr->GetSpawnerLocation().Num();
	GameStatePtr->AddSpawnerPosition(FVector2f(GetActorLocation().X, GetActorLocation().Y));
}

void ABDTSpawnerBeacon::BeginDestroy()
{
	Super::BeginDestroy();

	if (GetLocalRole() != ROLE_Authority || GameStatePtr == nullptr)
		return;

	GameStatePtr->RemoveSpawnerPosition(IndexGameStateSpawner);
}