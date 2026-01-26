// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "SurfaceEffectData.h" 
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::CheckSurfaceType()
{
	FHitResult Hit;
	FVector Start = GetActorLocation();
	
	// Trace down 150 units
	FVector End = Start - FVector(0, 0, 150.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bReturnPhysicalMaterial = true;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 15.0f, 0, 1.0f);

		// get the physical material from the hit result
		UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();
		if (PhysMat)
		{
			// Play sound on the basis of surface
			CurrentSurface = PhysMat->SurfaceType;

			if (SurfaceDataAsset)
			{
				// Try to find the specific sound for this surface
				USoundBase** SelectedSound = SurfaceDataAsset->FootstepSounds.Find(CurrentSurface);

				if (SelectedSound && *SelectedSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, *SelectedSound, Hit.ImpactPoint);
				}
				else
				{
					// FALLBACK: If we hit something with no material, play a "Default/Dirt" sound
					// This prevents the game from being silent.
					USoundBase** DefaultSound = SurfaceDataAsset->FootstepSounds.Find(SurfaceType_Default);
					if (DefaultSound)
					{
						UGameplayStatics::PlaySoundAtLocation(this, *DefaultSound, Hit.ImpactPoint);
					}
				}
			}
		}
	}
}

void ABaseCharacter::UpdateFootIK(float DeltaTime)
{
	// For left foot
	FHitResult LeftHit;
	FVector L_Start = GetMesh()->GetSocketLocation("foot_l");
	FVector L_End = L_Start - FVector(0, 0, 50.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(LeftHit, L_Start, L_End, ECC_Visibility, QueryParams))
	{
		// Calculate the target rotation base on the floor's angle (Normal)
		FRotator TargetRot = CalculateRotationFromNormal(LeftHit.Normal);

		// Smoothly interpolate to that rotation
		LeftFootTilt = FMath::RInterpTo(LeftFootTilt, TargetRot, DeltaTime, IKSmoothSpeed);
	}

	// For right foot
	FHitResult RightHit;
	FVector R_Start = GetMesh()->GetSocketLocation("foot_r");
	FVector R_End = R_Start - FVector(0, 0, 50.0f);

	if (GetWorld()->LineTraceSingleByChannel(RightHit, R_Start, R_End, ECC_Visibility, QueryParams))
	{
		// Calculate the target rotation base on the floor's angle (Normal)
		FRotator TargetRot = CalculateRotationFromNormal(RightHit.Normal);

		// Smoothly interpolate to that rotation
		RightFootTile = FMath::RInterpTo(RightFootTile, TargetRot, DeltaTime, IKSmoothSpeed);
	}
}

FRotator ABaseCharacter::CalculateRotationFromNormal(FVector Normal)
{
	// Convert the surface normal vector into Pitch(Y) and Roll(X)
	float Pitch = FMath::Atan2(Normal.Y, Normal.Z) * (180.0f/PI);
	float Roll = FMath::Atan2(Normal.X, Normal.Z) * (180.0f / PI) * -1.0f;

	return FRotator(Pitch, 0.0f, Roll);
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateFootIK (DeltaTime)
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

