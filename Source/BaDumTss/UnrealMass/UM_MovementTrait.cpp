// Fill out your copyright notice in the Description page of Project Settings.


#include "UM_MovementTrait.h"

#include "MassEntityTemplateRegistry.h"

void UUM_MovementTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FSimpleMovementFragment>();
}
