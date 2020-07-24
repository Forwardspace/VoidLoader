#include "graphics.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE graphicsMode;

bool initGraphics(EFI_SYSTEM_TABLE* SystemTable) {
	EFI_GUID gopGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = NULL;
	SystemTable->BootServices->LocateProtocol(&gopGUID, NULL, &gop);

	if (!gop) {
		Print(L"\nSorry, critical error: unable to find the EFI Graphics Output Protocol (gop = NULL)");
		return false;
	}

	graphicsMode = *(gop->Mode);

	return true;
}