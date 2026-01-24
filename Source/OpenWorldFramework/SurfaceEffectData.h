// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SurfaceEffectData.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDFRAMEWORK_API USurfaceEffectData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY (EditAnywhere, Category = "Effects")
	TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> FootstepSounds;
	
	UPROPERTY(EditAnywhere, Category = "Effects")
	TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> FootstepParticles;
};
