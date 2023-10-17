// Fill out your copyright notice in the Description page of Project Settings.


#include "BDTGameState.h"
#include "Net/UnrealNetwork.h"


ABDTGameState::ABDTGameState()
{
	NetUpdateFrequency = 30.f;
	
	MaxRatNumber = 5000;
	BBEGMapOffset = 0.f;
	SpawnHeightRats = 150.f;
	BadumCursorPosition = FVector2f::ZeroVector;
	BadumIsSpawningRats = false;

	RatsPositions.Init(FShortVector2D{0, 0}, MaxRatNumber);
}

const TArray<FShortVector2D>& ABDTGameState::GetRatsPositions() const
{
	return RatsPositions;
}

const TArray<FVector2f>& ABDTGameState::GetSpawnerLocation() const
{
	return SpawnerPositions;
}
float ABDTGameState::GetMapOffSet() const
{
	return BBEGMapOffset;
}

float ABDTGameState::GetSpawnHeightRats() const
{
	return SpawnHeightRats;
}

void ABDTGameState::SetRatPosition(size_t Index, FVector2f const& NewPosition)
{
	RatsPositions[Index] = CompressRatPosition({NewPosition.X, NewPosition.Y + BBEGMapOffset});
}

void ABDTGameState::AddSpawnerPosition(FVector2f const& NewPosition)
{
	SpawnerPositions.Add(NewPosition);
}

void ABDTGameState::RemoveSpawnerPosition(size_t Index)
{
	if (SpawnerPositions.IsValidIndex(Index))
	{
		SpawnerPositions.RemoveAt(Index);
	}
}

void ABDTGameState::SetBadumCursorPosition(FVector2f const& NewPosition)
{
	BadumCursorPosition = {NewPosition.X, NewPosition.Y - BBEGMapOffset};
}

void ABDTGameState::SetBadumIsSpawningRats(bool NewValue)
{
	BadumIsSpawningRats = NewValue;
}

FVector2f ABDTGameState::GetBadumCursorPosition() const
{
	return BadumCursorPosition;
}

bool ABDTGameState::GetBadumIsSpawningRats() const
{
	return BadumIsSpawningRats;
}

void ABDTGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABDTGameState, BadumCursorPosition);
	DOREPLIFETIME(ABDTGameState, BadumIsSpawningRats);
	DOREPLIFETIME(ABDTGameState, RatsPositions);
	DOREPLIFETIME(ABDTGameState, SpawnerPositions);
}
