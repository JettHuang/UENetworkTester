// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClientWidget.h"
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


SClientWidget::SClientWidget()
	: IpAddress(TEXT("127.0.0.1"))
	, IpPort(7777)
	, IsConnected(false)
{
	MinimalClient = NewObject<UMinimalClient>();
	MinimalClient->AddToRoot();

	DelegateHandle = MinimalClient->ReceiveMessageDel.AddRaw(this, &SClientWidget::OnReceiveMessage);
}

SClientWidget::~SClientWidget()
{
	if (MinimalClient) {
		MinimalClient->ReceiveMessageDel.Remove(DelegateHandle);
		MinimalClient->RemoveFromRoot();
		MinimalClient = NULL;
	}
}

void SClientWidget::Construct(const FArguments& InArgs)
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
				.Text(LOCTEXT("NetworkTester_ServerIp", "Connect Ip:"))
			]
			+ SHorizontalBox::Slot()
			.Padding(5.0f, 0.0f, 0.f, 0.0f)
			.AutoWidth()
			[
				SNew(SEditableTextBox)
				.MinDesiredWidth(75.0f)
				.Text(this, &SClientWidget::GetIpAddressAsText)
				.OnTextCommitted(this, &SClientWidget::OnIpAddressTextCommitted)
			]
			+ SHorizontalBox::Slot()
			.Padding(10.0f, 0.0f, 0.f, 0.0f)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("NetworkTester_ServerPort", "Port:"))
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
				.Value(this, &SClientWidget::GetIpAddressPort)
				.OnValueCommitted(this, &SClientWidget::OnPortValueCommitted)
			]
			+ SHorizontalBox::Slot()
			.MaxWidth(80.f)
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.OnClicked(this, &SClientWidget::OnConnectClicked)
				.Text(this, &SClientWidget::GetConnectButtonText)
				.ToolTipText(LOCTEXT("NetworkTester_ListenOrClose", "start listen or close"))
			]
			+ SHorizontalBox::Slot()
			.MaxWidth(80.f)
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.OnClicked(this, &SClientWidget::OnTickModeClicked)
				.Text(this, &SClientWidget::GetTickModeButtonText)
				.ToolTipText(LOCTEXT("NetworkTester_TickMode", "tick mode switch"))
			]
			+ SHorizontalBox::Slot()
			.MaxWidth(80.f)
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.IsEnabled(this, &SClientWidget::GetIsTickStepEnabled)
				.OnClicked(this, &SClientWidget::OnTickStepClicked)
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
				SAssignNew(HistoryBox, SMultiLineEditableTextBox)
				.IsReadOnly(false)
			]
			+ SSplitter::Slot()
			.Value(0.5f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.f)
				[
					SAssignNew(SendBox, SMultiLineEditableTextBox)
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
						.IsEnabled(this, &SClientWidget::GetIsSendMessageEnabled)
						.OnClicked(this, &SClientWidget::OnSendMessageClicked)
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

FText SClientWidget::GetIpAddressAsText() const
{
	return FText::FromString(IpAddress);
}

void SClientWidget::OnIpAddressTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo)
{
	IpAddress = NewText.ToString();
}

TOptional<uint16> SClientWidget::GetIpAddressPort() const
{
	return IpPort;
}

void SClientWidget::OnPortValueCommitted(uint16 InPort, ETextCommit::Type CommitInfo)
{
	IpPort = InPort;
}

FText SClientWidget::GetConnectButtonText() const
{
	return IsConnected ? LOCTEXT("NetworkTester_ConnectStop", "Disconnect") : LOCTEXT("NetworkTester_ConnectStart", "Connect");
}

FReply SClientWidget::OnConnectClicked()
{
	if (IsConnected) {
		MinimalClient->Cleanup();
		IsConnected = false;
	}
	else {
		MinimalClient->Connect(IpAddress, IpPort);
		IsConnected = true;
	}

	return FReply::Handled();
}

// tick mode button
FText SClientWidget::GetTickModeButtonText() const
{
	return LOCTEXT("NetworkTester_TickModeAuto", "Auto");
}

FReply SClientWidget::OnTickModeClicked()
{
	return FReply::Handled();
}

// tick step button
FReply SClientWidget::OnTickStepClicked()
{
	return FReply::Handled();
}

bool SClientWidget::GetIsTickStepEnabled() const
{
	return true;
}

// send message button
FReply SClientWidget::OnSendMessageClicked()
{
	FString Str;

	if (SendBox)
	{
		FText Text = SendBox->GetText();
		Str = Text.ToString();

		SendBox->SetText(FText());
	}
	if (MinimalClient && Str.Len() > 0)
	{
		MinimalClient->SendText(Str);
	}

	return FReply::Handled();
}

bool SClientWidget::GetIsSendMessageEnabled() const
{
	return true;
}

void SClientWidget::OnReceiveMessage(const FString& InText, class UNetConnection* InConnection)
{
	if (HistoryBox)
	{
		HistoryBox->InsertTextAtCursor(InText);
	}
}

#undef LOCTEXT_NAMESPACE
