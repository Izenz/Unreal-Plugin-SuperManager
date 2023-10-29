// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomOutlinerColumn/OutlinerSelectionDisableColumn.h"
#include "CustomStyle/SuperManagerStyle.h"
#include "ActorTreeItem.h"
#include "SuperManager.h"

SHeaderRow::FColumn::FArguments FOutlinerSelectionDisableColumn::ConstructHeaderRowColumn()
{
	SHeaderRow::FColumn::FArguments NewHeaderRow =
		SHeaderRow::Column(GetColumnID())
		.FixedWidth(24.f)
		.HAlignHeader(HAlign_Center)
		.VAlignHeader(VAlign_Center)
		.HAlignCell(HAlign_Center)
		.VAlignCell(VAlign_Center)
		.DefaultTooltip(FText::FromString(TEXT("Actor Selection Lock - Press icon to disable actor selection")))
		[
			SNew(SImage)
				.ColorAndOpacity(FSlateColor::UseForeground())
				.Image(FSuperManagerStyle::GetSlateStyleSet()->GetBrush(FName("LevelEditor.DisableSelection")))
		];

	return NewHeaderRow;
}

const TSharedRef<SWidget> FOutlinerSelectionDisableColumn::ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem,
	const STableRow<FSceneOutlinerTreeItemPtr>& Row)
{
	FActorTreeItem* ActorTreeItem = TreeItem->CastTo<FActorTreeItem>();
	if (!ActorTreeItem || !ActorTreeItem->IsValid())	return SNullWidget::NullWidget;

	FSuperManagerModule& SuperManagerModule =
		FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));
	const bool bIsSelectionLocked = SuperManagerModule.IsActorSelectionLocked(ActorTreeItem->Actor.Get());

	const FCheckBoxStyle& ToggleButtonStyle =
		FSuperManagerStyle::GetSlateStyleSet()->GetWidgetStyle<FCheckBoxStyle>(FName("SceneOutliner.SelectionLock"));

	TSharedRef<SCheckBox> NewRowWidget = SNew(SCheckBox)
		.Visibility(EVisibility::Visible)
		.Type(ESlateCheckBoxType::ToggleButton)
		.Style(&ToggleButtonStyle)
		.HAlign(HAlign_Center)
		.IsChecked(bIsSelectionLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
		.OnCheckStateChanged(this, &FOutlinerSelectionDisableColumn::OnRowWidgetCheckStateChanged, ActorTreeItem->Actor);

	return NewRowWidget;
}

void FOutlinerSelectionDisableColumn::OnRowWidgetCheckStateChanged(ECheckBoxState NewState, TWeakObjectPtr<AActor> Actor)
{
	FSuperManagerModule& SuperManagerModule =
		FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		SuperManagerModule.ProcessSelectionLockForOutliner(Actor.Get(), false);
		break;
	case ECheckBoxState::Checked:
		SuperManagerModule.ProcessSelectionLockForOutliner(Actor.Get(), true);
		break;
	case ECheckBoxState::Undetermined:
		break;
	default:
		break;
	}
}
