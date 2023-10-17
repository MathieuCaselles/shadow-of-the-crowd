// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BDTGameState.generated.h"

USTRUCT()
struct FShortVector2D
{
	GENERATED_BODY()
	
	UPROPERTY()
	int16 X = 0;

	UPROPERTY()
	int16 Y = 0;
};

/**
 * 
 */
UCLASS()
class BADUMTSS_API ABDTGameState : public AGameStateBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rats", meta = (AllowPrivateAccess = "true"))
	int MaxRatNumber;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BBEG", meta = (AllowPrivateAccess = "true"))
	float BBEGMapOffset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rats", meta = (AllowPrivateAccess = "true"))
	float SpawnHeightRats;

	UPROPERTY(Replicated)
	FVector2f BadumCursorPosition;

	UPROPERTY(Replicated)
	bool BadumIsSpawningRats;
	
	
public:
	ABDTGameState();

	UFUNCTION(Category = "Rats")
	const TArray<FShortVector2D>& GetRatsPositions() const;

	UFUNCTION(Category = "Rats")
	const TArray<FVector2f>& GetSpawnerLocation() const;

	float GetMapOffSet() const;

	float GetSpawnHeightRats() const;
	
	void SetRatPosition(size_t Index, FVector2f const& NewPosition);

	void AddSpawnerPosition(FVector2f const& NewPosition);
	void RemoveSpawnerPosition(size_t Index);
	
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void SetBadumCursorPosition(FVector2f const& NewPosition);

	void SetBadumIsSpawningRats(bool NewValue);

	FVector2f GetBadumCursorPosition() const;
	
	bool GetBadumIsSpawningRats() const;

private:

	FShortVector2D CompressRatPosition(const FVector2f& RatPosition)
	{
		return {static_cast<int16>(RatPosition.X), static_cast<int16>(RatPosition.Y)};
	}

	UPROPERTY(Replicated)
	TArray<FShortVector2D> RatsPositions;

	UPROPERTY(Replicated)
    TArray<FVector2f> SpawnerPositions;
};
