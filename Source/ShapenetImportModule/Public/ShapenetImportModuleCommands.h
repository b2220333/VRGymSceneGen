#pragma once


#include "SlateBasics.h"
#include "ShapenetImportModuleStyle.h"


class ShapenetImportModuleCommands : public TCommands<ShapenetImportModuleCommands>
{
public:

	ShapenetImportModuleCommands()
		: TCommands<ShapenetImportModuleCommands>(TEXT("ShapenetImportModule"), NSLOCTEXT("Contexts", "PLUGIN_NAME", "PLUGIN_NAME Plugin"), NAME_None, FName("ShapenetImportModuleStyle"))
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> importFromFileCommand;
	TSharedPtr<FUICommandInfo> importFromSynsetAndHashCommand;
	TSharedPtr<FUICommandInfo> importSynsetCommand;
	TSharedPtr<FUICommandInfo> importShapenetAllCommand;
	TSharedPtr<FUICommandInfo> setShapenetDirCommand;
};