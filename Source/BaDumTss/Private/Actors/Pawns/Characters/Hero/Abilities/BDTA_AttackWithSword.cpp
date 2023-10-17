// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/Characters/Hero/Abilities/BDTA_AttackWithSword.h"
#include "Abilities/AbilityTask/BDTAT_PlayMontageAndWaitForEvent.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "Actors/WeaponBase/BDTSwordSwipe.h"
#include "Kismet/KismetMathLibrary.h"


UBDTA_AttackWithSword::UBDTA_AttackWithSword()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	const FGameplayTag AttackWithSwordTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.AttackWithSword"));
	AbilityTags.AddTag(AttackWithSwordTag);
	ActivationOwnedTags.AddTag(AttackWithSwordTag);

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));

}

void UBDTA_AttackWithSword::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("ActivateAbility ATTACK WITH SWORD"));

	check(SwipeMontage);
	check(SwordSwipe);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogGameplayAbility, Display, TEXT("Can't activate ability ATTACK WITH SWORD because of Cooldown or missing resources"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	ApplyCooldown(Handle, ActorInfo, ActivationInfo);

	UAnimMontage* MontageToPlay = SwipeMontage;

	// Play fire montage and wait for event telling us to spawn the projectile
	UBDTAT_PlayMontageAndWaitForEvent* Task = UBDTAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	Task->OnCompleted.AddDynamic(this, &UBDTA_AttackWithSword::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UBDTA_AttackWithSword::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UBDTA_AttackWithSword::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UBDTA_AttackWithSword::EventReceived);
	// ReadyForActivation() is how you activate the AbilityTask in C++. Blueprint has magic from K2Node_LatentGameplayTaskCall that will automatically call ReadyForActivation().
	Task->ReadyForActivation();
}

void UBDTA_AttackWithSword::AttackWithSword(ABDTHeroCharacter* Hero)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("LANCEMENT DE ATTACK WITH SWORD"));

	const FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());

	FTransform MuzzleTransform = Hero->GetMesh()->GetSocketTransform(FName("Socket_FireBall"));
	const FVector Start = Hero->GetMesh()->GetSocketLocation("Socket_FireBall");
	const FVector End =  Hero->GetMesh()->GetSocketLocation("Socket_FireBall") + Hero->GetActorForwardVector();
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
	MuzzleTransform.SetRotation(Rotation.Quaternion());
	MuzzleTransform.SetScale3D(FVector(1.0f));

	ABDTSwordSwipe* Sword = GetWorld()->SpawnActorDeferred<ABDTSwordSwipe>(SwordSwipe, MuzzleTransform, GetOwningActorFromActorInfo(),
		Hero, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Sword->CharacterPointer = Hero;
	Sword->DamageEffectSpecHandle = DamageEffectSpecHandle;
	Sword->AttachToActor(Hero, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Sword->FinishSpawning(MuzzleTransform);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UBDTA_AttackWithSword::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("OnCancelled ATTACK WITH SWORD"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UBDTA_AttackWithSword::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("OnCancelled ATTACK WITH SWORD"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UBDTA_AttackWithSword::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("EventReceived ATTACK WITH SWORD"));

	// Only spawn projectiles on the Server.
	if (GetOwningActorFromActorInfo()->GetLocalRole() != ROLE_Authority || EventTag != FGameplayTag::RequestGameplayTag(FName("Event.Montage.AttackWithSword")))
		return;

	
	ABDTHeroCharacter* Hero = Cast<ABDTHeroCharacter>(GetAvatarActorFromActorInfo());
	if (!Hero)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	AttackWithSword(Hero);
}
