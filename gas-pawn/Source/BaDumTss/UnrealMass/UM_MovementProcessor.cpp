// Fill out your copyright notice in the Description page of Project Settings.


#include "UM_MovementProcessor.h"

#include "AbilitySystemComponent.h"
#include "MassAIBehaviorTypes.h"
#include "MassCommonFragments.h"
#include "MassGameplayDebugTypes.h"
#include "UM_MovementTrait.h"
#include "UM_VisualisationTrait.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "Actors/WeaponBase/BDTProjectile.h"
#include "BaDumTss/BaDumTssGameModeBase.h"
#include "Kismet/GameplayStatics.h"

UUM_MovementProcessor::UUM_MovementProcessor()
{
	bRequiresGameThreadExecution = true;
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
	EntityQuery.RegisterWithProcessor(*this);
}

void UUM_MovementProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FSimpleMovementFragment>(EMassFragmentAccess::ReadWrite);
}

void UUM_MovementProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	if(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == nullptr)
	{
		return;
	}
	
	ABDTHeroCharacter* HeroPtr = static_cast<ABDTHeroCharacter*>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	const FVector CharacterPos = HeroPtr->GetActorLocation();
	const TArray<ABDTProjectile*> ArrayOfProjectile = HeroPtr->GetArrayProjectile();

	FCriticalSection* CSTry;
	
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([this, HeroPtr, CharacterPos, ArrayOfProjectile, CSTry](FMassExecutionContext& Context)
	{
		const TArrayView<FTransformFragment> TransformList = Context.GetMutableFragmentView<FTransformFragment>();
		const TArrayView<FSimpleMovementFragment> SimpleMovementList = Context.GetMutableFragmentView<FSimpleMovementFragment>();
		const float WorldDeltaTime = Context.GetDeltaTimeSeconds();
		
		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
		{
			FTransform& Transform =	TransformList[EntityIndex].GetMutableTransform();
			const FVector& TargetPos = CharacterPos;

			FVector CurrentLocation	 = Transform.GetLocation();
			FVector TargetVector = TargetPos - CurrentLocation;

			if(TargetVector.Size() <= 100.0f)
			{
				if(HeroPtr != nullptr)
				{
					if(GEngine)	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("IS CALLED")));
					
					HeroPtr->TakeDamageFromRat();
				}
			}
			else
			{
				Transform.SetLocation(CurrentLocation + TargetVector.GetSafeNormal() * 300.0f * WorldDeltaTime);
			}

			// Collision Management
			// Fire Balls
			for (int i = 0; i < ArrayOfProjectile.Num(); ++i)
			{
				if((ArrayOfProjectile[i]->GetActorLocation() - CurrentLocation).Size() <= 200.0f)
				{
					//Context.Defer().RemoveFragment<FAgentDebugVisualization>(Context.GetEntity(EntityIndex));
					Context.Defer().DestroyEntity(Context.GetEntity(EntityIndex));
				}
			}
		}
	}));
}