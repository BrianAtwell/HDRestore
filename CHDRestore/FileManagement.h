#pragma once
/*
 * File: FileManagement.h
 * Description: Manages finding, copying files and directories.
 */

#include "windows.h"
#include <list>

enum FileTypeEnum {FILETYPEFILE, FILETYPEDIRECTORY, FILETYPENULL};

typedef struct {
	_TCHAR filePath[MAX_PATH];
	FileTypeEnum fileType;
} FileDataStruct;

#define MAX_VOLUME_BUFFER_SIZE	MAX_PATH
typedef struct {
	_TCHAR volumePath[MAX_VOLUME_BUFFER_SIZE];
	_TCHAR  volumeNameBuffer[MAX_VOLUME_BUFFER_SIZE];
	DWORD volumeSerialNumber;
	DWORD maximumComponentLength;
	DWORD fileSystemFlags;
	_TCHAR  fileSystemNameBuffer[MAX_VOLUME_BUFFER_SIZE];
}VolumeInfoStruct;

int FindFilesInDirectory(const _TCHAR* path, std::list<FileDataStruct> &fileList);
DWORD GetVolumeInfo(VolumeInfoStruct* volumeInfo);