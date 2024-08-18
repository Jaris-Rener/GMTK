// Copyright (c) 2024, Evelyn Schwab, Epic Games, Inc. All rights reserved.

#include "CameraDynamics.h"

#define LOCTEXT_NAMESPACE "FCameraDynamicsModule"

DEFINE_LOG_CATEGORY(LogCameraDynamics);

void FCameraDynamicsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FCameraDynamicsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCameraDynamicsModule, CameraDynamics)