// Copyright Epic Games, Inc. All Rights Reserved.

#include "ServerWidget.h"
#include "Modules/ModuleManager.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Styling/CoreStyle.h"
#include "SlateOptMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/SNumericEntryBox.h"


#define LOCTEXT_NAMESPACE	"NetworkTester"


SServerWidget::SServerWidget()
	: IpAddress(TEXT("127.0.0.1"))
	, IpPort(7777)
{

}

SServerWidget::~SServerWidget()
{

}

void SServerWidget::Construct(const FArguments& InArgs)
{
	TSharedPtr<SVerticalBox> VerticalBox = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(10.0f, 0.0f, 0.f, 0.0f)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("NetworkTester_ListenIp", "Listen Ip:"))
			]
			+ SHorizontalBox::Slot()
			.Padding(5.0f, 0.0f, 0.f, 0.0f)
			.AutoWidth()
			[
				SNew(SEditableTextBox)
				.MinDesiredWidth(75.0f)
				.Text(this, &SServerWidget::GetIpAddressAsText)
				.OnTextCommitted(this, &SServerWidget::OnIpAddressTextCommitted)
			]
			+ SHorizontalBox::Slot()
			.Padding(10.0f, 0.0f, 0.f, 0.0f)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("NetworkTester_ListenPort", "Port:"))
			]
			+ SHorizontalBox::Slot()
			.Padding(5.0f, 0.0f, 0.f, 0.0f)
			.AutoWidth()
			[
				SNew(SNumericEntryBox<uint16>)
				.AllowSpin(false)
				.MinValue(0)
				.MaxValue(65535)
				.MinDesiredValueWidth(75)
				.Value(this, &SServerWidget::GetIpAddressPort)
				.OnValueCommitted(this, &SServerWidget::OnPortValueCommitted)
			]
			+ SHorizontalBox::Slot()
			.MaxWidth(80.f)
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.OnClicked(this, &SServerWidget::OnListenClicked)
				.Text(this, &SServerWidget::GetListenButtonText)
				.ToolTipText(LOCTEXT("NetworkTester_ListenOrClose", "start listen or close"))
			]
			+ SHorizontalBox::Slot()
			.MaxWidth(80.f)
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.OnClicked(this, &SServerWidget::OnTickModeClicked)
				.Text(this, &SServerWidget::GetTickModeButtonText)
				.ToolTipText(LOCTEXT("NetworkTester_TickMode", "tick mode switch"))
			]
			+ SHorizontalBox::Slot()
			.MaxWidth(80.f)
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.IsEnabled(this, &SServerWidget::GetIsTickStepEnabled)
				.OnClicked(this, &SServerWidget::OnTickStepClicked)
				.Text(LOCTEXT("NetworkTester_TickStep", "Step"))
				.ToolTipText(LOCTEXT("NetworkTester_TickStep", "tick once"))
			]
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SSplitter)
			.Orientation(Orient_Vertical)
			+ SSplitter::Slot()
			.Value(0.5f)
			[
				SNew(SMultiLineEditableTextBox)
				.IsReadOnly(true)
			]
			+ SSplitter::Slot()
			.Value(0.5f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.f)
				[
					SNew(SMultiLineEditableTextBox)
					.IsReadOnly(false)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					[
						SNew(SButton)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.IsEnabled(this, &SServerWidget::GetIsSendMessageEnabled)
						.OnClicked(this, &SServerWidget::OnSendMessageClicked)
						.Text(LOCTEXT("NetworkTester_SendMsg", "Send"))
						.ToolTipText(LOCTEXT("NetworkTester_SendMsgTips", "send message"))
					]
				]
			]
		]
	;

	// add to children
	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				VerticalBox.ToSharedRef()
			]
		]
	];
}

FText SServerWidget::GetIpAddressAsText() const
{
	return FText::FromString(IpAddress);
}

void SServerWidget::OnIpAddressTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo)
{
	IpAddress = NewText.ToString();
}

TOptional<uint16> SServerWidget::GetIpAddressPort() const 
{ 
	return IpPort; 
}

void SServerWidget::OnPortValueCommitted(uint16 InPort, ETextCommit::Type CommitInfo)
{
	IpPort = InPort;
}

FText SServerWidget::GetListenButtonText() const
{
	return LOCTEXT("NetworkTester_ListenStart", "Listen");
}

FReply SServerWidget::OnListenClicked()
{
	return FReply::Handled();
}

// tick mode button
FText SServerWidget::GetTickModeButtonText() const
{
	return LOCTEXT("NetworkTester_TickModeAuto", "Auto");
}

FReply SServerWidget::OnTickModeClicked()
{
	return FReply::Handled();
}

// tick step button
FReply SServerWidget::OnTickStepClicked()
{
	return FReply::Handled();
}

bool SServerWidget::GetIsTickStepEnabled() const
{
	return true;
}

// send message button
FReply SServerWidget::OnSendMessageClicked()
{
	return FReply::Handled();
}

bool SServerWidget::GetIsSendMessageEnabled() const
{
	return true;
}

#undef LOCTEXT_NAMESPACE
