#pragma once

#include "CoreMinimal.h"

#include "EnhancedActionKeyMapping.h"
#include "InputAction.h"
#include "InputMappingContext.h"


UENUM(BlueprintType)
enum class EIMC_HeroCharacter : uint8 
{
	// 0
	Move			UMETA(DisplayName = "IA_Move"),
	// 1
	Look 			UMETA(DisplayName = "IA_Look"),
	// 2
	SummonFireball  UMETA(DisplayName = "IA_SummonFireball"),
	// 3
	AttackWithSword  UMETA(DisplayName = "IA_AttackWithSword"),
	// 4
	LightningDash  UMETA(DisplayName = "IA_LightningDash"),
};


inline TArray<TObjectPtr<const UInputAction>> GetOrderedInputActions(const UInputMappingContext& InputMappingContext)
{
	TArray<TObjectPtr<const UInputAction>> OrderedActions;
	for (const FEnhancedActionKeyMapping& Mapping : InputMappingContext.GetMappings())
	{
		if (Mapping.Action)
			OrderedActions.AddUnique(Mapping.Action);
	}
	return OrderedActions;
}

inline TObjectPtr<const UInputAction> GetInputActionFromImc(const UInputMappingContext& InputMappingContext,
                                                            EIMC_HeroCharacter ActionKey)
{
	return GetOrderedInputActions(InputMappingContext)[static_cast<int>(ActionKey)];
}
