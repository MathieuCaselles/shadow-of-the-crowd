// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/BBEG/Abilities/BDTA_SummonRats.h"

#include "BDTGameState.h"
#include "Actors/Pawns/BBEG/BDTBBEGPawn.h"
#include "Actors/Spawners/BDTRatsSpawner.h"
#include "UM_WorldSubsystem.h"

UBDTA_SummonRats::UBDTA_SummonRats()
{
	//UMSubsystem = GetWorld()->GetSubsystem<UUM_WorldSubsystem>();
	//GetWorld()->GetSubsystem<UUM_WorldSubsystem>();
	GameStatePtr = nullptr;
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	const FGameplayTag SummonRatsTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Summon.Rats"));
	AbilityTags.AddTag(SummonRatsTag);
	ActivationOwnedTags.AddTag(SummonRatsTag);
}

void UBDTA_SummonRats::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	UE_LOG(LogGameplayAbility, Display, TEXT("OnAvatarSet SUMMON RATS"));
	GameStatePtr = Cast<ABDTGameState>(GetWorld()->GetGameState());

	//check(GameStatePtr);
}
void UBDTA_SummonRats::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData * TriggerEventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("ActivateAbility SUMMON RATS"));
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogGameplayAbility, Display, TEXT("Can't activate ability SUMMON RATS because of Cooldown or missing resources"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	
	if (GetOwningActorFromActorInfo()->GetLocalRole() != ROLE_Authority)
		return;
	
	SpawnRats();
}

void UBDTA_SummonRats::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("END ABILITY SUMMON RATS"));
	if(GameStatePtr != nullptr)
	{
		GameStatePtr->SetBadumIsSpawningRats(false);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UBDTA_SummonRats::SpawnRats() const
{
	UE_LOG(LogGameplayAbility, Display, TEXT("Badum is spawning rats"));
	if(GameStatePtr != nullptr)
	{
		GameStatePtr->SetBadumIsSpawningRats(true);
	}
}


void UBDTA_SummonRats::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (GetOwningActorFromActorInfo()->GetLocalRole() != ROLE_Authority)
		return;
	
	UE_LOG(LogGameplayAbility, Display, TEXT("OnCancelled SUMMON Rats"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
