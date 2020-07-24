#include "bool.h"
#include <efi.h>
#include <efilib.h>

#include "graphics.h"
#include "loading.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
#if defined(_GNU_EFI)
	InitializeLib(ImageHandle, SystemTable);
#endif

	Print(L"\nVoidLoader, bringing you the best in VoidOS loading since 2020!\n\nInitializing...");
	

	if (!initGraphics(SystemTable)) {
		return -1;
	}

	if (!loadKernel(SystemTable, ImageHandle)) {
		return -1;
	}

	for (;;);

	return -1;	//We really shouldn't ever return back to the bootloader
}
