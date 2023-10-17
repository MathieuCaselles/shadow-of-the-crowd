// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/WeaponBase/BDTSwordSwipe.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABDTSwordSwipe::ABDTSwordSwipe()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpeedRotationPivot = 700.f;
	bReplicates = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	
	RootComponent = Root;
	Pivot->SetupAttachment(Root);
	BoxCollision->SetupAttachment(Pivot);

	Pivot->SetRelativeRotation(FRotator(0.0f, -20.0f, 0.0f));

	BoxCollision->SetBoxExtent(FVector(32.f, 32.f, 32.f));
	BoxCollision->SetRelativeLocation(FVector(0.f, -140.f, -40.f));
	BoxCollision->SetRelativeScale3D(FVector(1.5f, 4.5f, 1.75f));
	BoxCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	BoxCollision->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ABDTSwordSwipe::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() != ROLE_Authority || CharacterPointer == nullptr || !EffectToSpawn->IsValid())
	{
		Destroy();
		return;
	}

	CharacterPointer->SwordBoxCollision = BoxCollision;
	
	ReplicateEffect();
	
	CharacterPointer->bSwordDamage = true;
}

// Called every frame
void ABDTSwordSwipe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(GetLocalRole() != ROLE_Authority || CharacterPointer == nullptr || !EffectToSpawn->IsValid())
	{
		Destroy();
		return;
	}
	const float RotationToAdd = DeltaTime * SpeedRotationPivot + Pivot->GetRelativeRotation().Yaw;
	
	Pivot->SetRelativeRotation({0.f, RotationToAdd, 0.f});
	
	if(RotationToAdd < -20.f && RotationToAdd >= -160.f)
		Destroy();
}

void ABDTSwordSwipe::Destroyed()
{
	Super::Destroyed();
}

void ABDTSwordSwipe::ReplicateEffect_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, {0.f, 0.f, 0.f});

	const auto PivotWorldTransform = Pivot->GetComponentToWorld();
	const FVector PivotLocation = PivotWorldTransform.GetLocation();
	const FRotator PivotRotation = PivotWorldTransform.Rotator();
	
	const auto EffectSpawned =UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EffectToSpawn, PivotLocation, PivotRotation);
	EffectSpawned->AttachToComponent(Pivot, FAttachmentTransformRules::KeepWorldTransform);
}

bool ABDTSwordSwipe::ReplicateEffect_Validate()
{
	return true;
}
