// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/BBEG/Abilities/BDTA_SummonBeacon.h"
#include "Actors/Pawns/BBEG/BDTBBEGPawn.h"

#include "BDTGameState.h"
#include "Actors/Pawns/Characters/BBEGBeacon/BDTBeaconBase.h"


UBDTA_SummonBeacon::UBDTA_SummonBeacon()
{
	GameStatePtr = nullptr;
	BBEG = nullptr;
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	const FGameplayTag SummonBeaconTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Summon.Beacon"));
	AbilityTags.AddTag(SummonBeaconTag);
	ActivationOwnedTags.AddTag(SummonBeaconTag);

}

void UBDTA_SummonBeacon::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	UE_LOG(LogGameplayAbility, Display, TEXT("OnAvatarSet SUMMON RATS"));
	GameStatePtr = Cast<ABDTGameState>(GetWorld()->GetGameState());
	BBEG = Cast<ABDTCharacterBase>(GetAvatarActorFromActorInfo());

	//check(GameStatePtr);
}

void UBDTA_SummonBeacon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogGameplayAbility, Display, TEXT("Can't activate ability SUMMON BEACON because of Cooldown or missing resources"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	// Only spawn beacons on the Client.*/
	if (GetOwningActorFromActorInfo()->GetLocalRole() != ROLE_Authority)
		return;
	
	if (!BBEG || !GameStatePtr)
	{
		UE_LOG(LogGameplayAbility, Error, TEXT("BBEG Not found oskour"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	ApplyCooldown(Handle, ActorInfo, ActivationInfo);

	const FTransform SpawnTransform{BBEG->GetTransform()};
	SummonBeacon(SpawnTransform, BeaconToSpawn);
}

void UBDTA_SummonBeacon::SummonBeacon_Implementation(const FTransform& SpawnTransform, TSubclassOf<ABDTBeaconBase> Beacon)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("LANCEMENT DE SUMMON BEACON"));
	FTransform NewSpawnTransform{SpawnTransform};
	NewSpawnTransform.SetScale3D({1.f, 1.f, 1.f});
	const FVector SpawnLocation = NewSpawnTransform.GetLocation();

	NewSpawnTransform.SetLocation(FVector{SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z});

	ABDTBeaconBase* BeaconBP = GetWorld()->SpawnActorDeferred<ABDTBeaconBase>(Beacon, NewSpawnTransform, GetOwningActorFromActorInfo(),
		BBEG, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	BeaconBP->FinishSpawning(NewSpawnTransform);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

bool UBDTA_SummonBeacon::SummonBeacon_Validate(const FTransform& CursorTransform, TSubclassOf<ABDTBeaconBase> Beacon)
{
	return true;
}

void UBDTA_SummonBeacon::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("OnCancelled SUMMON BEACON"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UBDTA_SummonBeacon::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("OnCancelled SUMMON BEACON"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UBDTA_SummonBeacon::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("EventReceived SUMMON BEACON"));

	
}
