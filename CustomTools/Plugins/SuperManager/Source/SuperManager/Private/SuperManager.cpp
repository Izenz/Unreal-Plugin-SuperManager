// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManager.h"
#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "SlateWidgets/AdvancedDeleteWidget.h"
#include "CustomStyle/SuperManagerStyle.h"
#include "LevelEditor.h"
#include "Engine/Selection.h"
#include "Subsystems/EditorActorSubsystem.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{
	FSuperManagerStyle::InitializeIcons();

	InitCBMenuExtension();
	RegisterAdvancedDeleteTab();
	InitLevelEditorExtension();
	InitCustomSelectionEvent();
}

void FSuperManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName("AdvancedDelete"));
	FSuperManagerStyle::ShutDown();
}

#pragma region ProcessDataForAdvancedDeleteTab

bool FSuperManagerModule::DeleteAssetFromList(const FAssetData& AssetData)
{
	TArray<FAssetData> AssetDataForDeletion;
	AssetDataForDeletion.Add(AssetData);

	if (ObjectTools::DeleteAssets(AssetDataForDeletion) > 0)
	{
		return true;
	}

	return false;
}

bool FSuperManagerModule::DeleteMultipleAssetsForAssetList(const TArray<FAssetData>& AssetsToDelete)
{
	if (ObjectTools::DeleteAssets(AssetsToDelete) > 0)
	{
		return true;
	}

	return false;
}

void FSuperManagerModule::ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsData, TArray<TSharedPtr<FAssetData>>& OutDataArray)
{
	OutDataArray.Empty();

	for (const TSharedPtr<FAssetData>& Data : AssetsData)
	{
		TArray<FString> AssetReferencers =
			UEditorAssetLibrary::FindPackageReferencersForAsset(Data->ObjectPath.ToString());

		if (AssetReferencers.Num() == 0)
		{
			OutDataArray.Add(Data);
		}
	}
}

void FSuperManagerModule::ListAssetsWithSameNameForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsData, TArray<TSharedPtr<FAssetData>>& OutDataArray)
{
	OutDataArray.Empty();
	TMultiMap<FString, TSharedPtr<FAssetData>> AssetsInfoMultiMap;

	for (const TSharedPtr<FAssetData>& Data : AssetsData)
	{
		AssetsInfoMultiMap.Emplace(Data->AssetName.ToString(), Data);
	}

	for (const TSharedPtr<FAssetData>& Data : AssetsData)
	{
		TArray<TSharedPtr<FAssetData>> OutAssetsData;
		AssetsInfoMultiMap.MultiFind(Data->AssetName.ToString(), OutAssetsData);

		if (OutAssetsData.Num() <= 1)	continue;

		for (const TSharedPtr<FAssetData>& EqualNameData : OutAssetsData)
		{
			if (EqualNameData.IsValid())
			{
				OutDataArray.AddUnique(EqualNameData);
			}
		}
	}

}

void FSuperManagerModule::SyncContentBrowserToAsset(const FString& AssetPath)
{
	TArray<FString> AssetPathArray;
	AssetPathArray.Add(AssetPath);
	UEditorAssetLibrary::SyncBrowserToObjects(AssetPathArray);
}

#pragma endregion

#pragma region ContentBrowserMenuExtension

void FSuperManagerModule::InitCBMenuExtension()
{
	FContentBrowserModule& ContentBrowserModule =
		FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserModuleMenuExtenders =
		ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	// Add our custom delegate to all the existing ones
	FContentBrowserMenuExtender_SelectedPaths CustomCBMenuDelegate;
	CustomCBMenuDelegate.BindRaw(this, &FSuperManagerModule::CustomCBMenuExtender);
	ContentBrowserModuleMenuExtenders.Add(CustomCBMenuDelegate);
	/*
		// You can create delegate and bind at once too

		ContentBrowserModuleMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::
		CreateRaw(this, &FSuperManagerModule::CustomCBMenuExtender));
	*/
}

