#include "ShapenetImportModuleCommands.h"
#include "ShapenetImportModule.h"


#define LOCTEXT_NAMESPACE "ShapenetImportModule"

void ShapenetImportModuleCommands::RegisterCommands()
{
	UI_COMMAND(importShapenetButton, "Import Models", "Import models from External/import.json", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE