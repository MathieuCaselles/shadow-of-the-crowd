// Fill out your copyright notice in the Description page of Project Settings.


#include "UM_NavMeshMovementTrait.h"

#include "UM_AdvancedRandomMovementTrait.h"
#include "AITypes.h"
#include "BDTGameState.h"
#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "NavigationSystem.h"
#include "UnrealMass/UM_SpawnerProcessor.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "MassActorSubsystem.h"


struct FMassMoveTargetFragment;
struct FMassMovementParameters;

void UUM_NavMeshMovementTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddTag<FNavAgent>();
	BuildContext.AddTag<FCanBeInstantiatedTag>();
	BuildContext.AddFragment<FNavMeshPathFragment>();
	BuildContext.AddFragment<FEntityUniqueID>();
}

UUM_NavMeshMovementProcessor::UUM_NavMeshMovementProcessor()
{
	bRequiresGameThreadExecution = true;
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Avoidance);
	EntityQuery.RegisterWithProcessor(*this);
}

void UUM_NavMeshMovementProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);
	if(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == nullptr)
		return;

	HeroPtr = static_cast<ABDTHeroCharacter*>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	GameStatePtr = dynamic_cast<ABDTGameState*>(GetWorld()->GetGameState());
}

void UUM_NavMeshMovementProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FNavMeshPathFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FEntityUniqueID>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FMassMovementParameters>(EMassFragmentPresence::All);
	EntityQuery.AddTagRequirement<FNavAgent>(EMassFragmentPresence::All);
	EntityQuery.AddTagRequirement<FCanBeInstantiatedTag>(EMassFragmentPresence::None);
}

void UUM_NavMeshMovementProcessor::Execute(FMassEntityManager& EntitySubsystem, FMassExecutionContext& Context)
{
	if(!IsValid(HeroPtr))
	{
		if(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == nullptr)
			return;
			
		HeroPtr = static_cast<ABDTHeroCharacter*>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		return;
	}
	
	if(!IsValid(GameStatePtr))
	{
		GameStatePtr = dynamic_cast<ABDTGameState*>(GetWorld()->GetGameState());
		return;
	}
	
	const FVector CharacterPos = HeroPtr->GetActorLocation();
	TArray<ABDTProjectile*> ArrayOfProjectile = HeroPtr->GetArrayProjectile();

	const ENetMode NetMode = GEngine->GetNetMode(GetWorld());
	
	EntityQuery.ForEachEntityChunk(EntitySubsystem, Context, ([this, NetMode, CharacterPos, ArrayOfProjectile](FMassExecutionContext& Context)
	{
		TArrayView<FTransformFragment> TransformsList = Context.GetMutableFragmentView<FTransformFragment>();
		const TArrayView<FMassMoveTargetFragment> NavTargetsList = Context.GetMutableFragmentView<FMassMoveTargetFragment>();
		const TArrayView<FNavMeshPathFragment> PathList = Context.GetMutableFragmentView<FNavMeshPathFragment>();
		//const TArrayView<FEntityUniqueID> UniqueIDFragment = Context.GetMutableFragmentView<FEntityUniqueID>();
		const FMassMovementParameters& MovementParameters = Context.GetConstSharedFragment<FMassMovementParameters>();

		if(NetMode == NM_ListenServer)
		{
			if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
			{
				for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
				{
					// Get fragment data
					FTransform& Transform = TransformsList[EntityIndex].GetMutableTransform();
					
					// Kill of entities
					if(Transform.GetLocation().X > 10000)
					{
						Context.Defer().AddTag<FCanBeInstantiatedTag>(Context.GetEntity(EntityIndex));
						IndexToRemove.Add(EntityIndex);
					}

					// Get ID
					//FEntityUniqueID& EntityID = UniqueIDFragment[EntityIndex];
				
					FMassMoveTargetFragment& MoveTarget = NavTargetsList[EntityIndex];
					FNavMeshPathFragment& Path = PathList[EntityIndex];
					int8& PathIndex = Path.PathIndex;
					
					// Get info from fragments
					FVector CurrentLocation = Transform.GetLocation();
					FVector& TargetVector = Path.TargetLocation;
					
					// Calculate path
					if (Path.PathResult.Result != ENavigationQueryResult::Success)
					{
						FNavLocation Result;
						NavSys->GetRandomReachablePointInRadius(Transform.GetLocation(), 3000.f, Result);
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

					// Replicate position to client
					GameStatePtr->SetRatPosition(EntityIndex,  FVector2f(Transform.GetLocation().X, Transform.GetLocation().Y));
				
					// Collision Management
					FVector2f CharaPos2D(CharacterPos.X,CharacterPos.Y);
					FVector2f RatPos2D(Transform.GetLocation().X,Transform.GetLocation().Y);
				
					// Rats
					if((CharaPos2D - RatPos2D).Size() <= 150.0f)
					{
						if(HeroPtr != nullptr)
						{
							HeroPtr->TakeDamageFromRat();
						}
					}
				
					// Fire Balls
					for (int i = 0; i < ArrayOfProjectile.Num(); ++i)
					{
						if(((FVector2f(ArrayOfProjectile[i]->GetActorLocation().X,
							ArrayOfProjectile[i]->GetActorLocation().Y)) -
							(RatPos2D)).Size() <= 150.0f)
						{
							Transform.SetLocation(CurrentLocation + FVector(20000,0, 0));
							// Normalement le Kill est là mais le Defer se joue avant donc...

							ArrayOfProjectileToRemove.Add(ArrayOfProjectile[i]);
						}
					}

					// Dash
					if(HeroPtr != nullptr && HeroPtr->bDashDamage)
					{
						if((CharaPos2D - RatPos2D).Size() <= 600.0f)
						{
							Transform.SetLocation(CurrentLocation + FVector(20000,0, 0));
						}
					
						bMemoryDashDamage = true;
					}
				
					//Sword
					if(HeroPtr != nullptr && HeroPtr->bSwordDamage)
					{
						FVector2f ForwardVector2D(HeroPtr->GetActorForwardVector().X, HeroPtr->GetActorForwardVector().Y);
					
						if(((CharaPos2D + ForwardVector2D) - RatPos2D).Size() < ((CharaPos2D + ForwardVector2D * -1.0f) - RatPos2D).Size())
						{
							if((CharaPos2D - RatPos2D).Size() <= 500.0f)
							{
								Transform.SetLocation(CurrentLocation + FVector(20000,0, 0));
							}
						}

						bMemorySwordDamage = true;
					}

					if(Path.TimeBeforeRecalculatePath <= 0)
					{
						Path.TimeBeforeRecalculatePath = 0.5f;
					}
				}
			}
		}
		else if(NetMode == NM_Client)
		{
			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				// Get fragment data
				FShortVector2D ratPos = GameStatePtr->GetRatsPositions()[EntityIndex];

				if(ratPos.X >= 10000)
				{
					Context.Defer().AddTag<FCanBeInstantiatedTag>(Context.GetEntity(EntityIndex));
					IndexToRemove.Add(EntityIndex);
				}
				
				FTransform& Transform = TransformsList[EntityIndex].GetMutableTransform();
				
				float interpValue = 0.5f;

				FVector interpPos;

				if(FVector2d(ratPos.X + Transform.GetLocation().X,ratPos.Y + Transform.GetLocation().Y).Length() < 2)
				{
					interpPos = FMath::Lerp(FVector(Transform.GetLocation().X, Transform.GetLocation().Y, GameStatePtr->GetSpawnHeightRats()),
					FVector(ratPos.X, ratPos.Y, GameStatePtr->GetSpawnHeightRats()), interpValue);
				}
				else
				{
					interpPos = FVector(ratPos.X, ratPos.Y, GameStatePtr->GetSpawnHeightRats());
				}
				
				Transform.SetLocation(interpPos);
			}
		}
	}));
	
	if(NetMode == NM_Client)
		return;
	
	for (int i = ArrayOfProjectileToRemove.Num() - 1; i >= 0; --i)
	{
		ABDTProjectile* ProjectileToRemove = ArrayOfProjectileToRemove[i];

		ProjectileToRemove->Destroy();
		ArrayOfProjectile.Remove(ProjectileToRemove);
	}

	ArrayOfProjectileToRemove.Empty();
	
	IndexToRemove.Empty();

	if(bMemoryDashDamage)
	{
		HeroPtr->bDashDamage = false;
		bMemoryDashDamage = false;
	}

	if(bMemorySwordDamage)
	{
		HeroPtr->bSwordDamage = false;
		bMemorySwordDamage = false;
	}
}