// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_Footstep.h"
#include "BaseCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (MeshComp)
	{
		ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
		if (Character)
		{
			Character->CheckSurfaceType();
		}
	}
}
