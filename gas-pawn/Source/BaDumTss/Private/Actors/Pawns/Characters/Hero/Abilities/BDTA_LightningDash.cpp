// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/Characters/Hero/Abilities/BDTA_LightningDash.h"

#include "Abilities/AbilityTask/BDTAT_PlayMontageAndWaitForEvent.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "Actors/WeaponBase/BDTDash.h"
#include "Kismet/KismetMathLibrary.h"

UBDTA_LightningDash::UBDTA_LightningDash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	const FGameplayTag LightningDashTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.LightningDash"));
	AbilityTags.AddTag(LightningDashTag);
	ActivationOwnedTags.AddTag(LightningDashTag);
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



	UAnimMontage* MontageToPlay = DashMontage;

	// Play fire montage and wait for event telling us to spawn the projectile
	UBDTAT_PlayMontageAndWaitForEvent* Task = UBDTAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	Task->OnCompleted.AddDynamic(this, &UBDTA_LightningDash::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UBDTA_LightningDash::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UBDTA_LightningDash::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UBDTA_LightningDash::EventReceived);
	// ReadyForActivation() is how you activate the AbilityTask in C++. Blueprint has magic from K2Node_LatentGameplayTaskCall that will automatically call ReadyForActivation().
	Task->ReadyForActivation();
}

void UBDTA_LightningDash::LightningDash(ABDTHeroCharacter* Hero)
{
	FTransform MuzzleTransform = Hero->GetMesh()->GetSocketTransform(FName("Socket_FireBall"));
	const FVector Start = Hero->GetMesh()->GetSocketLocation("Socket_FireBall");
	const FVector End =  Hero->GetMesh()->GetSocketLocation("Socket_FireBall") + Hero->GetActorForwardVector();
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
	MuzzleTransform.SetRotation(Rotation.Quaternion());
	MuzzleTransform.SetScale3D(FVector(1.0f));

	
	UE_LOG(LogGameplayAbility, Display, TEXT("LANCEMENT DU LIGHTNING DASH"));
	ABDTDash* DashBP = GetWorld()->SpawnActorDeferred<ABDTDash>(Dash, MuzzleTransform, GetOwningActorFromActorInfo(),
		Hero, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	DashBP->CharacterPointer = Hero;
	DashBP->AttachToActor(Hero, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	DashBP->FinishSpawning(MuzzleTransform);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UBDTA_LightningDash::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogGameplayAbility, Display, TEXT("EventReceived LIGHTNING DASH"));

	// Only spawn projectiles on the Server.
	if (GetOwningActorFromActorInfo()->GetLocalRole() != ROLE_Authority || EventTag != FGameplayTag::RequestGameplayTag(FName("Event.Montage.LightningDash")))
		return;

	
	ABDTHeroCharacter* Hero = Cast<ABDTHeroCharacter>(GetAvatarActorFromActorInfo());
	if (!Hero)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	LightningDash(Hero);
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





