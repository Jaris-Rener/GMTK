// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#include "CDCommands.h"

#define LOCTEXT_NAMESPACE "FAssetActionUtilitiesModule"

void FCameraDynamicsCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Camera Dynamics Editor", "Bring up the camera dynamics editor window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE