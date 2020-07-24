#pragma once
#include "bool.h"
#include <efi.h>
#include <efilib.h>

//const CHAR16 kernelFilename = L"voidos.bin";

bool loadKernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);