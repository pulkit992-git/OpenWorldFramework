// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "SurfaceEffectData.h" 
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
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

	// Slerp provides a smoother, gimbal-lock-free transition
	LeftFootTilt = FQuat::Slerp(LeftFootTilt, GetFootRotation("foot_l"), IKSmoothSpeed * DeltaTime);
	RightFootTilt = FQuat::Slerp(RightFootTilt, GetFootRotation("foot_r"), IKSmoothSpeed * DeltaTime);
}

FQuat ABaseCharacter::GetFootRotation(FName SocketName)
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

	return FQuat::Identity;
}

FQuat ABaseCharacter::CalculateRotationFromNormal(FVector Normal, FHitResult Hit)
{
	// We want the foot's UP (Z) to be the Ground Normal.
	// We want the foot's FORWARD (X) to stay pointing with the Actor.
	// MakeRotFromZX is the "Magic Node" that handles this perfectly.
	FRotator TargetRot = UKismetMathLibrary::MakeRotFromZX(Normal, GetActorForwardVector());

	// Convert to Quaternion to prevent Gimbal Lock
	FQuat TargetQuat = TargetRot.Quaternion();

	// SKELETON OFFSET: This is the 90-degree fix for your specific mesh.
	// We do this by multiplying two Quaternions. 
	// If the foot is "sideways," change the FVector to (1,0,0) or (0,0,1).
	FQuat BoneFix = FQuat(FVector(0, 1, 0), FMath::DegreesToRadians(90.0f));

	// Combine them (Order matters: World Rotation * Offset)
	return TargetQuat * BoneFix;
}

FRotator ABaseCharacter::GetFootWorldRotation(FRotator LocalTilt)
{
	// Combine the Actor's world orientation with your calculated tilt
	FQuat ActorQuat = GetActorQuat();
	FQuat TiltQuat = FQuat(LocalTilt);

	// This 'rotates' the local tilt by the actor's current world direction
	FQuat WorldQuat = ActorQuat * TiltQuat;

	return WorldQuat.Rotator();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateFootIK(DeltaTime);

	if (PhysicsHandle->GrabbedComponent)
	{
		//FVector TargetLocation = GetPawnViewLocation() + (GetViewRotation().Vector() * HoldDistance);
		//PhysicsHandle->SetTargetLocation(TargetLocation);

		// Get the location of the character's chest or eye
		FVector HolderLocation = GetActorLocation() + FVector(0, 0, 60.0f);

		// Target position, always in front of the character
		FVector TargetLocation = HolderLocation + (GetActorForwardVector() * HoldDistance);

		// Make the object float
		float BobbingSpeed = 2.0f;
		float BobbingAmount = 15.0f;
		float VerticalOffset = FMath::Sin(GetWorld()->GetRealTimeSeconds() * BobbingSpeed);

		TargetLocation.Z += (VerticalOffset * BobbingAmount);

		// Add vertical offset at eye level
		PhysicsHandle->SetTargetLocation(TargetLocation);
		// Update the rotation 
		PhysicsHandle->SetTargetRotation(GetActorRotation());
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Grabbing (Physics Handle)
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Started, this, &ABaseCharacter::Grab);
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Completed, this, &ABaseCharacter::Release);

		// Throw (Physics Handle)
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &ABaseCharacter::Throw);
	}
}

void ABaseCharacter::Grab()
{
	FHitResult Hit;
	FVector Start = GetPawnViewLocation();
	FVector End = Start + (GetViewRotation().Vector() * GrabDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_PhysicsBody, Params))
	{
		UPrimitiveComponent* ComponentToGrab = Hit.GetComponent();

		if (ComponentToGrab && ComponentToGrab->IsSimulatingPhysics())
		{
			GrabbedComponent = ComponentToGrab;

			PhysicsHandle->GrabComponentAtLocationWithRotation(
				ComponentToGrab,
				NAME_None,
				Hit.ImpactPoint,
				ComponentToGrab->GetComponentRotation()
			);

			// Grab sound 
			if (GrabSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, GrabSound, Hit.ImpactPoint);
			}
		}
	}
}


void ABaseCharacter::Throw()
{
	if (GrabbedComponent)
	{
		UPrimitiveComponent* TempComponent = GrabbedComponent;
		FVector LaunchLocation = TempComponent->GetComponentLocation();

		// Release it
		PhysicsHandle->ReleaseComponent();
		GrabbedComponent = nullptr;

		// throw in forward direction
		FVector LaunchDirection = GetViewRotation().Vector();
		TempComponent->AddImpulse(LaunchDirection * ThrowForce, NAME_None, true);

		// Sound effect for throwing
		if (ThrowSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ThrowSound, LaunchLocation);
		}

		// Particle effect
		if (LaunchParticle)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LaunchParticle, LaunchLocation);
		}
	}
}

void ABaseCharacter::Release()
{
	PhysicsHandle->ReleaseComponent();
}


