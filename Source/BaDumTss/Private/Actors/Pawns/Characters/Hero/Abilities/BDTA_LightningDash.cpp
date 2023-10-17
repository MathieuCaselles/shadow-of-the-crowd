// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/Characters/Hero/Abilities/BDTA_LightningDash.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Abilities/AbilityTask/BDTAT_PlayMontageAndWaitForEvent.h"
#include "Actors/Pawns/Characters/BDTCharacterMovementComponent.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "Actors/WeaponBase/BDTDash.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UBDTA_LightningDash::UBDTA_LightningDash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	const FGameplayTag LightningDashTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.LightningDash"));
	const FGameplayTag InvulnerableToDamageTag = FGameplayTag::RequestGameplayTag(FName("State.Invulnerable.Damage"));
	AbilityTags.AddTag(LightningDashTag);
	ActivationOwnedTags.AddTag(LightningDashTag);
	ActivationOwnedTags.AddTag(InvulnerableToDamageTag);
}

void UBDTA_LightningDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("ActivateAbility LIGHTNING DASH"));

	check(DashMontage);
	check(Dash);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogGameplayAbility, Display, TEXT("Can't activate ability LIGHTNING DASH because of Cooldown or missing resources"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	ApplyCooldown(Handle, ActorInfo, ActivationInfo);


	Hero = Cast<ABDTCharacterBase>(GetAvatarActorFromActorInfo());
	if (!Hero)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	LightningDash();
}

void UBDTA_LightningDash::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}

void UBDTA_LightningDash::InitDash()
{
	Start = Hero->GetActorLocation();
	const FVector CharacterDirection = Hero->GetVelocity();
	MoveDirection = FVector2d(CharacterDirection.Y, CharacterDirection.X);
	MoveDirection.Normalize(); 
	
	MovementComponent = Cast<UBDTCharacterMovementComponent>(Hero->GetCharacterMovement());
	MovementComponent->bIgnoreClientMovementErrorChecksAndCorrection = true;
	MovementComponent->bServerAcceptClientAuthoritativePosition = true;
	MovementComponent->SetMovementMode(MOVE_Custom, static_cast<uint8>(ECustomMovementMode::Dashing));

	UGameplayStatics::PlaySoundAtLocation(this, TrailSoundToPlay, {0.f, 0.f, 0.f});
	if (!TrailEffectToSpawn->IsValid())
		return;

	UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(TrailEffectToSpawn, Hero->GetMesh(),
		EName::None, Start, FRotator(0.f, 0.f, 0.f),
		EAttachLocation::KeepWorldPosition, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandleToDestroyEffect,[SpawnedEffect]()
	{
		SpawnedEffect->DestroyComponent();
	}, DashTime + DelayToDestroyDashEffectAfterAbilityEnds, false);

	Hero->GetMesh()->SetVisibility(false);


	//end after dash time
	FTimerHandle TimerHandle_MaxDashTime;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MaxDashTime,[&]()
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_TickDash);
			EndDash();
		}, DashTime, false);}

void UBDTA_LightningDash::DashingTick_Implementation()
{
	const float DistanceToAdd = DashSpeed * DashTick;
	
	const FVector CurrentLocation = Hero->GetActorLocation();
	
	// We multiply the Y of the MoveVector with the X of the Location and vice-versa to have the correct vector
	const FVector NewLocation(CurrentLocation.X + DistanceToAdd * MoveDirection.Y, CurrentLocation.Y + DistanceToAdd * MoveDirection.X, CurrentLocation.Z);
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Hero);
	if (FHitResult HitResult; GetWorld()->LineTraceSingleByChannel(HitResult, CurrentLocation, NewLocation, ECC_WorldStatic, CollisionParams))
	{
		Hero->SetActorLocation({HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, HitResult.ImpactPoint.Z - 1.f}); // -1.f is to allow GetGroundPosition to work else its too high
		SetCharacterFeetPosition(GetGroundPosition());
		EndDash();
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_TickDash);
		return;
	}
	Hero->SetActorLocation(NewLocation);
	
	if (GetCharacterFeetPosition().Z > GetGroundPosition().Z)
		SetCharacterFeetPosition(GetGroundPosition());
}

bool UBDTA_LightningDash::DashingTick_Validate()
{
	return true;
}


void UBDTA_LightningDash::LightningDash()
{
	InitDash();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_TickDash,[&]()
		{
			DashingTick();
		}, DashTick, true);

}

void UBDTA_LightningDash::EndDash()
{
	const FVector GroundPosition = GetGroundPosition();
	const FRotator CharacterRotation = Hero->GetTransform().GetRotation().Rotator();

	const FActorSpawnParameters SpawnInfo;

	if (ZoneEffectToSpawn != nullptr && ZoneDecalsToSpawn != nullptr && ZoneSoundToPlay != nullptr)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ZoneEffectToSpawn, GroundPosition, CharacterRotation);

		GetWorld()->SpawnActor<AActor>(ZoneDecalsToSpawn, GroundPosition, CharacterRotation, SpawnInfo);
		UGameplayStatics::PlaySoundAtLocation(this, ZoneSoundToPlay, GroundPosition);

		Cast<ABDTHeroCharacter>(Hero)->bDashDamage = true;
	}
	MovementComponent->bIgnoreClientMovementErrorChecksAndCorrection = false;
	MovementComponent->bServerAcceptClientAuthoritativePosition = false;
	MovementComponent->SetMovementMode(MOVE_Falling);
	Hero->GetMesh()->SetVisibility(true);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}

FVector UBDTA_LightningDash::GetCharacterFeetPosition() const
{
	return Hero->GetActorLocation() - FVector(0.f, 0.f, Hero->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
}

FVector UBDTA_LightningDash::GetGroundPosition() const
{
	const FVector StartTrace = Hero->GetActorLocation();
	const FVector EndTrace = StartTrace - FVector(0.f, 0.f, Hero->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Hero);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_WorldStatic, CollisionParams))
		return HitResult.ImpactPoint;

	return StartTrace;
}

void UBDTA_LightningDash::SetCharacterFeetPosition(const FVector& NewPosition) const
{
	Hero->SetActorLocation(NewPosition + FVector(0.f, 0.f, Hero->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
}


void UBDTA_LightningDash::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("EventReceived LIGHTNING DASH"));

	 Hero = Cast<ABDTCharacterBase>(GetAvatarActorFromActorInfo());
	if (!Hero)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	LightningDash();
}

void UBDTA_LightningDash::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("OnCancelled LIGHTNING DASH"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UBDTA_LightningDash::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("OnCompleted LIGHTNING DASH"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}





