// Fill out your copyright notice in the Description page of Project Settings.


#include "UM_WorldSubsystem.h"

UUM_WorldSubsystem::UUM_WorldSubsystem()
{
	CursorPtr = nullptr;
}

UStaticMeshComponent* UUM_WorldSubsystem::GetCursorPtr() const
{
	return CursorPtr;
}

void UUM_WorldSubsystem::SetCursorPtr(UStaticMeshComponent* NewCursorPtr)
{
	if (NewCursorPtr == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("NewCursorPtr is null"));
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("NewCursorPtr world location: %s"), *NewCursorPtr->GetComponentLocation().ToString());
	}
	CursorPtr = NewCursorPtr;
}

void UUM_WorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// Initialize dependent subsystems before calling super
	Super::Initialize(Collection);

	// In here you can hook to delegates!
	// ie: OnFireHandle = FExample::OnFireDelegate.AddUObject(this, &UMyWorldSubsystem::OnFire);
}

void UUM_WorldSubsystem::Deinitialize()
{
	// In here you can unhook from delegates
	// ie: FExample::OnFireDelegate.Remove(OnFireHandle);
	Super::Deinitialize();
}