TSharedRef<FExtender> FSuperManagerModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	// Second binding to specify menu entry details
	TSharedRef<FExtender> MenuExtender(new FExtender());
	if (SelectedPaths.Num() > 0)
	{
		MenuExtender->AddMenuExtension
		(
			FName("Delete"),				// Extension hook, position to insert
			EExtensionHook::After,			// Before or after the hook
			TSharedPtr<FUICommandList>(),	// Custom hot keys
			FMenuExtensionDelegate::CreateRaw(this, &FSuperManagerModule::AddCBMenuEntry)
		);

		FolderPathsSelected = SelectedPaths;
	}
	return MenuExtender;
}

void FSuperManagerModule::AddCBMenuEntry(class FMenuBuilder& MenuBuilder)
{
	// Third binding to specify tool logic
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Unused Assets")),							// Option title
		FText::FromString(TEXT("Safely Delete all unused assets under folder")),	// Tooltip
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.DeleteUnusedAssets"),																// Icon
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteUnusedAssetButtonClicked)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Empty Folders")),
		FText::FromString(TEXT("Recursively delete empty folders inside selected directory")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.DeleteEmptyFolders"),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteEmptyFoldersButtonClicked)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Open Advanced Delete Menu")),
		FText::FromString(TEXT("Opens Advanced Delete Menu Editor Tab")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.AdvanceDeletion"),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnOpenAdvancedDeleteMenuButtonClicked)
	);
}

void FSuperManagerModule::OnDeleteUnusedAssetButtonClicked()
{
	if (FolderPathsSelected.Num() > 1)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok, TEXT("You can only do this action upon one folder"));
		return;
	}

	TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);
	if (AssetsPathNames.Num() == 0)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok, TEXT("No assets found in selected folder"));
		return;
	}

	EAppReturnType::Type ConfirmResult = DebugHeader::ShowMessageDialog(EAppMsgType::YesNo, TEXT("A total of ") + FString::FromInt(AssetsPathNames.Num()) + TEXT(" found.\nWould you like to proceed?"), false);
	if (ConfirmResult == EAppReturnType::No)	return;

	FixUpRedirectors();

	TArray<FAssetData> UnusedAssetsDataArray;
	for (const FString& AssetPathName : AssetsPathNames)
	{
		// Avoid root folders
		if (
			AssetPathName.Contains(TEXT("Developers")) ||
			AssetPathName.Contains(TEXT("Collections")) ||
			AssetPathName.Contains(TEXT("__ExternalActors__")) ||
			AssetPathName.Contains(TEXT("__ExternalObjects__"))
			)
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))	continue;

		TArray<FString> AssetsReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);
		if (AssetsReferencers.Num() == 0)
		{
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsDataArray.Add(UnusedAssetData);
		}
	}

	if (UnusedAssetsDataArray.Num() > 0)
	{
		ObjectTools::DeleteAssets(UnusedAssetsDataArray);
		DebugHeader::ShowNotifyInfo(TEXT("Succesfully deleted ") + FString::FromInt(UnusedAssetsDataArray.Num()) +
			TEXT(" unused assets from ") + FolderPathsSelected[0]);

		EAppReturnType::Type ClearEmptyFoldersResult = DebugHeader::ShowMessageDialog(EAppMsgType::YesNo, TEXT("Would you like to find and clear empty folders under selected directory?"), false);
		if (ClearEmptyFoldersResult == EAppReturnType::Yes)	OnDeleteEmptyFoldersButtonClicked();
	}
	else
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok, TEXT("No unused asset found under selected folder"), false);
	}
}

