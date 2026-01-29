// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "SurfaceEffectData.h" 
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

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
		//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 15.0f, 0, 1.0f);

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

	// Perform traces for both feet
	LeftFootTilt = FMath::RInterpTo(LeftFootTilt, GetFootRotation("foot_l"), DeltaTime, IKSmoothSpeed);
	RightFootTilt = FMath::RInterpTo(RightFootTilt, GetFootRotation("foot_r"), DeltaTime, IKSmoothSpeed);

		/*
	// For left foot
	FHitResult LeftHit;
	FVector L_Start = GetMesh()->GetSocketLocation("foot_l");
	FVector L_End = L_Start - FVector(0, 0, 50.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(LeftHit, L_Start, L_End, ECC_Visibility, QueryParams))
	{
		// Draw the Normal (Green line)
		DrawDebugLine(GetWorld(), LeftHit.ImpactPoint, LeftHit.ImpactPoint + (LeftHit.Normal * 50.0f), FColor::Green, false, -1.0f, 0, 2.0f);

		// Draw the Impact Point (Small Red Box)
		DrawDebugBox(GetWorld(), LeftHit.ImpactPoint, FVector(2, 2, 2), FColor::Red, false, -1.0f);

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
		RightFootTilt = FMath::RInterpTo(RightFootTilt, TargetRot, DeltaTime, IKSmoothSpeed);
		
	}*/
}

FRotator ABaseCharacter::GetFootRotation(FName SocketName)
{
	FHitResult Hit;
	FVector Start = GetMesh()->GetSocketLocation(SocketName);
	FVector End = Start - FVector(0, 0, 50.f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
	{
		// Draw the Normal (Green line)
		DrawDebugLine(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + (Hit.Normal * 50.0f), FColor::Green, false, -1.0f, 0, 2.0f);

		// Draw the Impact Point (Small Red Box)
		DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(2, 2, 2), FColor::Red, false, -1.0f);

		return CalculateRotationFromNormal(Hit.Normal, Hit);
	}

	return FRotator::ZeroRotator;
}

FRotator ABaseCharacter::CalculateRotationFromNormal(FVector Normal, FHitResult Hit)
{
	// Convert world normal into actor's local space
	FVector LocalNormal = ActorHasTag("Player") ? GetActorTransform().InverseTransformVectorNoScale(Normal) : Normal;

	// -----------------------------------------------------
	// Local normal to world normal 
	FVector WorldNormalFromLocal = GetActorTransform().TransformVectorNoScale(LocalNormal);

	// 3. Draw the Line
	DrawDebugLine(
		GetWorld(),
		Hit.ImpactPoint,
		Hit.ImpactPoint + (WorldNormalFromLocal * 50.f),
		FColor::Cyan, // Use a different color to distinguish from the World Normal
		false,
		-1.f,
		0,
		2.f
	);
	// ---------------------------------------------
	// -------------------------------------
	//Get the Actor's Right Vector
	FVector ActorRight = GetActorRightVector();

	//Calculate the Slope Forward Vector using Cross Product
	// Normal cross Right = Forward (Tangent to the surface)
	FVector SlopeForward = FVector::CrossProduct(ActorRight, Hit.Normal);

	//Draw the Debug Line (Blue)
	DrawDebugLine(
		GetWorld(),
		Hit.ImpactPoint,
		Hit.ImpactPoint + (SlopeForward * 50.f),
		FColor::Blue,
		false,
		-1.f,
		0,
		2.f
	);
	// ------------------------------------------------

	// Convert the surface normal vector into Pitch(Y) and Roll(X)
	float Pitch = FMath::RadiansToDegrees(FMath::Atan2(LocalNormal.X, LocalNormal.Z));
	float Roll = FMath::RadiansToDegrees(FMath::Atan2(LocalNormal.Y, LocalNormal.Z)) * -1.0f;

	//Return the rotator (Yaw should be 0 because we don't want the foot to spin)
	return FRotator(Roll, 0.0f, Pitch);
}


// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateFootIK(DeltaTime);
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

