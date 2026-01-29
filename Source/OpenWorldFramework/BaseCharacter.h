// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "OpenWorldFrameworkCharacter.h"
#include "BaseCharacter.generated.h"

UCLASS()
class OPENWORLDFRAMEWORK_API ABaseCharacter : public AOpenWorldFrameworkCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Stores current surface for the animation blueprint
	UPROPERTY(BlueprintReadOnly, Category = "Physics")
	TEnumAsByte<EPhysicalSurface> CurrentSurface;

	UPROPERTY(EditAnywhere, Category = "Effects")
	class USurfaceEffectData* SurfaceDataAsset;

	// How fast feed rotate to match the slope (smoothness)
	UPROPERTY(EditAnywhere, Category = "IK")
	float IKSmoothSpeed = 15.0f;

	// The calculated tilt for each foot
	UPROPERTY(BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	FRotator LeftFootTilt;

	UPROPERTY(BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	FRotator RightFootTilt;

	// Actual logic function
	void UpdateFootIK(float DeltaTime);

	// Calculate the tilt from a hit normal
	FRotator CalculateRotationFromNormal(FVector Normal, FHitResult hit);

	FRotator GetFootRotation(FName SocketName);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Check's floor type by throwing raycast down
	UFUNCTION(BlueprintCallable, Category = "Physics")
	void CheckSurfaceType();
};
