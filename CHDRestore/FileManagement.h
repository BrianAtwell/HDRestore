#pragma once
/*
 * File: FileManagement.h
 * Description: Manages finding, copying files and directories.
 */

#include "windows.h"
#include <list>
#include <strsafe.h>

enum FileTypeEnum {FILETYPEFILE, FILETYPEDIRECTORY, FILETYPENULL};

typedef struct FileDataStruct {
	_TCHAR filePath[MAX_PATH];
	FileTypeEnum fileType;

	FileDataStruct(_TCHAR* lfilePath, FileTypeEnum lfileTypeEnum) :  fileType(lfileTypeEnum)
	{
		StringCchCopy(filePath, MAX_PATH, lfilePath);
	}

	FileDataStruct()
	{
		fileType = FILETYPEFILE;
	}
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
BOOL StringPathRemoveFileSpec(_TCHAR* dest, size_t destSize, _TCHAR* path);
BOOL StringGetDirectoryName(_TCHAR* dest, size_t destSize, _TCHAR* path);
BOOL IsPathModifier(FileDataStruct* data1);
BOOL IsPathModifier(_TCHAR* filename);
BOOL CovertPathToNewPath(_TCHAR *path, _TCHAR *basePath, _TCHAR *newBasePath, _TCHAR *newPath);