// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#include "CameraDynamicsEditor.h"
#include "CDCommands.h"
#include "CDDataTypesDetails.h"
#include "CDStyle.h"
#include "Editor.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "PropertyEditorDelegates.h"
#include "PropertyEditorModule.h"
#include "ToolMenus.h"
#include "Data/CameraDynamicDataTypes.h"

#define LOCTEXT_NAMESPACE "FCameraDynamicsEditorModule"



void FCameraDynamicsEditorModule::StartupModule()
{
    // Get a reference to the property module
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    /*
    Register the custom property type's layout. This requires the name of the property type. You can provide it manually as a string ("CustomData"), or you can get the FName from the StaticStruct.
    You also need to provide a delegate for the function that creates an instance of the details' customization. In this case, it's the MakeInstance function created earlier.
    */

    PropertyModule.RegisterCustomPropertyTypeLayout(FCDCameraAxisData::StaticStruct()->GetFName(),
                                                    FOnGetPropertyTypeCustomizationInstance::CreateStatic(
                                                        FCDCameraAxisDetails::MakeInstance));


    FCameraDynamicsStyle::Initialize();
    FCameraDynamicsStyle::ReloadTextures();

    FCameraDynamicsCommands::Register();

    
    PluginCommands = MakeShareable(new FUICommandList);

    PluginCommands->MapAction(
        FCameraDynamicsCommands::Get().OpenPluginWindow,
        FExecuteAction::CreateRaw(this, &FCameraDynamicsEditorModule::CameraDynamicsButtonClicked),
        FCanExecuteAction());
	
	
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCameraDynamicsEditorModule::RegisterMenus));
    
    
}

void FCameraDynamicsEditorModule::ShutdownModule()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.UnregisterCustomPropertyTypeLayout(FCDCameraAxisData::StaticStruct()->GetFName());

    UToolMenus::UnRegisterStartupCallback(this);

    UToolMenus::UnregisterOwner(this);

    FCameraDynamicsStyle::Shutdown();

    FCameraDynamicsCommands::Unregister();
    
}

void FCameraDynamicsEditorModule::CameraDynamicsButtonClicked()
{
    // Try load the editor utility widget
    UObject* UtilityWidgetBP = UEditorAssetLibrary::LoadAsset(UtilityWidgetReference);
	
    // Early return if the widget is invalid
    if (UtilityWidgetBP == nullptr) return;

    UEditorUtilityWidgetBlueprint* UtilityWidget = Cast<UEditorUtilityWidgetBlueprint>(UtilityWidgetBP);

    if (UtilityWidget == nullptr) return;
	
    // Get the editor utility subsystem
    UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();

    SpawnedUtilityWidget = EditorUtilitySubsystem->SpawnAndRegisterTab(UtilityWidget);
}

void FCameraDynamicsEditorModule::RegisterMenus()
{
    // Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
    FToolMenuOwnerScoped OwnerScoped(this);

    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
        {
            FToolMenuSection& Section = Menu->FindOrAddSection("Tools");
            Section.AddMenuEntryWithCommandList(FCameraDynamicsCommands::Get().OpenPluginWindow, PluginCommands);
        }
    }
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FCameraDynamicsEditorModule, CameraDynamicsEditor)