#include "loading.h"

CHAR16* kernelFilename = L"voidos";
const void* kernelAddress = (const void*)0x100000000;

void memcpycustom(void* src, void* dest, uint64_t num) {
	for (int i = 0; i < num; i++) {
		*((char*)dest + i) = *((char*)src + i);
	}
}

void memsetcustom0(void* dest, uint64_t size) {
	for (int i = 0; i < size; i++) {
		*((char*)dest) = 0;
	}
}

bool loadElfMinimal(void* start, void* loadAddress) {
	//Assume that the elf does not need to be relocated

	Elf64_Ehdr hdr = *(Elf64_Ehdr*)start;

	uint32_t numSections = hdr.e_shnum;

	Elf64_Shdr* sHeader = (Elf64_Shdr*)((char*)start + hdr.e_shoff);

	char* address = kernelAddress;

	//Figure out which sections need to be copied to memory and copy them
	for (int i = 1; i < numSections; i++) {
		memsetcustom0(address, sHeader[i].sh_size);
		if (sHeader[i].sh_type != SHT_NOBITS) {
			memcpycustom((char*)start + sHeader[i].sh_offset, address, sHeader[i].sh_size);
		}

		address += sHeader[i].sh_size;
	}

	return true;
}

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
	void* tempAddress;
	SystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, 8196, &tempAddress);

	//Allocate 2GB space for the kernel for later
	//SystemTable->BootServices->AllocatePages(AllocateAddress, Efi, 1024 * 1024 / 2, kernelAddress);

	UINTN bufferSize = fInfo.FileSize;

	if (kernel->Read(kernel, &bufferSize, tempAddress) != EFI_SUCCESS) {
		Print(L"\nSorry, critical error: unable to read kernel file (kernel->Read(...) != EFI_SUCCESS)");
		return false;
	}

	//But wait, this is just the elf *file*, load the actual code now
	return loadElfMinimal(tempAddress, kernelAddress);
}

void transferControl() {
	//Simply jump to the kernel address
	void (*kernelEntry)(void*) = (void(*)(void*))kernelAddress;

	kernelEntry(&graphicsMode);
}