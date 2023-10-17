// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "GameMapsSettings.h"

void UMyGameInstance::AssociateGamepadToKeyboard(bool const ShouldBeAssociated) {
	// Get GameSettings
	auto const Settings = const_cast<UGameMapsSettings*>(GetDefault<UGameMapsSettings>());

	// It will affect Gamepad from Controller 1 (and keyboard to 1) if they should not be associated
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("GamepadAssociated")));
	Settings->bOffsetPlayerGamepadIds = !ShouldBeAssociated;
	Settings->SaveConfig();
}