void FSuperManagerModule::OnDeleteEmptyFoldersButtonClicked()
{
	FixUpRedirectors();

	TArray<FString> FolderPathsArray = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0], true, true);
	uint32 count = 0;
	FString EmptyFolderPaths;
	TArray<FString> EmptyFolderPathArray;

	for (const FString& FolderPath : FolderPathsArray)
	{
		// Avoid root folders
		if (
			FolderPath.Contains(TEXT("Developers")) ||
			FolderPath.Contains(TEXT("Collections")) ||
			FolderPath.Contains(TEXT("__ExternalActors__")) ||
			FolderPath.Contains(TEXT("__ExternalObjects__"))
			)
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesDirectoryExist(FolderPath))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath))
		{
			EmptyFolderPaths.Append(FolderPath);
			EmptyFolderPaths.Append(TEXT("\n"));

			EmptyFolderPathArray.Add(FolderPath);
		}
	}

	if (EmptyFolderPathArray.Num() == 0)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok, TEXT("No empty folder found under selected folder"), false);
		return;
	}

	EAppReturnType::Type ConfirmationResult = DebugHeader::ShowMessageDialog(
		EAppMsgType::OkCancel,
		TEXT("Empty folders found in:\n") + EmptyFolderPaths + TEXT("\nWould you like to delete all?"),
		false
	);

	if (ConfirmationResult == EAppReturnType::Cancel)	return;

	for (const FString& EmptyFolderPath : EmptyFolderPathArray)
	{
		UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath) ? ++count : DebugHeader::Print(TEXT("Failed to delete " + EmptyFolderPath), FColor::Red);
	}
	if (count > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Succesfully deleted " + FString::FromInt(count) + " empty folders."));
	}
}

void FSuperManagerModule::OnOpenAdvancedDeleteMenuButtonClicked()
{
	FixUpRedirectors();

	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvancedDelete"));
}

void FSuperManagerModule::FixUpRedirectors()
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

#pragma endregion

#pragma region LevelEditorMenuExtension

void FSuperManagerModule::InitLevelEditorExtension()
{
	FLevelEditorModule& LevelEditorModule =
		FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

	TArray<FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors>& LevelEditorMenuExtenders =
		LevelEditorModule.GetAllLevelViewportContextMenuExtenders();

	LevelEditorMenuExtenders.Add(FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateRaw(this, &FSuperManagerModule::CustomLevelEditorMenuExtender));
}

TSharedRef<FExtender> FSuperManagerModule::CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> UICommandList, const TArray<AActor*> SelectedActors)
{
	TSharedRef<FExtender> NewMenuExtender = MakeShareable(new FExtender());
	if (SelectedActors.Num() > 0)
	{
		NewMenuExtender->AddMenuExtension(
			FName("ActorOptions"),
			EExtensionHook::Before,
			UICommandList,
			FMenuExtensionDelegate::CreateRaw(this, &FSuperManagerModule::AddLevelEditorMenuEntry)
		);
	}

	return NewMenuExtender;
}

void FSuperManagerModule::AddLevelEditorMenuEntry(class FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Disable Actor Selection")),
		FText::FromString(TEXT("Prevents actor from being selected.")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "LevelEditor.DisableSelection"),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDisableActorSelectionPressed)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Enable All Actors Selection")),
		FText::FromString(TEXT("Lifts selection restriction on all actors.")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "LevelEditor.EnableSelection"),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnEnableActorSelectionPressed)
	);
}

void FSuperManagerModule::OnDisableActorSelectionPressed()
{
	if (!GetEditorActorSubsystem())	return;
	TArray<AActor*> SelectedActors = WeakEditorActorSubsystem->GetSelectedLevelActors();

	if (SelectedActors.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor selected."));
		return;
	}

	FString UserNotificationMsg = TEXT("Disabled selection for objects:");
	for (AActor* Actor : SelectedActors)
	{
		if (!Actor)	continue;
		DisableActorSelection(Actor);
		WeakEditorActorSubsystem->SetActorSelectionState(Actor, false);

		UserNotificationMsg.Append(TEXT("\n"));
		UserNotificationMsg.Append(Actor->GetActorLabel());
	}

	DebugHeader::ShowNotifyInfo(UserNotificationMsg);
}

