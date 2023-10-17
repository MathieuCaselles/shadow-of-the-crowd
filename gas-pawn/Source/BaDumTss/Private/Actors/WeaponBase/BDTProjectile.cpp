// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/WeaponBase/BDTProjectile.h"

#include "Abilities/Components/BDTAbilitySystemComponent.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABDTProjectile::ABDTProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LifeSpan = 2.f;
	bReplicates = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
}

// Called when the game starts or when spawned
void ABDTProjectile::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() != ROLE_Authority || CharacterPointer == nullptr)
	{
		Destroy();
		return;
	}

	CharacterPointer->GetArrayProjectile().Add(this);

	SetLifeSpan(LifeSpan);
}

void ABDTProjectile::Destroyed()
{
		UE_LOG(LogActor, Display, TEXT("detruit"));
	Super::Destroyed();
	if(GetLocalRole() != ROLE_Authority || CharacterPointer == nullptr)
		return;

	CharacterPointer->GetArrayProjectile().Remove(this);
}