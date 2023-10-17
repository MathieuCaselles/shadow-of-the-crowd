// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/WeaponBase/BDTDash.h"
#include "Abilities/BDTGameplayAbility.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Pawns/Characters/BDTCharacterMovementComponent.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "BaDumTss/Utils/EnhancedInputUtils.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABDTDash::ABDTDash()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	DashSpeed = 1500.f;
	DashTime = 0.5f;
	DelayToDestroyDashEffectAfterAbilityEnds = 1.f;
	
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ABDTDash::BeginPlay()
{
	Super::BeginPlay();

	if(CharacterPointer == nullptr)
		return;

	Start = CharacterPointer->GetActorLocation();
	MoveDirection =  CharacterPointer->GetInputActionValue(
		GetInputActionFromImc(*CharacterPointer->InputMapping.Get(),EIMC_HeroCharacter::Move).Get()
		).Get<FVector2D>();
	MoveDirection.Normalize();
	
	MovementComponent = Cast<UBDTCharacterMovementComponent>(CharacterPointer->GetCharacterMovement());
	MovementComponent->SetMovementMode(MOVE_Custom, static_cast<uint8>(ECustomMovementMode::Dashing));

	UGameplayStatics::PlaySoundAtLocation(this, TrailSoundToPlay, {0.f, 0.f, 0.f});
	if (!TrailEffectToSpawn->IsValid())
		return;

	UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(TrailEffectToSpawn, CharacterPointer->GetMesh(),
		EName::None, Start, FRotator(0.f, 0.f, 0.f),
		EAttachLocation::KeepWorldPosition, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandleToDestroyEffect,[SpawnedEffect]()
	{
		SpawnedEffect->DestroyComponent();
	}, DashTime + DelayToDestroyDashEffectAfterAbilityEnds, false);

	CharacterPointer->GetMesh()->SetVisibility(false);
	SetLifeSpan(DashTime); 
}



// Called every frame
void ABDTDash::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	const float DistanceToAdd = DashSpeed * DeltaTime;
	
	const FVector CurrentLocation = CharacterPointer->GetActorLocation();
	
	// We multiply the Y of the MoveVector with the X of the Location and vice-versa to have the correct vector
	const FVector NewLocation(CurrentLocation.X + DistanceToAdd * MoveDirection.Y, CurrentLocation.Y + DistanceToAdd * MoveDirection.X, CurrentLocation.Z);
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(CharacterPointer);
	if (FHitResult HitResult; GetWorld()->LineTraceSingleByChannel(HitResult, CurrentLocation, NewLocation, ECC_WorldStatic, CollisionParams))
	{
		CharacterPointer->SetActorLocation({HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, HitResult.ImpactPoint.Z - 1.f}); // -1.f is to allow GetGroundPosition to work else its too high
		SetCharacterFeetPosition(GetGroundPosition());
		Destroy();
		return;
	}
	
	CharacterPointer->SetActorLocation(NewLocation);
	
	if (GetCharacterFeetPosition().Z > GetGroundPosition().Z)
		SetCharacterFeetPosition(GetGroundPosition());
	
}

FVector ABDTDash::GetCharacterFeetPosition() const
{
	return CharacterPointer->GetActorLocation() - FVector(0.f, 0.f, CharacterPointer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
}

FVector ABDTDash::GetGroundPosition() const
{
	const FVector StartTrace = CharacterPointer->GetActorLocation();
	const FVector EndTrace = StartTrace - FVector(0.f, 0.f, CharacterPointer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(CharacterPointer);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_WorldStatic, CollisionParams))
		return HitResult.ImpactPoint;

	return StartTrace;
}

void ABDTDash::SetCharacterFeetPosition(const FVector& NewPosition) const
{
	CharacterPointer->SetActorLocation(NewPosition + FVector(0.f, 0.f, CharacterPointer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
}

void ABDTDash::Destroyed()
{
	Super::Destroyed();
	if(GetLocalRole() != ROLE_Authority || CharacterPointer == nullptr)
		return;
	
	const FVector GroundPosition = GetGroundPosition();
	const FRotator CharacterRotation = CharacterPointer->GetTransform().GetRotation().Rotator();

	const FActorSpawnParameters SpawnInfo; 

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ZoneEffectToSpawn, GroundPosition, CharacterRotation);

	GetWorld()->SpawnActor<AActor>(ZoneDecalsToSpawn, GroundPosition, CharacterRotation, SpawnInfo);
	UGameplayStatics::PlaySoundAtLocation(this, ZoneSoundToPlay, GroundPosition);

	MovementComponent->SetMovementMode(MOVE_Falling);
	CharacterPointer->GetMesh()->SetVisibility(true);
}


