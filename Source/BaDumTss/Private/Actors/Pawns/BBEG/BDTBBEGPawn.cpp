// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/BBEG/BDTBBEGPawn.h"

#include "BDTGameState.h"
#include "EnhancedInputComponent.h"
#include "VectorTypes.h"
#include "Abilities/AttributeSets/BDTPawnAttributeSet.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "BaDumTss/BaDumTssGameModeBase.h"
#include "BaDumTss/Utils/CustomMathUtils.h"
#include "BaDumTss/Utils/EnhancedInputUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/ActorChannel.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ABDTBBEGPawn::ABDTBBEGPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(true);
	NetUpdateFrequency = 60.f;

	GameStatePtr = nullptr;
	
	// Instantiating your class Components
	CameraOriginLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("CameraOriginLocation"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	// Attaching your class Components to the dDefault Character's Skeletal Mesh Component.
	CameraOriginLocation->SetupAttachment(GetRootComponent());
	CameraComp->SetupAttachment(GetRootComponent());
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());

	//Attach the Camera to the SpringArmComponent
	CameraComp->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepRelativeTransform);
	
	SpringArm->TargetArmLength = 5000.0f;
	SpringArm->bEnableCameraLag = true;

	

	SMC_Cursor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMC_Cursor"));
	SMC_Cursor->SetupAttachment(GetRootComponent());
	SMC_Cursor->SetIsReplicated(true);

	CursorDistanceFromBBEG = 2500.0f;
	LerpValue = 5.f;
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
	

	UClass* LoadedBpAsset = TSoftClassPtr<AActor>(FSoftObjectPath(TEXT("Blueprint'/Game/TMPS/BP/BP_HeroToken.BP_HeroToken_C'"))).LoadSynchronous();
	const FVector Location = GetActorLocation();
	const FRotator Rotation = FRotator::ZeroRotator;
	HeroToken = GetWorld()->SpawnActor(LoadedBpAsset, &Location, &Rotation);

	GameStatePtr = Cast<ABDTGameState>(UGameplayStatics::GetGameState(GetWorld()));
	GameStatePtr->SetBadumCursorPosition(FVector2f(SMC_Cursor->GetComponentLocation().X, SMC_Cursor->GetComponentLocation().Y));

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PawnAttributeSet->GetEnergyAttribute()).AddUObject(this, &ABDTBBEGPawn::OnEnergyChangedInternal);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PawnAttributeSet->GetEnergyRegenRateAttribute()).AddUObject(this, &ABDTBBEGPawn::OnEnergyRegenRateChangedInternal);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PawnAttributeSet->GetGoldusAttribute()).AddUObject(this, &ABDTBBEGPawn::OnGoldusChangedInternal);
	}

	
	check(HeroToken);
	check(GameStatePtr);

	const FVector3d CursorLocation = {GetActorLocation().X + CursorDistanceFromBBEG, GetActorLocation().Y, GetActorLocation().Z};
	SMC_Cursor->SetWorldLocation(CursorLocation);
	SetCursorLocationReplicated(CursorLocation);
}

void ABDTBBEGPawn::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetActorLocation(HeroToken->GetActorLocation());
}

void ABDTBBEGPawn::Move(const FInputActionInstance& Instance)
{
	const FVector2D AxisValue = Instance.GetValue().Get<FVector2D>();
	const FVector ReversedAxisValue{AxisValue.Y, AxisValue.X, 0.0f};
	
	const FVector CursorLocation = SMC_Cursor->GetComponentLocation();
	const FVector BBEGLocation = GetActorLocation();

	const FVector DifferenceVector = CursorLocation - BBEGLocation;
	const FVector NormalizedDifferenceVector = DifferenceVector.GetSafeNormal(0.0001);
	
	const double Angle =  GetSignedAngleFromVectorAToVectorB(DifferenceVector, ReversedAxisValue);
	
	const FVector NewPosition = NormalizedDifferenceVector.RotateAngleAxis(Angle / LerpValue, {0.f, 0.f,1.f}) * CursorDistanceFromBBEG;

	const FVector HeroTokenLocation = HeroToken->GetActorLocation();
	const FVector NewSmcCursorLocation =  FVector{ NewPosition.X + HeroTokenLocation.X, NewPosition.Y + HeroTokenLocation.Y, BBEGLocation.Z};

	SMC_Cursor->SetWorldLocation(NewSmcCursorLocation);
	SetCursorLocationReplicated(NewSmcCursorLocation);
}

void ABDTBBEGPawn::SetCursorLocationReplicated_Implementation(const FVector& Location)
{
	GameStatePtr->SetBadumCursorPosition(FVector2f(Location.X, Location.Y));
}

bool ABDTBBEGPawn::SetCursorLocationReplicated_Validate(const FVector& Location)
{
	return true;
}


FTransform ABDTBBEGPawn::GetCursorTransform() const
{
	return SMC_Cursor->GetComponentTransform();
}


void ABDTBBEGPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABDTBBEGPawn, SMC_Cursor);
}

void ABDTBBEGPawn::OnEnergyChangedInternal(const FOnAttributeChangeData& Data)
{
	OnEnergyChanged(Data.OldValue, Data.NewValue);
	
}

void ABDTBBEGPawn::OnEnergyRegenRateChangedInternal(const FOnAttributeChangeData& Data)
{
	OnEnergyRegenRateChanged(Data.OldValue, Data.NewValue);
	
}

void ABDTBBEGPawn::OnGoldusChangedInternal(const FOnAttributeChangeData& Data)
{
	OnGoldusChanged(Data.OldValue, Data.NewValue);
	
}

