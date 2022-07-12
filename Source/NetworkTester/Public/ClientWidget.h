// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "MinimalClient.h"


/* Implements the client operating window. */
class SClientWidget : public SCompoundWidget
{
public:
	/** Default constructor. */
	SClientWidget();

	/** Virtual destructor. */
	virtual ~SClientWidget();

	SLATE_BEGIN_ARGS(SClientWidget) {}
	SLATE_END_ARGS()

	/**
	* Constructs this widget.
	*/
	void Construct(const FArguments& InArgs);

protected:
	/** @return The String containing the value of the property as Text */
	FText GetIpAddressAsText() const;

	void OnIpAddressTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo);

	TOptional<uint16> GetIpAddressPort() const;

	void OnPortValueCommitted(uint16 InPort, ETextCommit::Type CommitInfo);

	// connect button
	FText GetConnectButtonText() const;

	FReply OnConnectClicked();

	// tick mode button
	FText GetTickModeButtonText() const;

	FReply OnTickModeClicked();

	// tick step button
	FReply OnTickStepClicked();

	bool GetIsTickStepEnabled() const;

	// send message button
	FReply OnSendMessageClicked();

	bool GetIsSendMessageEnabled() const;

protected:
	TSharedPtr<SMultiLineEditableTextBox>  SendBox;

	FString IpAddress;
	uint16	IpPort;

	bool IsConnected;
	UMinimalClient* MinimalClient;
};
