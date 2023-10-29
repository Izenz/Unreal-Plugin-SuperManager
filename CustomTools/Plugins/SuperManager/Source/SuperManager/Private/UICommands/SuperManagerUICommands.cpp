#include "UICommands/SuperManagerUICommands.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerUICommands::RegisterCommands()
{
	UI_COMMAND (
		DisableActorSelection,
		"Disable Actor Selection",
		"Once triggered, selected actor can no longer be selected.",
		EUserInterfaceActionType::Button,
		FInputChord(EKeys::W, EModifierKey::Alt)
	);

	UI_COMMAND(
		EnableActorSelection,
		"Enable Actor Selection",
		"Once triggered, lifts selection restriction on all actors.",
		EUserInterfaceActionType::Button,
		FInputChord(EKeys::W, EModifierKey::Alt | EModifierKey::Shift)
	);
}

#undef LOCTEXT_NAMESPACE
