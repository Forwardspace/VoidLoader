#include "loading.h"

CHAR16* kernelFilename = L"voidos.bin";
const void* kernelAddress = (const void*)0xC0000000;

bool loadKernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
	EFI_LOADED_IMAGE_PROTOCOL* lip = NULL;
	SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, &lip);

	if (!lip) {
		Print(L"\nSorry, critical error: unable to find the Loaded Image Protocol (lip = NULL");
		return false;
	}

	EFI_HANDLE device = lip->DeviceHandle;
	
	//First get the protocol with which to open the root directory on the drive
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* sfs = NULL;
	SystemTable->BootServices->HandleProtocol(device, &gEfiSimpleFileSystemProtocolGuid, &sfs);

	if (!sfs) {
		Print(L"\nSorry, critical error: unable to find the Simple File System on the boot drive (sfs = NULL");
		return false;
	}

	//Now open the directory with the protocol
	EFI_FILE_HANDLE rootVolume = NULL;
	sfs->OpenVolume(sfs, &rootVolume);

	if (!rootVolume) {
		Print(L"\nSorry, critical error: unable to open the root volume of the boot drive (rootVolume = NULL");
		return false;
	}

	EFI_FILE_HANDLE kernel = NULL;
	rootVolume->Open(rootVolume, &kernel, kernelFilename, EFI_FILE_MODE_READ, NULL);

	if (!kernel) {
		Print(L"\nSorry, critical error: unable to open the VoidOS kernel file (kernel = NULL)");
		return false;
	}

	EFI_FILE_INFO fInfo;
	UINTN size = 2048;	//Note: sizeof(fInfo) does not work, but we know there is enough
						//space to store the info, so set some arbitrarily large number as
						//the size to make it work
	
	if (kernel->GetInfo(kernel, &gEfiFileInfoGuid, &size, &fInfo) != EFI_SUCCESS) {
		Print(L"\nSorry, critical error: invalid file info struct (kernel->GetInfo(...) != EFI_SUCCESS)");
		return false;
	}

	if (fInfo.FileSize == 0) {
		Print(L"\nSorry, critical error: invalid kernel file size (fInfo.FileSize = 0)");
		return false;
	}

	//Actually read the kernel
	//Hope nothing is at this address
	kernel->Read(kernel, fInfo.FileSize + 1, kernelAddress);

	return true;
}

void transferControl() {
	//Simply jump to the kernel address
	void (*kernelEntry)(void*) = (void(*)(void*))kernelAddress;

	kernelEntry(&graphicsMode);
}