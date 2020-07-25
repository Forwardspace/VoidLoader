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

	SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

	Print(L"\nVoidLoader, bringing you the best in VoidOS loading since 2020!\n\nInitializing...");
	

	if (!initGraphics(SystemTable)) {
		for (;;);
		return -1;
	}
	else {
		Print(L"\nGraphics OK");
	}

	if (!loadKernel(ImageHandle, SystemTable)) {
		for (;;);
		return -1;
	}
	else {
		Print(L"\nKernel OK");
	}

	Print(L"\n\nTransferring control to kernel...\n");

	SystemTable->BootServices->ExitBootServices(ImageHandle, 0);

	transferControl();

	return -1;	//We really shouldn't ever return back to the bootloader
}
