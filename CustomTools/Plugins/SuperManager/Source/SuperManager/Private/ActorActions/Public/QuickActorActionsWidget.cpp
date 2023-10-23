// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorActions/Public/QuickActorActionsWidget.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "DebugHeader.h"

#pragma region ActorBatchSelection

void UQuickActorActionsWidget::SelectAllActorsWithSimilarName()
{
	if (!GetEditorActorSubsystem())	return;

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 NumOfSelectedActors = 0;

	if (SelectedActors.Num() != 1)
	{
		DebugHeader::ShowNotifyInfo(TEXT("A single actor must be selected."));
		return;
	}

	FString SelectedActorLabel = SelectedActors[0]->GetActorLabel();
	const FString NameToSearch = SelectedActorLabel.LeftChop(4);

	TArray<AActor*> AllActorsInLevel = EditorActorSubsystem->GetAllLevelActors();
	for (AActor* ActorInLevel : AllActorsInLevel)
	{
		if (!ActorInLevel)	continue;

		if (ActorInLevel->GetActorLabel().Contains(NameToSearch, SearchCase))
		{
			EditorActorSubsystem->SetActorSelectionState(ActorInLevel, true);
			++NumOfSelectedActors;
		}
	}

	if (NumOfSelectedActors > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully selected ") + FString::FromInt(NumOfSelectedActors) +
			TEXT(" actors."));
	}
	else
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor with similar name found."));
	}
}

#pragma endregion

#pragma region ActorBatchDuplication

void UQuickActorActionsWidget::DuplicateActors()
{
	if (!GetEditorActorSubsystem())	return;

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 NumOfDuplicates = 0;

	if (SelectedActors.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor selected."));
		return;
	}

	if (NumOfWantedDuplicates <= 0 || OffsetDistance == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Wrong duplication details. Specify valid Quantity and Offset."));
	}
	
	for (AActor* Actor : SelectedActors)
	{
		if (!Actor)	continue;

		for (int32 i = 0; i < NumOfWantedDuplicates; ++i)
		{
			AActor* NewActor = EditorActorSubsystem->DuplicateActor(Actor, Actor->GetWorld());

			if (!NewActor)	continue;

			const float DuplicationOffsetDistance = (i + 1) * OffsetDistance;

			switch (DuplicationAxis)
			{
			case E_DuplicationAxis::EDA_XAxis:
				NewActor->AddActorWorldOffset(FVector(DuplicationOffsetDistance, 0.f, 0.f));
				break;
			case E_DuplicationAxis::EDA_YAxis:
				NewActor->AddActorWorldOffset(FVector(0.f, DuplicationOffsetDistance, 0.f));
				break;
			case E_DuplicationAxis::EDA_ZAxis:
				NewActor->AddActorWorldOffset(FVector(0.f, 0.f, DuplicationOffsetDistance));
				break;
			case E_DuplicationAxis::EDA_MAX:
				break;
			default:
				break;
			}

			EditorActorSubsystem->SetActorSelectionState(NewActor, true);
			++NumOfDuplicates;
		}
	}

	if (NumOfDuplicates > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully duplicated ") + FString::FromInt(NumOfDuplicates) +
			TEXT(" new actors."));
	}
}

#pragma endregion

#pragma region RandomizeActorTransform

void UQuickActorActionsWidget::RandomizeActorTransform()
{
	const bool bConditionNotSet = 
		!RandomActorRotation.bRandomizeYawRot &&
		!RandomActorRotation.bRandomizePitchRot &&
		!RandomActorRotation.bRandomizeRollRot;

	if (bConditionNotSet)	return;
	if (!GetEditorActorSubsystem())	return;

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 NumOfRotatedActors = 0;

	if (SelectedActors.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("At least one actor must be selected."));
		return;
	}

	for (AActor* Actor : SelectedActors)
	{
		if (!Actor)	continue;

		if (RandomActorRotation.bRandomizeYawRot)
		{
			const float RandomRotYawValue = FMath::RandRange(RandomActorRotation.YawRotMin, RandomActorRotation.YawRotMax);
			Actor->AddActorWorldRotation(FRotator(0.f, RandomRotYawValue, 0.f));
		}

		if (RandomActorRotation.bRandomizePitchRot)
		{
			const float RandomRotPitchValue = FMath::RandRange(RandomActorRotation.YawRotMin, RandomActorRotation.YawRotMax);
			Actor->AddActorWorldRotation(FRotator(RandomRotPitchValue, 0.f, 0.f));
		}

		if (RandomActorRotation.bRandomizeRollRot)
		{
			const float RandomRotRollValue = FMath::RandRange(RandomActorRotation.YawRotMin, RandomActorRotation.YawRotMax);
			Actor->AddActorWorldRotation(FRotator(0.f, 0.f, RandomRotRollValue));
		}

		const bool bShouldIncreaseCounter =
			RandomActorRotation.bRandomizeYawRot ||
			RandomActorRotation.bRandomizePitchRot ||
			RandomActorRotation.bRandomizeRollRot;

		if (bShouldIncreaseCounter)	++NumOfRotatedActors;
	}


	if (NumOfRotatedActors > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully rotated ") +
			FString::FromInt(NumOfRotatedActors) + TEXT(" actors."));
	}
}

#pragma endregion

bool UQuickActorActionsWidget::GetEditorActorSubsystem()
{
	if (!EditorActorSubsystem)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}

	return EditorActorSubsystem != nullptr;
}
