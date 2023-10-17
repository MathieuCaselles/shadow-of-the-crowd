// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/AttributeSets/BDTPrimaryAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"

#include "Actors/Pawns/Characters/BDTCharacterBase.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroController.h"
#include "Net/UnrealNetwork.h"

UBDTPrimaryAttributeSet::UBDTPrimaryAttributeSet()
{
}

void UBDTPrimaryAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	UE_LOG(LogTemp, Display, TEXT("Attribut qui va être modifié : %s  | Nouvelle valeur: %f"), *Attribute.AttributeName, NewValue);
}
void UBDTPrimaryAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	// Clamp Health
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
}

void UBDTPrimaryAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBDTPrimaryAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBDTPrimaryAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBDTPrimaryAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBDTPrimaryAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBDTPrimaryAttributeSet, CharacterLevel, COND_None, REPNOTIFY_Always);

}

void UBDTPrimaryAttributeSet::OnRep_Health(const FGameplayAttributeData& oldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBDTPrimaryAttributeSet, Health, oldHealth);
}

void UBDTPrimaryAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& oldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBDTPrimaryAttributeSet, MaxHealth, oldMaxHealth);
}

void UBDTPrimaryAttributeSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBDTPrimaryAttributeSet, HealthRegenRate, OldHealthRegenRate);
}

void UBDTPrimaryAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBDTPrimaryAttributeSet, MoveSpeed, OldMoveSpeed);
}

void UBDTPrimaryAttributeSet::OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBDTPrimaryAttributeSet, CharacterLevel, OldCharacterLevel);
}