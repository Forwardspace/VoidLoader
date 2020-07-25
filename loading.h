#pragma once
#include "bool.h"
#include <efi.h>
#include <efilib.h>

extern CHAR16* kernelFilename;
extern const void* kernelAddress;

extern EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE graphicsMode;

bool loadKernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);

void transferControl();