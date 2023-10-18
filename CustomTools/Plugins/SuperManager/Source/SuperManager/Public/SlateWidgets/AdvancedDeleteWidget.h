// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"

class SAdvancedDeleteTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvancedDeleteTab) {}
	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetsDataToStore)
	SLATE_ARGUMENT(FString, CurrentSelectedFolder)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:

	TArray<TSharedPtr<FAssetData>> SelectedFolderAssetsData;
	TArray<TSharedPtr<FAssetData>> DisplayedAssetsData;
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> AssetListView;
	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetListView();
	void RefreshAssetListView();

#pragma region ComboBoxForList

	TArray<TSharedPtr<FString>> ComboBoxSourceItems;

	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructComboBox();

	TSharedRef<SWidget> OnGenerateComboBoxContent(TSharedPtr<FString> SourceItem);

	void OnComboBoxSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo);

	TSharedPtr<STextBlock> ComboBoxDisplayTextBlock;

	TSharedRef<STextBlock> ConstructComboHelpTexts(const FString& TextContent, ETextJustify::Type TextJustify);

#pragma endregion

#pragma region RowWidgetForAssetListView

	TArray<TSharedRef<SCheckBox>> CheckBoxArray;

	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, 
		const TSharedRef<STableViewBase>& OwnerTable);

	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);
	TSharedRef<STextBlock> ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse);
	TSharedRef<SButton> ConstructButtonForRowWidget(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);
	void OnRowWidgetMouseButtonClick(TSharedPtr<FAssetData> ClickedData);

#pragma endregion

#pragma region TabButtons

	TSharedRef<SButton> ConstructDeleteAllButton();
	TSharedRef<SButton> ConstructSelectAllButton();
	TSharedRef<SButton> ConstructDeselectAllButton();

	FReply OnDeleteAllButtonClicked();
	FReply OnSelectAllButtonClicked();
	FReply OnDeselectAllButtonClicked();

	TSharedRef<STextBlock> ConstructTextForTabButtons(const FString& TextContent);

#pragma endregion

	TArray<TSharedPtr<FAssetData>> AssetsDataCheckedToDelete;
	bool DeleteAssetsFromList(const TArray<FAssetData>& AssetsToDelete);

	FSlateFontInfo GetEmbossedTextFont() const {
		return FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	};

};