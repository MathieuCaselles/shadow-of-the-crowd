// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BADUMTSS_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	
	/**
    * Allow to set if keyboard and gamepad should be associate to same player
    */
    UFUNCTION(BlueprintCallable, Category = "Controllers")
    void AssociateGamepadToKeyboard(bool const ShouldBeAssociated);
};
