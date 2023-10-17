// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/BBEG/BDTBBEGController.h"

#include "Actors/Pawns/BBEG/BDTBBEGPawn.h"

void ABDTBBEGController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	if (ABDTBBEGPawn* BBEGPawn = Cast<ABDTBBEGPawn>(P))
	{
		BBEGPawn->GetAbilitySystemComponent()->InitAbilityActorInfo(BBEGPawn, BBEGPawn);
	}
}
