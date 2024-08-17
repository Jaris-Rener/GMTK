// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

class IPropertyHandle;

// This class is used to customize the details panel for the CDCameraAxisData struct
class FCDCameraAxisDetails : public IPropertyTypeCustomization
{
public:
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
};

// This class is used to customize the details panel for the FCameraDynamicsRuntimeDefaultFloat struct
class FCDDataTypesDetails : public IPropertyTypeCustomization
{
public:

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
};
