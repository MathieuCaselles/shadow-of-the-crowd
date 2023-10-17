// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/Characters/Hero/Abilities/BDTA_SummonFireBall.h"
#include "Abilities/AbilityTask/BDTAT_PlayMontageAndWaitForEvent.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "BaDumTss/Utils/EnhancedInputUtils.h"

UBDTA_SummonFireBall::UBDTA_SummonFireBall()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	const FGameplayTag SummonFireBallTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.SummonFireBall"));
	AbilityTags.AddTag(SummonFireBallTag);
	ActivationOwnedTags.AddTag(SummonFireBallTag);
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));
}

void UBDTA_SummonFireBall::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData * TriggerEventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("ActivateAbility SUMMON FIREBALL"));
	check(FireHipMontage);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogGameplayAbility, Display, TEXT("Can't activate ability SUMMON FIREBALL because of Cooldown or missing resources"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	ApplyCooldown(Handle, ActorInfo, ActivationInfo);

	UAnimMontage* MontageToPlay = FireHipMontage;
	
	// Play fire montage and wait for event telling us to spawn the projectile
	UBDTAT_PlayMontageAndWaitForEvent* Task = UBDTAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	Task->OnInterrupted.AddDynamic(this, &UBDTA_SummonFireBall::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UBDTA_SummonFireBall::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UBDTA_SummonFireBall::EventReceived);
	// ReadyForActivation() is how you activate the AbilityTask in C++. Blueprint has magic from K2Node_LatentGameplayTaskCall that will automatically call ReadyForActivation().
	Task->ReadyForActivation();
}

void UBDTA_SummonFireBall::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("END ABILITY SUMMON FIREBALL"));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBDTA_SummonFireBall::SpawnFireBall(ABDTHeroCharacter* Hero)
{

	FTransform MuzzleTransform = Hero->GetMesh()->GetSocketTransform(FName("Socket_FireBall"));
	const FVector Start = Hero->GetMesh()->GetSocketLocation("Socket_FireBall");
	const FVector End =  Hero->GetMesh()->GetSocketLocation("Socket_FireBall") + Hero->GetActorForwardVector();
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
	MuzzleTransform.SetRotation(Rotation.Quaternion());
	MuzzleTransform.SetScale3D(FVector(1.0f));

	const FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());

	ABDTProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABDTProjectile>(ProjectileClass, MuzzleTransform, GetOwningActorFromActorInfo(),
		Hero, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Projectile->CharacterPointer = Hero;
	Projectile->DamageEffectSpecHandle = DamageEffectSpecHandle;
	Projectile->FinishSpawning(MuzzleTransform);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}

void UBDTA_SummonFireBall::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("OnCancelled SUMMON FIREBALL"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UBDTA_SummonFireBall::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("EventReceived SUMMON FIREBALL"));

	// Only spawn projectiles on the Server.
	if (GetOwningActorFromActorInfo()->GetLocalRole() != ROLE_Authority || EventTag != FGameplayTag::RequestGameplayTag(FName("Event.Montage.SpawnFireball")))
	 return;

	
	ABDTHeroCharacter* Hero = Cast<ABDTHeroCharacter>(GetAvatarActorFromActorInfo());
	if (!Hero)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	SpawnFireBall(Hero);
}