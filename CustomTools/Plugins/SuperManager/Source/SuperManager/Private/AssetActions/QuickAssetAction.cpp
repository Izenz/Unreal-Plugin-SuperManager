// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickAssetAction.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "DebugHeader.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"

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

void UQuickAssetAction::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssetsData;

	FixUpRedirectors();

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.ObjectPath.ToString());
		if (AssetReferencers.Num() == 0)
		{
			UnusedAssetsData.Add(SelectedAssetData);
		}
	}

	if (UnusedAssetsData.Num() == 0)
	{
		ShowMessageDialog(EAppMsgType::Ok, TEXT("No unused asset found among selected assets."), false);
		return;
	}

	int32 numOfAssetsDeleted = ObjectTools::DeleteAssets(UnusedAssetsData);
	if (numOfAssetsDeleted == 0)	return;
	ShowNotifyInfo(TEXT("Successfully deleted ") + FString::FromInt(numOfAssetsDeleted) + TEXT(" unused assets."));
}

void UQuickAssetAction::RenameSelectedAssets(const FString& NewName)
{
	if (NewName.IsEmpty())
	{
		ShowMessageDialog(EAppMsgType::Ok, TEXT("Please enter a VALID name"));
		return;
	}

	TArray<UObject*> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 counter = 0;

	for (UObject* SelectedObject : SelectedObjects)
	{
		if (!SelectedObject)	continue;

		const FString NewNameWithSuffix = NewName + TEXT("_" + FString::FromInt(counter + 1));
		UEditorUtilityLibrary::RenameAsset(SelectedObject, NewNameWithSuffix);
		++counter;
	}

	if (counter > 0)
	{
		ShowNotifyInfo(TEXT("Successfully renamed " + FString::FromInt(counter) + " assets"));
	}
}

void UQuickAssetAction::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFixArray;
	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassNames.Emplace("ObjectRedirector");

	TArray<FAssetData> OutRedirectors;

	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors);

	for (const FAssetData& RedirectorData : OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}
	}

	FAssetToolsModule& AssetToolsModule =
	FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	
	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}
