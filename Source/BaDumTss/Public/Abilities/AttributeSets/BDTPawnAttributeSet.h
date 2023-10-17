// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "BDTPawnAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)	

/**
 * 
 */
UCLASS()
class BADUMTSS_API UBDTPawnAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBDTPawnAttributeSet();

	// AttributeSet override
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "Energy|PawnAttribute", ReplicatedUsing = OnRep_Energy)
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS(UBDTPawnAttributeSet, Energy);

	UPROPERTY(BlueprintReadOnly, Category = "Energy|PawnAttribute", ReplicatedUsing = OnRep_MaxEnergy)
	FGameplayAttributeData MaxEnergy;
	ATTRIBUTE_ACCESSORS(UBDTPawnAttributeSet, MaxEnergy);

	// Energy regen rate will passively increase Energy every second
	UPROPERTY(BlueprintReadOnly, Category = "Energy|PawnAttribute", ReplicatedUsing = OnRep_EnergyRegenRate)
	FGameplayAttributeData EnergyRegenRate;
	ATTRIBUTE_ACCESSORS(UBDTPawnAttributeSet, EnergyRegenRate)

	UPROPERTY(BlueprintReadOnly, Category = "Goldus|PawnAttribute", ReplicatedUsing = OnRep_Goldus)
	FGameplayAttributeData Goldus;
	ATTRIBUTE_ACCESSORS(UBDTPawnAttributeSet, Goldus)
	
	UPROPERTY(BlueprintReadOnly, Category = "Goldus|PawnAttribute", ReplicatedUsing = OnRep_MaxGoldus)
	FGameplayAttributeData MaxGoldus;
	ATTRIBUTE_ACCESSORS(UBDTPawnAttributeSet, MaxGoldus)

	UPROPERTY(BlueprintReadOnly, Category = "Cursor|PawnAttribute", ReplicatedUsing = OnRep_CursorMoveSpeed)
	FGameplayAttributeData CursorMoveSpeed;
	ATTRIBUTE_ACCESSORS(UBDTPawnAttributeSet, CursorMoveSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "SpawnRate|PawnAttribute", ReplicatedUsing = OnRep_SpawnRate)
	FGameplayAttributeData SpawnRate;
	ATTRIBUTE_ACCESSORS(UBDTPawnAttributeSet, SpawnRate)

protected:

	UFUNCTION()
	virtual void OnRep_Energy(const FGameplayAttributeData& oldEnergy);

	UFUNCTION()
	virtual void OnRep_MaxEnergy(const FGameplayAttributeData& oldMaxEnergy);

	UFUNCTION()
	virtual void OnRep_EnergyRegenRate(const FGameplayAttributeData& oldEnergyRegenRate);

	UFUNCTION()
	virtual void OnRep_Goldus(const FGameplayAttributeData& oldGoldus);

	UFUNCTION()
	virtual void OnRep_MaxGoldus(const FGameplayAttributeData& oldMaxGoldus);

	UFUNCTION()
	virtual void OnRep_CursorMoveSpeed(const FGameplayAttributeData& oldCursorMoveSpeed);

	UFUNCTION()
	virtual void OnRep_SpawnRate(const FGameplayAttributeData& oldSpawnRate);
};
