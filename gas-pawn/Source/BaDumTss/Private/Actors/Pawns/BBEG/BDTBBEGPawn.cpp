// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/BBEG/BDTBBEGPawn.h"

#include "EnhancedInputComponent.h"
#include "VectorTypes.h"
#include "Abilities/AttributeSets/BDTPawnAttributeSet.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "BaDumTss/Utils/EnhancedInputUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ABDTBBEGPawn::ABDTBBEGPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Instantiating your class Components
	CameraOriginLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("CameraOriginLocation"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));

	// Attaching your class Components to the dDefault Character's Skeletal Mesh Component.
	CameraOriginLocation->SetupAttachment(GetRootComponent());
	CameraComp->SetupAttachment(GetRootComponent());
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());

	//Attach the Camera to the SpringArmComponent
	CameraComp->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepRelativeTransform);
	
	SpringArm->TargetArmLength = 5000.0f;
	SpringArm->bEnableCameraLag = true;

	
	SM_Cursor = CreateDefaultSubobject<UStaticMesh>(TEXT("SM_Cursor"));
	SM_Cursor->SetMaterial(0, CursorMaterial);

	SMC_Cursor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMC_Cursor"));
	SMC_Cursor->SetStaticMesh(SM_Cursor);
}

void ABDTBBEGPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// You can bind to any of the trigger events here by changing the "ETriggerEvent" enum value
	
	Input->BindAction(GetInputActionFromImc(*InputMapping.Get(),EIMC_HeroCharacter::Move),
		ETriggerEvent::Triggered, this, &ABDTBBEGPawn::Move);
}

void ABDTBBEGPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (IsValid(AbilitySystemComponent))
	{
		AddStartupEffects();
	}
}

void ABDTBBEGPawn::BeginPlay()
{
	Super::BeginPlay();
	
	const FVector3d CursorLocation = {GetActorLocation().X + cursorDistanceFromBBEG, GetActorLocation().Y, GetActorLocation().Z};
	SMC_Cursor->SetWorldLocation(CursorLocation);
}

void ABDTBBEGPawn::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABDTBBEGPawn::Move(const FInputActionInstance& Instance)
{
	const FVector2D AxisValue = Instance.GetValue().Get<FVector2D>();
	const FVector2D ReversedAxisValue{AxisValue.Y, AxisValue.X};

	const FVector2D CursorLocation{SMC_Cursor->GetComponentLocation().X, SMC_Cursor->GetComponentLocation().Y};
	const FVector2D BBEGLocation{GetActorLocation().X, GetActorLocation().Y};

	const FVector2D DifferenceVector = CursorLocation - BBEGLocation;

	const double Angle = UE::Geometry::SignedAngleR(ReversedAxisValue, DifferenceVector);
	const FVector2D NormalizedDifferenceVector = DifferenceVector.GetSafeNormal(0.0001);
	const FVector2D NewPosition = NormalizedDifferenceVector.GetRotated(Angle / LerpValue);
	SMC_Cursor->SetWorldLocation({NewPosition.X, NewPosition.Y, 550.0f});
}
