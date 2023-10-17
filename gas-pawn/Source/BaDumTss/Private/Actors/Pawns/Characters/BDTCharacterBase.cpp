// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/Characters/BDTCharacterBase.h"

#include "EnhancedInputSubsystems.h"
#include "Abilities/BDTGameplayAbility.h"
#include "Abilities/AttributeSets/BDTPrimaryAttributeSet.h"
#include "Abilities/Components/BDTAbilitySystemComponent.h"
#include "Actors/Pawns/Characters/BDTCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABDTCharacterBase::ABDTCharacterBase(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer.SetDefaultSubobjectClass<UBDTCharacterMovementComponent>(CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);

    bAlwaysRelevant = true;

	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("Effect.RemoveOnDeath"));


	//Add the Ability System Component to the character.
	AbilitySystemComponent = CreateDefaultSubobject<UBDTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

}

void ABDTCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const auto PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (!InputMapping.IsNull())
				InputSystem->AddMappingContext(InputMapping.LoadSynchronous(), 1);
			else
				UE_LOG(LogEnhancedInputCharacter, Warning, TEXT("InputMapping is missing in BDTHeroCharacter"));		}
	}
}

FInputActionValue ABDTCharacterBase::GetInputActionValue(TObjectPtr<const UInputAction> ForAction)
{
	const auto PlayerController = Cast<APlayerController>(Controller);

	if (!PlayerController)
		return FInputActionValue::Axis3D::ZeroVector;
	
	const UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	if (!InputSystem)
	{
		UE_LOG(LogEnhancedInputCharacter, Error, TEXT("GetInputActionValue: a variable required is missing"))
		return FInputActionValue::Axis3D::ZeroVector;
	}
	
	return InputSystem->GetPlayerInput()->GetActionValue(ForAction);
}


// Called when the game starts or when spawned
void ABDTCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	//Initializes the attribute set using a data table.
	if (IsValid(AbilitySystemComponent))
	{
		PrimaryAttributeSet = AbilitySystemComponent->GetSet<UBDTPrimaryAttributeSet>();
		AbilitySystemComponent->OnAbilityEnded.AddUObject(this, &ABDTCharacterBase::OnGameplayAbilityEndedInternal);
	}
	
}

void ABDTCharacterBase::OnGameplayAbilityEndedInternal(const FAbilityEndedData& AbilityEndData)
{
	OnGameplayAbilityEnded(AbilityEndData.bWasCancelled, AbilityEndData.AbilitySpecHandle, AbilityEndData.AbilityThatEnded, AbilityEndData.bReplicateEndAbility);
}


int32 ABDTCharacterBase::GetCharacterLevel() const
{
	if (IsValid(PrimaryAttributeSet))
		return PrimaryAttributeSet->GetCharacterLevel();

	return -1.0f;
}

float ABDTCharacterBase::GetHealth() const
{
	if (IsValid(PrimaryAttributeSet))
		return PrimaryAttributeSet->GetHealth();

	return -1.0f;
}

float ABDTCharacterBase::GetMaxHealth() const
{
	if (IsValid(PrimaryAttributeSet))
		return PrimaryAttributeSet->GetMaxHealth();

	return -1.0f;
}


float ABDTCharacterBase::GetMoveSpeed() const
{

	if (IsValid(PrimaryAttributeSet))
		return PrimaryAttributeSet->GetMoveSpeed();

	return -1.0f;
}


bool ABDTCharacterBase::IsAlive() const
{
	return GetHealth() > 0.0f;
}

int32 ABDTCharacterBase::GetAbilityLevel(int32 InputCode) const
{
	return 1.f;
}


void ABDTCharacterBase::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent))
		return;

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
			AbilitiesToRemove.Add(Spec.Handle);
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}
}


void ABDTCharacterBase::GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level, int32 InputCode)
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

void ABDTCharacterBase::ActivateAbility(int32 InputCode)
{
	if (IsValid(AbilitySystemComponent))
		AbilitySystemComponent->AbilityLocalInputPressed(InputCode);
}

void ABDTCharacterBase::CancelAbilityWithTags(const FGameplayTagContainer CancelWithTags)
{
	//The Ability System Component has multiple ways of cancelling and activating abilities.
	//Using tags provides a generic means of categorizing different types of abilities without knowing what specific ability is active.
	AbilitySystemComponent->CancelAbilities(&CancelWithTags);
}



// Run on Server and all clients
void ABDTCharacterBase::Die()
{
	// Only runs on Server
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	OnCharacterDied.Broadcast(this);

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	if (DeathMontage)
		PlayAnimMontage(DeathMontage);
	else
		FinishDying();
}

void ABDTCharacterBase::FinishDying()
{
	Destroy();
}

void ABDTCharacterBase::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->bStartupEffectsApplied)
		return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (const TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		if (FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext); NewHandle.IsValid())
			AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
	}

	AbilitySystemComponent->bStartupEffectsApplied = true;
}

