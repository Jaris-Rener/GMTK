// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#include "CDDataTypesDetails.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

void FCDDataTypesDetails::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
                                          IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FCDDataTypesDetails::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if (!PropertyHandle->IsValidHandle()) return;
	
	TSharedPtr<IPropertyHandle> DefaultValueProperty = PropertyHandle->GetChildHandle("DefaultValue");
	TSharedPtr<IPropertyHandle> CurrentValueProperty = PropertyHandle->GetChildHandle("CurrentValue");

	// Check all the properties are valid
	check(DefaultValueProperty.IsValid() && CurrentValueProperty.IsValid());
	
	ChildBuilder.AddCustomRow(FText::FromString("Runtime Default Float"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(PropertyHandle->GetPropertyDisplayName())
		.Font(CustomizationUtils.GetRegularFont())
	]
	.ValueContent()
	.HAlign(HAlign_Fill)
	[
		SNew(SWrapBox)
		.UseAllottedWidth(true)
		+SWrapBox::Slot()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			.AutoWidth()
			[
				SNew(SBox)
				.MinDesiredWidth(10.0f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Font(CustomizationUtils.GetRegularFont())
					.Text(FText::FromString(TEXT("Default")))
				]
			]
			+SHorizontalBox::Slot()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			.AutoWidth()
			[
				SNew(SBox)
				.MinDesiredWidth(30.0f)
				.VAlign(VAlign_Center)
				[
					DefaultValueProperty->CreatePropertyValueWidget()
				]
			]
		]
		+SWrapBox::Slot()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			.AutoWidth()
			[
				SNew(SBox)
				.MinDesiredWidth(10.0f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Font(CustomizationUtils.GetRegularFont())
					.Text(FText::FromString(TEXT("Current")))
				]
			]
			+SHorizontalBox::Slot()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			.AutoWidth()
			[
				 SNew(SBox)
				.MinDesiredWidth(30.0f)
				.VAlign(VAlign_Center)
				[
					CurrentValueProperty->CreatePropertyValueWidget()
				]
			]
		]
		+SWrapBox::Slot()
		[
			SNew(SBox)
			.MinDesiredWidth(20.0f)
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Apply")))
				.ToolTipText(FText::FromString(TEXT("Apply the default value to the current value")))
				.ButtonStyle(FAppStyle::Get(), "Button")
				.OnPressed_Lambda([DefaultValueProperty, CurrentValueProperty]()
				{
					float DefaultValue = 0.0f;
					DefaultValueProperty->GetValue(DefaultValue);
					CurrentValueProperty->SetValue(DefaultValue);
				})
			]
		]
	];
}

// ------------------------------------------------- Camera axis details -------------------------------------------------

TSharedRef<IPropertyTypeCustomization> FCDDataTypesDetails::MakeInstance()
{
	return MakeShareable(new FCDDataTypesDetails);
}

void FCDCameraAxisDetails::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
                                                                  FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	
}

void FCDCameraAxisDetails::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// Make sure the property handle is valid
	if (!PropertyHandle->IsValidHandle()) return;

	// Create pointers for the property handles of the struct properties
	TSharedPtr<IPropertyHandle> XYActiveProperty = PropertyHandle->GetChildHandle("bXYActive");
	TSharedPtr<IPropertyHandle> XYScaleProperty	= PropertyHandle->GetChildHandle("XYScale");
	TSharedPtr<IPropertyHandle> ZActiveProperty	= PropertyHandle->GetChildHandle("bZActive");
	TSharedPtr<IPropertyHandle> ZScaleProperty	= PropertyHandle->GetChildHandle("ZScale");

	// CHeck through properties to ensure they are all valid
	check(XYActiveProperty.IsValid() && XYScaleProperty.IsValid() && ZActiveProperty.IsValid() && ZScaleProperty.IsValid());
	
	ChildBuilder.AddCustomRow(FText::FromString("Axis Influence"))
	.NameContent()
    [
        SNew(STextBlock)
        .Text(PropertyHandle->GetPropertyDisplayName())
        .Font(CustomizationUtils.GetRegularFont())
    ]
	.ValueContent()
	.HAlign(HAlign_Fill)
	[
		SNew(SWrapBox)
		.UseAllottedWidth(true)
		+SWrapBox::Slot()
		[
			SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
            [
            	SNew(SBox)
            	.MinDesiredWidth(10.0f)
            	.VAlign(VAlign_Center)
            	[
            		SNew(STextBlock)
                    .Font(CustomizationUtils.GetRegularFont())
                    .Text(FText::FromString(TEXT("XY")))
            	]
            ]
            +SHorizontalBox::Slot()
			.Padding(0.0f, 5.0f)
            .AutoWidth()
            [
            	SNew(SBorder)
            	.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
            	.Content()
            	[
            		SNew(SHorizontalBox)
            		+SHorizontalBox::Slot()
            		.Padding(0.0f, 0.0f, 5.0f, 0.0f)
            		.AutoWidth()
            		[
            			XYActiveProperty->CreatePropertyValueWidget()
            		]
            		+SHorizontalBox::Slot()
            		.AutoWidth()
            		[
            			SNew(SBox)
            			.MinDesiredWidth(64.0f)
            			[
            				XYScaleProperty->CreatePropertyValueWidget()
            			]
            		]
            	]
            ]	
		]
		+SWrapBox::Slot()
		.Padding(5.0f, 0.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
            [
                SNew(SBox)
                .MinDesiredWidth(10.0f)
                .VAlign(VAlign_Center)
                [
                	SNew(STextBlock)
                	.Font(CustomizationUtils.GetRegularFont())
                	.Text(FText::FromString(TEXT("Z")))
                ]
            ]
            +SHorizontalBox::Slot()
			.Padding(0.0f, 5.0f)
            .AutoWidth()
            [
                SNew(SBorder)
                .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
                .Content()
                [
                	SNew(SHorizontalBox)
                	+SHorizontalBox::Slot()
                	.Padding(0.0f, 0.0f, 5.0f, 0.0f)
                	.AutoWidth()
                	[
                		ZActiveProperty->CreatePropertyValueWidget()
                	]
                	+SHorizontalBox::Slot()
                	[
                		SNew(SBox)
                		.MinDesiredWidth(64.0f)
                		[
                			ZScaleProperty->CreatePropertyValueWidget()
                		]
                	]
                ]
            ]
		]
	];
	
}

TSharedRef<IPropertyTypeCustomization> FCDCameraAxisDetails::MakeInstance()
{
	return MakeShareable(new FCDCameraAxisDetails);
}
