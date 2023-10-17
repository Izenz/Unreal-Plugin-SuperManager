// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvancedDeleteWidget.h"
#include "SlateBasics.h"
#include "DebugHeader.h"
#include "SuperManager.h"

void SAdvancedDeleteTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;
	SelectedFolderAssetsData = InArgs._AssetsDataToStore;
	FSlateFontInfo TitleTextFont = GetEmbossedTextFont();
	TitleTextFont.Size = 30;

	ChildSlot
		[
			// Main vertical box
			SNew(SVerticalBox)

			// First vertical slot for title
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Advanced Delete")))
				.Font(TitleTextFont)
				.Justification(ETextJustify::Center)
				.ColorAndOpacity(FColor::White)
			]

			// Second slot for drop down to specify listing filter
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
			]
			// Third slot for the asset list
			+ SVerticalBox::Slot()
				.VAlign(VAlign_Fill)
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
				[
					ConstructAssetListView()
				]
			]
			// Fourth slot for 3 buttons
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				// Button one
				+SHorizontalBox::Slot()
				.FillWidth(10.f)
				.Padding(5.f)
				[
					ConstructDeleteAllButton()
				]
				// Button two
				+SHorizontalBox::Slot()
				.FillWidth(10.f)
				.Padding(5.f)
				[
					ConstructSelectAllButton()
				]
				// Button three
				+SHorizontalBox::Slot()
				.FillWidth(10.f)
				.Padding(5.f)
				[
					ConstructDeselectAllButton()
				]
			]

		];
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvancedDeleteTab::ConstructAssetListView()
{
	AssetListView = SNew(SListView<TSharedPtr<FAssetData>>)
		.ItemHeight(24.f)
		.ListItemsSource(&SelectedFolderAssetsData)
		.OnGenerateRow(this, &SAdvancedDeleteTab::OnGenerateRowForList);

	return AssetListView.ToSharedRef();
}

void SAdvancedDeleteTab::RefreshAssetListView()
{
	if (AssetListView.IsValid())
	{
		AssetListView->RebuildList();
	}
}

#pragma region RowWidgetForAssetListView

TSharedRef<ITableRow> SAdvancedDeleteTab::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!AssetDataToDisplay->IsValid())	return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);
	
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();
	const FString DisplayAssetClassName = AssetDataToDisplay->AssetClass.ToString();

	FSlateFontInfo AssetClassNameFont = GetEmbossedTextFont();
	AssetClassNameFont.Size = 10;

	FSlateFontInfo AssetNameFont = GetEmbossedTextFont();
	AssetNameFont.Size = 15;

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget =
		SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable).Padding(FMargin(5.f))
		[
			SNew(SHorizontalBox)
			// First slot for checkbox
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.FillWidth(.1f)
			[
				ConstructCheckBox(AssetDataToDisplay)
			]
			// Second slot for displaying Asset Class Name
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.FillWidth(.5f)
			[
				ConstructTextForRowWidget(DisplayAssetClassName, AssetClassNameFont)
			]
			// Third slot for actual Asset name
			+ SHorizontalBox::Slot()
			[
				ConstructTextForRowWidget(DisplayAssetName, AssetNameFont)
			]
			// Forth slot for a button to delete directly
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Fill)
			[
				ConstructButtonForRowWidget(AssetDataToDisplay)
			]
		];

	return ListViewRowWidget;
}

TSharedRef<SCheckBox> SAdvancedDeleteTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> NewCheckBox = SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.OnCheckStateChanged(this, &SAdvancedDeleteTab::OnCheckBoxStateChanged, AssetDataToDisplay)
		.Visibility(EVisibility::Visible);

	return NewCheckBox;
}

void SAdvancedDeleteTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
	DebugHeader::Print(AssetData->AssetName.ToString() + TEXT(" is now unchecked"), FColor::Red);
		break;

	case ECheckBoxState::Checked:
		DebugHeader::Print(AssetData->AssetName.ToString() + TEXT(" is now checked"), FColor::Green);
		break;

	case ECheckBoxState::Undetermined:
		break;

	default:
		break;
	}
}

TSharedRef<STextBlock> SAdvancedDeleteTab::ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
	TSharedRef<STextBlock> NewTextBlock = SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(FontToUse)
		.ColorAndOpacity(FColor::White);

	return NewTextBlock;
}

TSharedRef<SButton> SAdvancedDeleteTab::ConstructButtonForRowWidget(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> NewButton = SNew(SButton)
		.Text(FText::FromString(TEXT("Delete")))
		.OnClicked(this, &SAdvancedDeleteTab::OnDeleteButtonClicked, AssetDataToDisplay)
		.ButtonColorAndOpacity(FColor::FromHex("#ff4d4d"));

	return NewButton;
}

FReply SAdvancedDeleteTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
	FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));
	const bool bAssetDeleted = SuperManagerModule.DeleteAssetFromList(*ClickedAssetData.Get());
	
	// Refresh list if delete is successful.
	if (bAssetDeleted)
	{
		// Update list source
		if (SelectedFolderAssetsData.Contains(ClickedAssetData))
		{
			SelectedFolderAssetsData.Remove(ClickedAssetData);
		}
		// Refresh list
		RefreshAssetListView();
	}

	return FReply::Handled();
}

#pragma endregion

TSharedRef<SButton> SAdvancedDeleteTab::ConstructDeleteAllButton()
{
	TSharedRef<SButton> NewButton = SNew(SButton)
		.ContentPadding(FMargin(5.f))
		.OnClicked(this, &SAdvancedDeleteTab::OnDeleteAllButtonClicked)
		.ButtonColorAndOpacity(FColor::FromHex("#ff4d4d"));

	NewButton->SetContent(ConstructTextForTabButtons(TEXT("Delete All")));

	return NewButton;
}

TSharedRef<SButton> SAdvancedDeleteTab::ConstructSelectAllButton()
{
	TSharedRef<SButton> NewButton = SNew(SButton)
		.Text(FText::FromString(TEXT("Select All")))
		.OnClicked(this, &SAdvancedDeleteTab::OnSelectAllButtonClicked);

	NewButton->SetContent(ConstructTextForTabButtons(TEXT("Select All")));

	return NewButton;
}

TSharedRef<SButton> SAdvancedDeleteTab::ConstructDeselectAllButton()
{
	TSharedRef<SButton> NewButton = SNew(SButton)
		.Text(FText::FromString(TEXT("Deselect All")))
		.OnClicked(this, &SAdvancedDeleteTab::OnDeselectAllButtonClicked);

	NewButton->SetContent(ConstructTextForTabButtons(TEXT("Deselect All")));

	return NewButton;
}

FReply SAdvancedDeleteTab::OnDeleteAllButtonClicked()
{
	return FReply::Handled();
}

FReply SAdvancedDeleteTab::OnSelectAllButtonClicked()
{
	return FReply::Handled();
}

FReply SAdvancedDeleteTab::OnDeselectAllButtonClicked()
{
	return FReply::Handled();
}

TSharedRef<STextBlock> SAdvancedDeleteTab::ConstructTextForTabButtons(const FString& TextContent)
{
	FSlateFontInfo TextFont = GetEmbossedTextFont();
	TextFont.Size = 15;


	TSharedRef<STextBlock> NewText = SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(TextFont)
		.Justification(ETextJustify::Center);

	return NewText;
}
