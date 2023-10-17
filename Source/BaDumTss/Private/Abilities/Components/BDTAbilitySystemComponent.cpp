// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/Components/BDTAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

UBDTAbilitySystemComponent::UBDTAbilitySystemComponent()
{
}

UBDTAbilitySystemComponent* UBDTAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor,
	bool LookForComponent)
{
	return Cast<UBDTAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}

void UBDTAbilitySystemComponent::ReceiveDamage(UBDTAbilitySystemComponent* SourceASC, float UnmitigatedDamage,
	float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}
