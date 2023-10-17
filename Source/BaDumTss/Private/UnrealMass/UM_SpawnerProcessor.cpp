// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealMass/UM_SpawnerProcessor.h"

#include "BDTGameState.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "BaDumTss/UnrealMass/UM_NavMeshMovementTrait.h"


UUM_SpawnerProcessor::UUM_SpawnerProcessor()
{
	bRequiresGameThreadExecution = true;
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	EntityQuery.RegisterWithProcessor(*this);
}

void UUM_SpawnerProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	GameStatePtr = dynamic_cast<ABDTGameState*>(GetWorld()->GetGameState());
}

void UUM_SpawnerProcessor::SetSpawnRate(const int NewSpawnRate)
{
	SpawnRate = NewSpawnRate;
}

void UUM_SpawnerProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FNavMeshPathFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FEntityUniqueID>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FMassMovementParameters>(EMassFragmentPresence::All);
	EntityQuery.AddTagRequirement<FNavAgent>(EMassFragmentPresence::All);
	EntityQuery.AddTagRequirement<FCanBeInstantiatedTag>(EMassFragmentPresence::All);
}

void UUM_SpawnerProcessor::Execute(FMassEntityManager& EntitySubsystem, FMassExecutionContext& Context)
{
	/*
	// Used to init Unique ID
	if(!bHasInitUniqueID)
	{
		EntityQuery.ForEachEntityChunk(EntitySubsystem, Context, ([&](FMassExecutionContext& NewContext)
		{
			const TArrayView<FEntityUniqueID> UniqueIDFragment = Context.GetMutableFragmentView<FEntityUniqueID>();
			
			// Loop for each entity in pools
			for (int EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				UniqueIDFragment[EntityIndex].SetUniqueID(EntityIndex);
			}
		}));
		
		bHasInitUniqueID = true;
	}
	*/
	
	if(!IsValid(GameStatePtr))
	{
		GameStatePtr = dynamic_cast<ABDTGameState*>(GetWorld()->GetGameState());
		return;
	}
	
	const float TmpsSpawnRate = 1.0f / SpawnRate;
	const float TmpsTokenSpawnRate = 1.0f / TokenSpawnRate;

	int NumberOfEnemiesToSpawn = Clock(TmpsSpawnRate);
	int NumberOfEnemiesToSpawnByToken = TokenClock(TmpsTokenSpawnRate);


	EntityQuery.ForEachEntityChunk(EntitySubsystem, Context, ([&](FMassExecutionContext& NewContext)
	{
		/*
		const TArrayView<FEntityUniqueID> UniqueIDFragment = Context.GetMutableFragmentView<FEntityUniqueID>();
		for (int EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
		{
			if(GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Unique ID : %d"), UniqueIDFragment[EntityIndex].GetUniqueID()));
			}
		}
		*/
		
		for (int EntityIndex = 0; EntityIndex < NumberOfEnemiesToSpawn; ++EntityIndex)
		{
			if(GameStatePtr->GetBadumIsSpawningRats() && (NewContext.GetNumEntities() > EntityIndex))
			{
				const FMassEntityHandle EntityHandle = NewContext.GetEntity(EntityIndex);

				TArrayView<FTransformFragment> TransformsList = NewContext.GetMutableFragmentView<FTransformFragment>();
				FTransform& Transform = TransformsList[EntityIndex].GetMutableTransform();
				Transform.SetLocation(FVector(GameStatePtr->GetBadumCursorPosition().X, GameStatePtr->GetBadumCursorPosition().Y,GameStatePtr->GetSpawnHeightRats()));

				NewContext.Defer().RemoveTag<FCanBeInstantiatedTag>(EntityHandle);
				GameStatePtr->SetRatPosition(EntityIndex,FVector2f(Transform.GetLocation().X, Transform.GetLocation().Y));
			}
		}
		
		for (int EntityIndex = 0; EntityIndex < NumberOfEnemiesToSpawnByToken; ++EntityIndex)
		{
			for(int i = 0; i < GameStatePtr->GetSpawnerLocation().Num(); ++i)
			{
				if(NewContext.GetNumEntities() > EntityIndex * GameStatePtr->GetSpawnerLocation().Num() + i + NumberOfEnemiesToSpawn)
				{
					const FMassEntityHandle EntityHandle = NewContext.GetEntity(EntityIndex * GameStatePtr->GetSpawnerLocation().Num() + i + NumberOfEnemiesToSpawn);
					TArrayView<FTransformFragment> TransformsList = NewContext.GetMutableFragmentView<FTransformFragment>();
					FTransform& Transform = TransformsList[EntityIndex].GetMutableTransform();
					Transform.SetLocation(FVector(GameStatePtr->GetSpawnerLocation()[i].X, GameStatePtr->GetSpawnerLocation()[i].Y, GameStatePtr->GetSpawnHeightRats()));
					
					NewContext.Defer().RemoveTag<FCanBeInstantiatedTag>(EntityHandle);
					GameStatePtr->SetRatPosition(EntityIndex,FVector2f(Transform.GetLocation().X, Transform.GetLocation().Y));
				}
			}
		}
		
		NumberOfEnemiesToSpawn = 0;
		NumberOfEnemiesToSpawnByToken = 0;
	}));
}

int UUM_SpawnerProcessor::Clock(const float tmpsSpawnRate)
{
	if(SpawnTime >= tmpsSpawnRate)
	{
		SpawnTime -= tmpsSpawnRate;
		
		return 1 + Clock(tmpsSpawnRate);
	}
	
	SpawnTime += FApp::GetDeltaTime();
	return 0;
}

int UUM_SpawnerProcessor::TokenClock(const float tmpsTokenSpawnRate)
{
	TokenSpawnTime += FApp::GetDeltaTime();
	
	if(TokenSpawnTime >= tmpsTokenSpawnRate)
	{
		TokenSpawnTime -= tmpsTokenSpawnRate;
		
		return 1 + Clock(tmpsTokenSpawnRate);
	}
	
	return 0;
}

int FEntityUniqueID::GetUniqueID()
{
	return UniqueID;
}

void FEntityUniqueID::SetUniqueID(int newUniqueID)
{
	UniqueID = newUniqueID;
}
