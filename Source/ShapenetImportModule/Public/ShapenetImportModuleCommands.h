#pragma once


#include "SlateBasics.h"
#include "ShapenetImportModuleStyle.h"


class ShapenetImportModuleCommands : public TCommands<ShapenetImportModuleCommands>
{
public:

	ShapenetImportModuleCommands()
		: TCommands<ShapenetImportModuleCommands>(TEXT("ShapenetImportModule"), NSLOCTEXT("Contexts", "ShapenetImportModule", "ShapenetImportModule Plugin"), NAME_None, FName("ShapenetImportModuleStyle"))
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> importShapenetButton;
};