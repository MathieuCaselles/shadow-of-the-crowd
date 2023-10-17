// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Abilities/AttributeSets/BDTPrimaryAttributeSet.h"
#include "Abilities/Components/BDTAbilitySystemComponent.h"
#include "Actors/Pawns/Characters/BDTCharacterMovementComponent.h"
#include "BaDumTss/BaDumTss.h"
#include "BaDumTss/BaDumTssGameModeBase.h"
#include "BaDumTss/Utils/EnhancedInputUtils.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ABDTHeroCharacter::ABDTHeroCharacter(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
// TODO peut être supprimé dans l'éditeur donc mettre un assert
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Makes sure that the animations play on the Server so that we can use bone and socket transforms
	// to do things like spawning projectiles and other FX.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());
}

// Called to bind functionality to input
void ABDTHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	check(PlayerInputComponent);

    UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    // You can bind to any of the trigger events here by changing the "ETriggerEvent" enum value
	
	Input->BindAction(GetInputActionFromImc(*InputMapping.Get(),EIMC_HeroCharacter::Move),
		ETriggerEvent::Triggered, this, &ABDTHeroCharacter::Move);
	Input->BindAction(GetInputActionFromImc(*InputMapping.Get(),EIMC_HeroCharacter::Look),
	ETriggerEvent::Triggered, this, &ABDTHeroCharacter::Look);
}

void ABDTHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (IsValid(AbilitySystemComponent))
	{
		AddStartupEffects();
		// Forcibly set the DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);
	}

}


void ABDTHeroCharacter::FinishDying()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (ABaDumTssGameModeBase* GameMode = Cast<ABaDumTssGameModeBase>(GetWorld()->GetAuthGameMode()))
			GameMode->HeroDied(GetController());
	}
	Super::FinishDying();
}

void ABDTHeroCharacter::TakeDamageFromRat() const
{
	if(AbilitySystemComponent->HasMatchingGameplayTag(InvulnerableToDamageTag))
		return;
	
	const UGameplayEffect* GameplayEffect = RatDamage->GetDefaultObject<UGameplayEffect>();
	AbilitySystemComponent->ApplyGameplayEffectToSelf(GameplayEffect, 0.f, AbilitySystemComponent->MakeEffectContext());
}

TArray<ABDTProjectile*>& ABDTHeroCharacter::GetArrayProjectile()
{
	return ArrayOfProjectiles;
}

UBoxComponent* ABDTHeroCharacter::GetSwordBoxCollision() const
{
	return SwordBoxCollision;
}



/**
* On the Server, Possession happens before BeginPlay.
* On the Client, BeginPlay happens before Possession.
*/
void ABDTHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ABDTHeroCharacter::Look(const FInputActionInstance& Instance)
{
	const FVector2D Direction = Instance.GetValue().Get<FVector2D>();
	const FRotator TargetRotation = FRotator(0.f, FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X)), 0.f);

	// add 90 because of right stick
	SetActorRotation(TargetRotation + FRotator(0.0f, 90.0f, 0.0f));
}


void ABDTHeroCharacter::Move(const FInputActionInstance& Instance)
{
	const FVector2D AxisValue = Instance.GetValue().Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("AxisValue : %s"), *AxisValue.ToString());
	// MoveForward
	AddMovementInput(UKismetMathLibrary::GetForwardVector(GetControlRotation()), AxisValue.Y);
	
	//MoveRight
	AddMovementInput(UKismetMathLibrary::GetRightVector(GetControlRotation()), AxisValue.X);
}



