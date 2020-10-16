#pragma once
/*
 * File: FileManagement.h
 * Description: Manages finding, copying files and directories.
 */

#include "windows.h"
#include <list>
#include <strsafe.h>

// Progress bar for the log file looks like the following example.
// But the '=' is progress bar and the max used is represented by LOG_PROGESS_LENGTH
// [===]
#define LOG_PROGRESS_LENGTH	20
#define LOG_PROGRESS_HAS_BRACKETS
#define LOG_PROGRESS_BRACKET_START_CHAR	_T('[')
#define LOG_PROGRESS_BRACKET_END_CHAR	_T(']')
#define LOG_PROGRESS_BAR_CHAR			_T('=')

// technically only need + 5 because of the two brackets, a space, a newline character and a null character.
#define MAX_LOG_STRING_LEN MAX_PATH+LOG_PROGRESS_LENGTH+10

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

#define MAX_VOLUME_BUFFER_SIZE			MAX_PATH
#define MAX_VOLUME_FILE_READ_SIZE		MAX_VOLUME_BUFFER_SIZE+25
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
BOOL GetLogName(_TCHAR* recoveryDrive, _TCHAR* recoveryPath, _TCHAR* restorePath, _TCHAR* logPath, size_t logPathLen);
BOOL CopyAllFiles(_TCHAR* recoveryDrive, _TCHAR *recoveryPath, _TCHAR *restoredPath, std::list<FileDataStruct> &fileList);
BOOL DirectoryExists(_TCHAR* path);