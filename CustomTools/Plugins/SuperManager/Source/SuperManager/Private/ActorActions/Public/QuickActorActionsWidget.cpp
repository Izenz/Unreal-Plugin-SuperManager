// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorActions/Public/QuickActorActionsWidget.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "DebugHeader.h"

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

bool UQuickActorActionsWidget::GetEditorActorSubsystem()
{
	if (!EditorActorSubsystem)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}

	return EditorActorSubsystem != nullptr;
}
