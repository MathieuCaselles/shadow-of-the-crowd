// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/BDTPawnBase.h"

#include "EnhancedInputSubsystems.h"
#include "Abilities/BDTGameplayAbility.h"
#include "Abilities/AttributeSets/BDTPawnAttributeSet.h"
#include "Abilities/Components/BDTAbilitySystemComponent.h"
#include "Actors/Pawns/BDTPawnMovementComponent.h"

// Sets default values


ABDTPawnBase::ABDTPawnBase() :
Super()
{
	//Add the Ability System Component to the pawn.
	AbilitySystemComponent = CreateDefaultSubobject<UBDTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

// Called when the game starts or when spawned
void ABDTPawnBase::BeginPlay()
{
	Super::BeginPlay();

	//Initializes the attribute set using a data table.
	if (IsValid(AbilitySystemComponent))
	{
		PawnAttributeSet = AbilitySystemComponent->GetSet<UBDTPawnAttributeSet>();
		AbilitySystemComponent->OnAbilityEnded.AddUObject(this, &ABDTPawnBase::OnGameplayAbilityEndedInternal);
	}
}

void ABDTPawnBase::OnGameplayAbilityEndedInternal(const FAbilityEndedData& AbilityEndData)
{
	OnGameplayAbilityEnded(AbilityEndData.bWasCancelled, AbilityEndData.AbilitySpecHandle, AbilityEndData.AbilityThatEnded, AbilityEndData.bReplicateEndAbility);
}

void ABDTPawnBase::GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level, int32 InputCode)
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || !IsValid(AbilityClass))
		return;

	if (UGameplayAbility* Ability = AbilityClass->GetDefaultObject<UGameplayAbility>(); IsValid(Ability))
	{
		// create the new ability spec struct. Ability specs contain metadata about the ability, like what level they're set to, as well as a reference to the ability.
		const FGameplayAbilitySpec AbilitySpec(
			Ability,
			Level,
			InputCode
		);

		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
}

void ABDTPawnBase::ActivateAbility(int32 InputCode)
{
	if (IsValid(AbilitySystemComponent))
		AbilitySystemComponent->AbilityLocalInputPressed(InputCode);
}

void ABDTPawnBase::CancelAbilityWithTags(const FGameplayTagContainer CancelWithTags)
{
	//The Ability System Component has multiple ways of cancelling and activating abilities.
	//Using tags provides a generic means of categorizing different types of abilities without knowing what specific ability is active.
	AbilitySystemComponent->CancelAbilities(&CancelWithTags);
}

int32 ABDTPawnBase::GetAbilityLevel(int32 InputCode) const
{
	return 1.f;
}

void ABDTPawnBase::RemovePawnAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent))
		return;

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'PawnAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && PawnAbilities.Contains(Spec.Ability->GetClass()))
			AbilitiesToRemove.Add(Spec.Handle);
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}
}

int32 ABDTPawnBase::GetEnergy() const
{
	if (IsValid(PawnAttributeSet))
		return PawnAttributeSet->GetEnergy();

	return -1.0f;
}

int32 ABDTPawnBase::GetMaxEnergy() const
{
	if (IsValid(PawnAttributeSet))
		return PawnAttributeSet->GetMaxEnergy();

	return -1.0f;
}

int32 ABDTPawnBase::GetEnergyRegenRate() const
{
	if (IsValid(PawnAttributeSet))
		return PawnAttributeSet->GetEnergyRegenRate();

	return -1.0f;
}

int32 ABDTPawnBase::GetGoldus() const
{
	if (IsValid(PawnAttributeSet))
		return PawnAttributeSet->GetGoldus();

	return -1.0f;
}

int32 ABDTPawnBase::GetCursorMoveSpeed() const
{
	if (IsValid(PawnAttributeSet))
		return PawnAttributeSet->GetCursorMoveSpeed();

	return -1.0f;
}

int32 ABDTPawnBase::GetSpawnRate() const
{
	if (IsValid(PawnAttributeSet))
		return PawnAttributeSet->GetSpawnRate();

	return -1.0f;
}

void ABDTPawnBase::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->bStartupEffectsApplied)
		return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (const TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		if (FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1.0f, EffectContext); NewHandle.IsValid())
			AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
	}

	AbilitySystemComponent->bStartupEffectsApplied = true;
}

// Called every frame
void ABDTPawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



// Called to bind functionality to input
void ABDTPawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const auto PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (!InputMapping.IsNull())
				InputSystem->AddMappingContext(InputMapping.LoadSynchronous(), 1);
			else
				UE_LOG(LogEnhancedInputPawn, Warning, TEXT("InputMapping is missing in BDTPawn"));		}
	}
}

FInputActionValue ABDTPawnBase::GetInputActionValue(TObjectPtr<const UInputAction> ForAction)
{
	const auto PlayerController = Cast<APlayerController>(Controller);

	if (!PlayerController)
		return FInputActionValue::Axis3D::ZeroVector;
	
	const UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	if (!InputSystem)
	{
		UE_LOG(LogEnhancedInputPawn, Error, TEXT("GetInputActionValue: a variable required is missing"))
		return FInputActionValue::Axis3D::ZeroVector;
	}
	
	return InputSystem->GetPlayerInput()->GetActionValue(ForAction);
}

