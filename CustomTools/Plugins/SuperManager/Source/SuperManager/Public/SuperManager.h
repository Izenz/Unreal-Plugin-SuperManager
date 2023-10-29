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
	void ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsData, 
		TArray<TSharedPtr<FAssetData>>& OutDataArray);
	void ListAssetsWithSameNameForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsData,
		TArray<TSharedPtr<FAssetData>>& OutDataArray);
	void SyncContentBrowserToAsset(const FString& AssetPath);

#pragma endregion

	bool IsActorSelectionLocked(AActor* Actor);
	void ProcessSelectionLockForOutliner(AActor* Actor, bool bShouldDisable);

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

#pragma region LevelEditorMenuExtension

	void InitLevelEditorExtension();
	TSharedRef<FExtender> CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> UICommandList, const TArray<AActor*> SelectedActors);
	void AddLevelEditorMenuEntry(class FMenuBuilder& MenuBuilder);
	void OnDisableActorSelectionPressed();
	void OnEnableActorSelectionPressed();

	void InitCustomSelectionEvent();
	void OnActorSelected(UObject* SelectedObject);

	void DisableActorSelection(AActor* Actor);
	void EnableActorSelection(AActor* Actor);

#pragma endregion

#pragma region CustomEditorUICommands

	TSharedPtr<class FUICommandList> CustomUICommands;
	void InitCustomUICommands();
	void OnDisableActorSelectionHotkeyPressed();
	void OnEnableActorSelectionHotkeyPressed();

#pragma endregion

#pragma region SceneOutlinerExtension

	void InitSceneOutlinerColumnExtension();
	TSharedRef<class ISceneOutlinerColumn> OnCreateSelectionLockColumn(class ISceneOutliner& SceneOutliner);

#pragma endregion

	TWeakObjectPtr<class UEditorActorSubsystem> WeakEditorActorSubsystem;
	bool GetEditorActorSubsystem();
	void RefreshSceneOutliner();
	void UnRegisterSceneOutlinerColumnExtension();

#pragma region CustomEditorTab
	void RegisterAdvancedDeleteTab();

	TSharedRef<SDockTab> OnSpawnAdvancedDelete(const FSpawnTabArgs& TabArgs);
	TArray<TSharedPtr<FAssetData>> GetAllAssetDataUnderSelectedFolder();
#pragma endregion
};
