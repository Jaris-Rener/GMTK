// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CDDeveloperSettings.generated.h"

/**
 * 
 */

UCLASS(Config = EditorPerProjectUserSettings, meta = (DisplayName = "Camera Dynamics Developer Settings"))
class CAMERADYNAMICSEDITOR_API UCDDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UCDDeveloperSettings();

	UFUNCTION(BlueprintCallable, Category = "Camera Dynamics|Developer Settings")
	static UCDDeveloperSettings* GetCameraDynamicsDeveloperSettings();

	UFUNCTION(BlueprintCallable, Category = "Camera Dynamics|Developer Settings")
	void SetActiveDataAsset(TSoftObjectPtr<UDataAsset> NewActiveAsset);

	UFUNCTION(BlueprintCallable, Category = "Camera Dynamics|Developer Settings")
	void SetDebugActive(bool bNewDebugActive);
	
	UPROPERTY(Config, BlueprintReadOnly, Category = "Camera Dynamics|Developer Settings")
	TSoftObjectPtr<UDataAsset> ActiveDataAsset;

	UPROPERTY(COnfig, BlueprintReadOnly, Category = "Camera Dynamics|Developer Settings")
	bool bIsDebugActive;
};
