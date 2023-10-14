// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickAssetAction.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "DebugHeader.h"

void UQuickAssetAction::DuplicateAssets(int32 NumOfDups)
{
	if (NumOfDups < 1)
	{
		Print(TEXT("Please enter a VALID number"), FColor::Red);
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 count = 0;

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		for (int32 i = 0; i < NumOfDups; ++i)
		{
			const FString SourceAssetPath = SelectedAssetData.ObjectPath.ToString();
			const FString NewDupAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i+1);
			const FString NewPathName = FPaths::Combine(SelectedAssetData.PackagePath.ToString(), NewDupAssetName);

			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
			{
				UEditorAssetLibrary::SaveAsset(NewPathName, false);
				++count;
			}
		}
	}

	Print(TEXT("Successfully duplicated " + FString::FromInt(count) + " files."), FColor::Green);
}
