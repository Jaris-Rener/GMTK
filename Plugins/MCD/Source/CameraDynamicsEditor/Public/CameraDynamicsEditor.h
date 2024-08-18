// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UEditorUtilityWidget;

class FCameraDynamicsEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /** This function will be bound to Command (by default it will bring up plugin window) */
    void CameraDynamicsButtonClicked();
    
private:
    void RegisterMenus();

    FString UtilityWidgetReference = TEXT("EditorUtilityWidgetBlueprint'/CameraDynamics/Utility/EUW_CameraDynamicsEditor'");

    UEditorUtilityWidget* SpawnedUtilityWidget = nullptr;

    TSharedPtr<class FUICommandList> PluginCommands;
};