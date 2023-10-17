// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pawns/Characters/BDTCharacterMovementComponent.h"

#include "Actors/Pawns/Characters/BDTCharacterBase.h"

UBDTCharacterMovementComponent::UBDTCharacterMovementComponent()
{
}

float UBDTCharacterMovementComponent::GetMaxSpeed() const
{
	ABDTCharacterBase* Owner = Cast<ABDTCharacterBase>(GetOwner());
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() No Owner"), *FString(__FUNCTION__));
		return Super::GetMaxSpeed();
	}
	return Owner->GetMoveSpeed();
}

void UBDTCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	//The Flags parameter contains the compressed input flags that are stored in the saved move.
	//UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
	//It basically just resets the movement component to the state when the move was made so it can simulate from there.
}



void UBDTCharacterMovementComponent::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);


}

void UBDTCharacterMovementComponent::FGDSavedMove::Clear()
{
	Super::Clear();
}

uint8 UBDTCharacterMovementComponent::FGDSavedMove::GetCompressedFlags() const
{
	return Super::GetCompressedFlags();
}

bool UBDTCharacterMovementComponent::FGDSavedMove::CanCombineWith(const FSavedMovePtr & NewMove, ACharacter * Character, float MaxDelta) const
{
	//Set which moves can be combined together. This will depend on the bit flags that are used.
	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void UBDTCharacterMovementComponent::FGDSavedMove::SetMoveFor(ACharacter * Character, float InDeltaTime, FVector const & NewAccel, FNetworkPredictionData_Client_Character & ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);
}

void UBDTCharacterMovementComponent::FGDSavedMove::PrepMoveFor(ACharacter * Character)
{
	Super::PrepMoveFor(Character);
}

UBDTCharacterMovementComponent::FGDNetworkPredictionData_Client::FGDNetworkPredictionData_Client(const UCharacterMovementComponent & ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UBDTCharacterMovementComponent::FGDNetworkPredictionData_Client::AllocateNewMove()
{
	return MakeShared<FGDSavedMove>();
}