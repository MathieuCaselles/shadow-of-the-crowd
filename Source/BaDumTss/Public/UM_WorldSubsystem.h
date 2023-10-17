// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UM_WorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class BADUMTSS_API UUM_WorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UUM_WorldSubsystem();
	UStaticMeshComponent* GetCursorPtr() const;
	void SetCursorPtr(UStaticMeshComponent* NewPos);

protected:
	// UWorldSubsystem begin interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
private:
	UPROPERTY()
	UStaticMeshComponent* CursorPtr;
};