void FSuperManagerModule::OnEnableActorSelectionPressed()
{
	if (!GetEditorActorSubsystem()) return;

	TArray<AActor*> AllActorsInLevel = WeakEditorActorSubsystem->GetAllLevelActors();
	TArray<AActor*> UnselectableActors;

	for (AActor* ActorInLevel : AllActorsInLevel)
	{
		if (!ActorInLevel) continue;

		if (IsActorSelectionLocked(ActorInLevel))
		{
			UnselectableActors.Add(ActorInLevel);
		}
	}

	if (UnselectableActors.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor with restricted selection found."));
	}

	FString UserNotificationMsg = TEXT("Lifted selection constraint for objects:");
	for (AActor* Actor : UnselectableActors)
	{
		EnableActorSelection(Actor);

		UserNotificationMsg.Append(TEXT("\n"));
		UserNotificationMsg.Append(Actor->GetActorLabel());
	}

	DebugHeader::ShowNotifyInfo(UserNotificationMsg);
}

void FSuperManagerModule::InitCustomSelectionEvent()
{
	USelection* UserSelection = GEditor->GetSelectedActors();
	UserSelection->SelectObjectEvent.AddRaw(this, &FSuperManagerModule::OnActorSelected);
}

void FSuperManagerModule::OnActorSelected(UObject* SelectedObject)
{
	if (!GetEditorActorSubsystem())	return;
	if (AActor* SelectedActor = Cast<AActor>(SelectedObject))
	{
		if (IsActorSelectionLocked(SelectedActor))
		{
			// Deselect actor immediately.
			WeakEditorActorSubsystem->SetActorSelectionState(SelectedActor, false);
		}
	}
}

void FSuperManagerModule::DisableActorSelection(AActor* Actor)
{
	if (!Actor)	return;
	if (!Actor->ActorHasTag(FName("Locked")))
	{
		Actor->Tags.Add(FName("Locked"));
	}
}

void FSuperManagerModule::EnableActorSelection(AActor* Actor)
{
	if (!Actor)	return;
	if (Actor->ActorHasTag(FName("Locked")))
	{
		Actor->Tags.Remove(FName("Locked"));
	}
}

bool FSuperManagerModule::IsActorSelectionLocked(AActor* Actor)
{
	if (!Actor)	return false;
	return Actor->ActorHasTag(FName("Locked"));
}

#pragma endregion

#pragma region CustomEditorTab

void FSuperManagerModule::RegisterAdvancedDeleteTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner
	(
		FName("AdvancedDelete"),
		FOnSpawnTab::CreateRaw(this, &FSuperManagerModule::OnSpawnAdvancedDelete))
		.SetDisplayName(FText::FromString(TEXT("Advanced Delete")))
		.SetIcon(FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.AdvanceDeletion"));
}

TSharedRef<SDockTab> FSuperManagerModule::OnSpawnAdvancedDelete(const FSpawnTabArgs& TabArgs)
{
	return SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SAdvancedDeleteTab)
				.AssetsDataToStore(GetAllAssetDataUnderSelectedFolder())
				.CurrentSelectedFolder(FolderPathsSelected[0])
		];
}

TArray<TSharedPtr<FAssetData>> FSuperManagerModule::GetAllAssetDataUnderSelectedFolder()
{
	TArray<TSharedPtr<FAssetData>> AvailableAssetsData;
	TArray<FString> AssetsPathNames;

	if (!FolderPathsSelected.IsEmpty())
	{
		AssetsPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);
	}

	for (const FString& AssetPathName : AssetsPathNames)
	{
		// Avoid root folders
		if (
			AssetPathName.Contains(TEXT("Developers")) ||
			AssetPathName.Contains(TEXT("Collections")) ||
			AssetPathName.Contains(TEXT("__ExternalActors__")) ||
			AssetPathName.Contains(TEXT("__ExternalObjects__"))
			)
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))	continue;

		const FAssetData Data = UEditorAssetLibrary::FindAssetData(AssetPathName);
		AvailableAssetsData.Add(MakeShared<FAssetData>(Data));
	}

	return AvailableAssetsData;
}

#pragma endregion

bool FSuperManagerModule::GetEditorActorSubsystem()
{
	if (!WeakEditorActorSubsystem.IsValid())
	{
		WeakEditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}

	return WeakEditorActorSubsystem.IsValid();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)