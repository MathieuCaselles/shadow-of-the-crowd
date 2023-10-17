// Fill out your copyright notice in the Description page of Project Settings.


#include "UM_NavMeshMovementTrait.h"

#include "UM_AdvancedRandomMovementTrait.h"
#include "AITypes.h"
#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "NavigationSystem.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "Actors/WeaponBase/BDTProjectile.h"
#include "Kismet/GameplayStatics.h"


struct FMassMoveTargetFragment;
struct FMassMovementParameters;

void UUM_NavMeshMovementTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddTag<FNavAgent>();
	BuildContext.AddFragment<FNavMeshPathFragment>();
	// FNavTimeSharedFragment
	//FSharedStruct<FNavTimeSharedFragment> MyNavTimeSharedFragment;
	//BuildContext.AddSharedFragment(MyNavTimeSharedFragment);

	/*
	FMySharedSettings MyFragment;
	MyFragment.MyValue = UserSetValue;

	//Get a hash of a FConstStructView of said fragment and store it
	uint32 MySharedFragmentHash = UE::StructUtils::GetStructCrc32(FConstStructView::Make(MyFragment));
	
	//Search the Mass Entity subsystem for an identical struct with the hash. If there are none, make a new one with the set fragment.
	FSharedStruct MySharedFragment = 
		EntityManager.GetOrCreateSharedFragment<FMySharedSettings>(MySharedFragmentHash, MyFragment);

	//Finally, add the shared fragment to the BuildContext!
	BuildContext.AddSharedFragment(MySharedFragment);
	*/
}

UUM_NavMeshMovementProcessor::UUM_NavMeshMovementProcessor()
{
	bRequiresGameThreadExecution = true;
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Avoidance);
	EntityQuery.RegisterWithProcessor(*this);
}

void UUM_NavMeshMovementProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FNavMeshPathFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FMassMovementParameters>(EMassFragmentPresence::All);
	//EntityQuery.AddSharedRequirement<FNavTimeSharedFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddTagRequirement<FNavAgent>(EMassFragmentPresence::All);
}

