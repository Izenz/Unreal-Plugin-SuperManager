// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSuperManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#pragma region ProcessDataForAdvancedDeleteTab

	bool DeleteAssetFromList(const FAssetData& AssetData);
	bool DeleteMultipleAssetsForAssetList(const TArray<FAssetData>& AssetsToDelete);

#pragma endregion

private:

#pragma region ContentBrowserMenuExtension

	void InitCBMenuExtension();

	TArray<FString> FolderPathsSelected;

	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);
	void AddCBMenuEntry(class FMenuBuilder& MenuBuilder);
	void OnDeleteUnusedAssetButtonClicked();
	void OnDeleteEmptyFoldersButtonClicked();
	void OnOpenAdvancedDeleteMenuButtonClicked();
	void FixUpRedirectors();
#pragma endregion

#pragma region CustomEditorTab
	void RegisterAdvancedDeleteTab();

	TSharedRef<SDockTab> OnSpawnAdvancedDelete(const FSpawnTabArgs& TabArgs);
	TArray<TSharedPtr<FAssetData>> GetAllAssetDataUnderSelectedFolder();
#pragma endregion
};
