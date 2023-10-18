// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"

class SAdvancedDeleteTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvancedDeleteTab) {}
	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetsDataToStore)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:

	TArray<TSharedPtr<FAssetData>> SelectedFolderAssetsData;
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> AssetListView;
	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetListView();
	void RefreshAssetListView();

#pragma region RowWidgetForAssetListView

	TArray<TSharedRef<SCheckBox>> CheckBoxArray;

	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, 
		const TSharedRef<STableViewBase>& OwnerTable);

	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);
	TSharedRef<STextBlock> ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse);
	TSharedRef<SButton> ConstructButtonForRowWidget(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);

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