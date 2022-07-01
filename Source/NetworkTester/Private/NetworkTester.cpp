// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetworkTester.h"
#include "NetworkTesterStyle.h"
#include "NetworkTesterCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

#include "ServerWidget.h"
#include "ClientWidget.h"


static const FName NetworkTesterTabName("NetworkTester");

#define LOCTEXT_NAMESPACE "FNetworkTesterModule"

void FNetworkTesterModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FNetworkTesterStyle::Initialize();
	FNetworkTesterStyle::ReloadTextures();

	FNetworkTesterCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FNetworkTesterCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FNetworkTesterModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FNetworkTesterModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(NetworkTesterTabName, FOnSpawnTab::CreateRaw(this, &FNetworkTesterModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FNetworkTesterTabTitle", "NetworkTester"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FNetworkTesterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FNetworkTesterStyle::Shutdown();

	FNetworkTesterCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(NetworkTesterTabName);
}

TSharedRef<SDockTab> FNetworkTesterModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	// Server View
	TSharedPtr<SServerWidget> ServerWidget = SNew(SServerWidget);
	TSharedPtr<SClientWidget> ClientWidget = SNew(SClientWidget);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SSplitter)
			.Orientation(Orient_Horizontal)
			+ SSplitter::Slot()
			.Value(0.5f)
			[
				ServerWidget.ToSharedRef()
			]
			+ SSplitter::Slot()
			.Value(0.5f)
			[
				ClientWidget.ToSharedRef()
			]
		];
}

void FNetworkTesterModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(NetworkTesterTabName);
}

void FNetworkTesterModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FNetworkTesterCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FNetworkTesterCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNetworkTesterModule, NetworkTester)