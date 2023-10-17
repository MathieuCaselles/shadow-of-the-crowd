// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/Characters/Hero/BDTHeroController.h"

#include "AbilitySystemComponent.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"

void ABDTHeroController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	
	if (ABDTHeroCharacter* HeroCharacter = Cast<ABDTHeroCharacter>(P))
	{
		HeroCharacter->GetAbilitySystemComponent()->InitAbilityActorInfo(HeroCharacter, HeroCharacter);
	}
}
