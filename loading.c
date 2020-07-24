#include "loading.h"

bool loadKernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
	EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
	EFI_GUID sfsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

	EFI_LOADED_IMAGE_PROTOCOL* lip = NULL;
	SystemTable->BootServices->OpenProtocol(ImageHandle, &lipGuid, &lip, ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);

	if (!lip) {
		Print(L"\nSorry, critical error: unable to find the Loaded Image Protocol (lip = NULL");
		return false;
	}

	EFI_HANDLE device = lip->DeviceHandle;
	
	//First get the protocol with which to open the root directory on the drive
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* sfs = NULL;
	SystemTable->BootServices->OpenProtocol(device, &sfsGuid, &sfs, ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);

	if (!sfs) {
		Print(L"\nSorry, critical error: unable to find the Simple File System on the boot drive (sfs = NULL");
	}

	//Now open the directory with the protocol
	EFI_FILE_HANDLE rootVolume = NULL;
	sfs->OpenVolume(sfs, &rootVolume);

	if (!rootVolume) {
		Print(L"\nSorry, critical error: unable to open the root volume of the boot drive (rootVolume = NULL");
	}

	EFI_FILE_HANDLE kernel = NULL;
	//rootVolume->Open(rootVolume, &kernel, );
}