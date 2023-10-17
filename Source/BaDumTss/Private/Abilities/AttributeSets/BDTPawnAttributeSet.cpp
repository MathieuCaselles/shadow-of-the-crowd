// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AttributeSets/BDTPawnAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UBDTPawnAttributeSet::UBDTPawnAttributeSet()
{
}

void UBDTPawnAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	UE_LOG(LogTemp, Display, TEXT("Attribut qui va être modifié : %s  | Nouvelle valeur: %f"), *Attribute.AttributeName, NewValue);
}

void UBDTPawnAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	// Clamp Health
	if (Data.EvaluatedData.Attribute == GetEnergyAttribute())
		SetEnergy(FMath::Clamp(GetEnergy(), 0.0f, GetMaxEnergy()));
}

void UBDTPawnAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBDTPawnAttributeSet, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBDTPawnAttributeSet, MaxEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBDTPawnAttributeSet, EnergyRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBDTPawnAttributeSet, CursorMoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBDTPawnAttributeSet, Goldus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBDTPawnAttributeSet, MaxGoldus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBDTPawnAttributeSet, SpawnRate, COND_None, REPNOTIFY_Always);

}

void UBDTPawnAttributeSet::OnRep_Energy(const FGameplayAttributeData& oldEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBDTPawnAttributeSet, Energy, oldEnergy);
}

void UBDTPawnAttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& oldMaxEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBDTPawnAttributeSet, MaxEnergy, oldMaxEnergy);
}

void UBDTPawnAttributeSet::OnRep_EnergyRegenRate(const FGameplayAttributeData& oldEnergyRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBDTPawnAttributeSet, EnergyRegenRate, oldEnergyRegenRate);
}

void UBDTPawnAttributeSet::OnRep_Goldus(const FGameplayAttributeData& oldGoldus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBDTPawnAttributeSet, Goldus, oldGoldus);
}

void UBDTPawnAttributeSet::OnRep_MaxGoldus(const FGameplayAttributeData& oldMaxGoldus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBDTPawnAttributeSet, Goldus, oldMaxGoldus);
}

void UBDTPawnAttributeSet::OnRep_CursorMoveSpeed(const FGameplayAttributeData& oldCursorMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBDTPawnAttributeSet, CursorMoveSpeed, oldCursorMoveSpeed);
}

void UBDTPawnAttributeSet::OnRep_SpawnRate(const FGameplayAttributeData& oldSpawnRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBDTPawnAttributeSet, SpawnRate, oldSpawnRate);
}
