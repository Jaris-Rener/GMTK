// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "CDStyle.h"

class FCameraDynamicsCommands : public TCommands<FCameraDynamicsCommands>
{
public:

	FCameraDynamicsCommands()
		: TCommands<FCameraDynamicsCommands>(TEXT("CameraDynamics"), NSLOCTEXT("Contexts", "CameraDynamics", "CameraDynamics Plugin"), NAME_None, FCameraDynamicsStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};