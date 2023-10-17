#pragma once

#include "CoreMinimal.h"

inline float GetSignedAngleFromVectorAToVectorB(const FVector& A, const FVector& B)
{
	return FMath::RadiansToDegrees(FMath::Atan2(FVector::CrossProduct(A, B).Z, FVector::DotProduct(A, B)));
}
