// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickAssetAction.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "DebugHeader.h"

void UQuickAssetAction::DuplicateAssets(int32 NumOfDups)
{
	if (NumOfDups < 1)
	{
		ShowMessageDialog(EAppMsgType::Ok, TEXT("Please enter a VALID number"));
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 count = 0;

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		for (int32 i = 0; i < NumOfDups; ++i)
		{
			const FString SourceAssetPath = SelectedAssetData.ObjectPath.ToString();
			const FString NewDupAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i + 1);
			const FString NewPathName = FPaths::Combine(SelectedAssetData.PackagePath.ToString(), NewDupAssetName);

			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
			{
				UEditorAssetLibrary::SaveAsset(NewPathName, false);
				++count;
			}
		}
	}
	if (count > 0)
	{
		// Print(TEXT("Successfully duplicated " + FString::FromInt(count) + " files."), FColor::Green);
		ShowNotifyInfo(TEXT("Successfully duplicated " + FString::FromInt(count) + " files."));
	}
}

void UQuickAssetAction::AddPrefixes()
{
	TArray<UObject*> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 counter = 0;

	for (UObject* SelectedObject : SelectedObjects)
	{
		if (!SelectedObject)	continue;

		FString* PrefixFound = PrefixMap.Find(SelectedObject->GetClass());
		if (!PrefixFound || PrefixFound->IsEmpty())
		{
			Print(TEXT("Failed to find prefix for class: " + SelectedObject->GetClass()->GetName()), FColor::Red);
			continue;
		}

		FString OldName = SelectedObject->GetName();
		if (OldName.StartsWith(*PrefixFound))
		{
			Print(OldName + TEXT(" already has suitable prefix added."), FColor::Red);
			continue;
		}

		if (SelectedObject->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart(TEXT("M_"));
			OldName.RemoveFromEnd(TEXT("_Inst"));
		}

		const FString NewNameWithPrefix = *PrefixFound + OldName;
		UEditorUtilityLibrary::RenameAsset(SelectedObject, NewNameWithPrefix);
		++counter;
	}
	if (counter > 0)
	{
		ShowNotifyInfo(TEXT("Successfully renamed " + FString::FromInt(counter) + " assets"));
	}
}