void UUM_NavMeshMovementProcessor::Execute(FMassEntityManager& EntitySubsystem, FMassExecutionContext& Context)
{
	if(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == nullptr)
	{
		return;
	}
	
	ABDTHeroCharacter* HeroPtr = static_cast<ABDTHeroCharacter*>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	const FVector CharacterPos = HeroPtr->GetActorLocation();
	const TArray<ABDTProjectile*> ArrayOfProjectile = HeroPtr->GetArrayProjectile();

	//float& TimeBeforeRecalculatePath = Context.GetMutableSharedFragment<FNavTimeSharedFragment>().Time;

	//EntityQuery.ForEachEntityChunk(EntitySubsystem, Context, ([this, CharacterPos, TimeBeforeRecalculatePath](FMassExecutionContext& Context)
	EntityQuery.ForEachEntityChunk(EntitySubsystem, Context, ([this, HeroPtr, CharacterPos, ArrayOfProjectile](FMassExecutionContext& Context)
	{
		TArrayView<FTransformFragment> TransformsList = Context.GetMutableFragmentView<FTransformFragment>();
		const TArrayView<FMassMoveTargetFragment> NavTargetsList = Context.GetMutableFragmentView<FMassMoveTargetFragment>();
		const TArrayView<FNavMeshPathFragment> PathList = Context.GetMutableFragmentView<FNavMeshPathFragment>();
		const FMassMovementParameters& MovementParameters = Context.GetConstSharedFragment<FMassMovementParameters>();
		
		if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
		{
			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				// Get fragment data
				FTransform& Transform = TransformsList[EntityIndex].GetMutableTransform();
				FMassMoveTargetFragment& MoveTarget = NavTargetsList[EntityIndex];
				FNavMeshPathFragment& Path = PathList[EntityIndex];
				int8& PathIndex = Path.PathIndex;

				if(Transform.GetLocation().Z <= -500.0f)
				{
					Context.Defer().DestroyEntity(Context.GetEntity(EntityIndex));
				}
				
				// Get info from fragments
				FVector CurrentLocation = Transform.GetLocation();
				FVector& TargetVector = Path.TargetLocation;
				
				// Calculate path
				if (Path.PathResult.Result != ENavigationQueryResult::Success)
				{
					FNavLocation Result;
					NavSys->GetRandomReachablePointInRadius(Transform.GetLocation(), 3000.f, Result);
					//Path.TargetLocation = Result.Location;
					Path.TargetLocation = CharacterPos;
					
					FAIMoveRequest MoveRequest(TargetVector);
					const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance();
					FSharedConstNavQueryFilter NavFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, this, MoveRequest.GetNavigationFilter());
					FPathFindingQuery Query(&*this, *NavData, CurrentLocation, TargetVector, NavFilter);
					Path.PathResult = NavSys->FindPathSync(Query);
					
					if (Path.PathResult.Result == ENavigationQueryResult::Success)
					{
						PathIndex = 0;
						MoveTarget.SlackRadius = 100.f;
						MoveTarget.Center = Path.PathResult.Path->GetPathPointLocation(1).Position;
					}
					// Use result to move ai

					//DrawDebugLine(GetWorld(), Transform.GetLocation(),MoveTarget.Center, FColor::Red, true, -1, 0, 5);
					MoveTarget.DesiredSpeed = FMassInt16Real(MovementParameters.DefaultDesiredSpeed);
				}
				
				MoveTarget.DistanceToGoal = (MoveTarget.Center-Transform.GetLocation()).Size();
				MoveTarget.Forward = (MoveTarget.Center-Transform.GetLocation()).GetSafeNormal();
				FHitResult OutHit;
				GetWorld()->LineTraceSingleByChannel(OutHit, Transform.GetLocation()+(Transform.GetRotation().GetUpVector()*100), Transform.GetLocation()-(Transform.GetRotation().GetUpVector()*100), ECollisionChannel::ECC_Visibility);
				MoveTarget.Center.Z = OutHit.ImpactPoint.Z;
				
				// Go from point to point to reach final destination
				/*
				if (MoveTarget.DistanceToGoal <= MoveTarget.SlackRadius)
				{
					if (PathIndex < Path.PathResult.Path->GetPathPoints().Num()-1)
					{
						PathIndex++;
						MoveTarget.Center = Path.PathResult.Path->GetPathPoints()[PathIndex];
					}
					else
					{
						PathIndex = 0;
						Path.PathResult.Result = ENavigationQueryResult::Invalid;
						FNavLocation Result;
						NavSys->GetRandomReachablePointInRadius(Transform.GetLocation(), 3000.f, Result);
						//Path.TargetLocation = Result.Location;
						Path.TargetLocation = CharacterPos;
					}
				}
				*/

				/*
				Path.PathResult.Result = ENavigationQueryResult::Invalid;
				Path.TargetLocation = CharacterPos;
				*/

				Path.TimeBeforeRecalculatePath -= FApp::GetDeltaTime();
				
				if ((MoveTarget.DistanceToGoal <= MoveTarget.SlackRadius) || (Path.TimeBeforeRecalculatePath <= 0))
				{
					if (PathIndex < Path.PathResult.Path->GetPathPoints().Num()-1)
					{
						PathIndex++;
						MoveTarget.Center = Path.PathResult.Path->GetPathPoints()[PathIndex];
					}
					else
					{
						PathIndex = 0;
						Path.PathResult.Result = ENavigationQueryResult::Invalid;
						FNavLocation Result;
						NavSys->GetRandomReachablePointInRadius(Transform.GetLocation(), 3000.f, Result);
						Path.TargetLocation = CharacterPos;
					}
				}
				// Collision Management
				// Rats
				if((CharacterPos - Transform.GetLocation()).Size() <= 200.0f)
				{
					if(HeroPtr != nullptr)
					{
						//if(GEngine)	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("IS CALLED")));
					
						HeroPtr->TakeDamageFromRat();
					}
				}
				
				// Fire Balls
				for (int i = 0; i < ArrayOfProjectile.Num(); ++i)
				{
					if((ArrayOfProjectile[i]->GetActorLocation() - CurrentLocation).Size() <= 150.0f)
					{
						//if(GEngine)	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("IS MOVING")));
						Transform.SetLocation(CurrentLocation + FVector(0,0,-1000));
						
						// Normalement le Kill est là mais le Defer se joue avant donc...
					}
				}

				if(Path.TimeBeforeRecalculatePath <= 0)
				{
					Path.TimeBeforeRecalculatePath = 0.5f;
				}
			}
		}
	}));
}