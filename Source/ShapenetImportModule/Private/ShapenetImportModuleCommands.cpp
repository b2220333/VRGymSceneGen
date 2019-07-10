#include "ShapenetImportModuleCommands.h"
#include "ShapenetImportModule.h"


#define LOCTEXT_NAMESPACE "ShapenetImportModule"

void ShapenetImportModuleCommands::RegisterCommands()
{
	UI_COMMAND(importShapenetAllCommand, "ShapenetImportModule", "Execute ShapenetImportModule action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE