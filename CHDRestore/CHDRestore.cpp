// CHDRestore.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"

#include "FileManagement.h"
#include "ErrorHandling.h"

#include <cstdio>
#include <tchar.h>
#include <windows.h>
#include <strsafe.h>
#include <list>

// Pass in (Recovery Drive, Restore BASEPATH, -R RDrive path)

#define MAX_DRIVE   4

const _TCHAR PROGRAM_NAME[] = _T("CHDRestore");
_TCHAR recoveryDrive[MAX_DRIVE] = _T("C:\\");
_TCHAR restoredPath[MAX_PATH] = _T("C:\\testRecovery");
_TCHAR recoveryPath[MAX_PATH] = _T("C:\\temp");
_TCHAR copyFileLogName[MAX_PATH] = { 0 };

/*
DWORD CALLBACK  CopyProgressRoutine(
	LARGE_INTEGER TotalFileSize,
	LARGE_INTEGER TotalBytesTransferred,
	LARGE_INTEGER StreamSize,
	LARGE_INTEGER StreamBytesTransferred,
	DWORD dwStreamNumber,
	DWORD dwCallbackReason,
	HANDLE hSourceFile,
	HANDLE hDestinationFile,
	LPVOID lpData
	);

DWORD CALLBACK  CopyProgressRoutine(
	LARGE_INTEGER TotalFileSize,
	LARGE_INTEGER TotalBytesTransferred,
	LARGE_INTEGER StreamSize,
	LARGE_INTEGER StreamBytesTransferred,
	DWORD dwStreamNumber,
	DWORD dwCallbackReason,
	HANDLE hSourceFile,
	HANDLE hDestinationFile,
	LPVOID lpData
	)
{
	int percentage = (double(TotalBytesTransferred.QuadPart) / double(TotalFileSize.QuadPart)) * 100;

	printf("%%%d copied\n", percentage);

	return PROGRESS_CONTINUE;
}
*/

int _tmain(int argc, _TCHAR* argv[])
{
	argc = 4;
	if (argc == 4)
	{
		VolumeInfoStruct volumeInfo = { {0},{0},0,0,0,{0} };
		StringCchCopy(volumeInfo.volumePath, MAX_PATH, recoveryDrive);
		GetVolumeInfo(&volumeInfo);

		std::list<FileDataStruct> fileList;

		std::list<FileDataStruct> curFileList;

		FindFilesInDirectory(recoveryPath, fileList);

		std::list<FileDataStruct>::iterator beforeInsert;

		//directoryList.push_back(FileDataStruct(recoveryPath, FILETYPEDIRECTORY));
		for (std::list<FileDataStruct>::iterator it = fileList.begin(); it != fileList.end(); ++it)
		{
			_tprintf(_T("%s: %s\n"), it->fileType == FILETYPEFILE ? _T("File") : _T("Directory"), it->filePath);
			if (it->fileType == FILETYPEDIRECTORY)
			{
				curFileList.clear();
				if (FindFilesInDirectory(it->filePath, curFileList) != 0)
				{
					_tprintf(_T("FindFilesInDirectory Failed! file list length %d\n"), curFileList.size());
				}

				// Save iterator then insert insert after the current elment.
				// Then restore the iterator to the first new inserted element.
				beforeInsert = it;
				it++;
				fileList.insert(it, curFileList.begin(), curFileList.end());
				it = beforeInsert;
				it++;
				_tprintf(_T("%s: %s\n"), it->fileType == FILETYPEFILE ? _T("File") : _T("Directory"), it->filePath);
			}

		}

		_TCHAR tempNewPath[MAX_PATH] = { 0 };

		CovertPathToNewPath(fileList.begin()->filePath, recoveryPath, restoredPath, tempNewPath);

		_tprintf(_T("CovertPathToNewPath File %s NewFilePath %s\n"), fileList.begin()->filePath, tempNewPath);

		StringGetDirectoryName(tempNewPath, MAX_PATH, recoveryPath);

		if (_tcscmp(tempNewPath, recoveryDrive) == 0)
		{
			StringCchPrintf(tempNewPath, MAX_PATH, _T("root%c"), recoveryDrive[0]);
		}

		StringCchPrintf(tempNewPath, MAX_PATH, _T("%s\\%s.txt"), recoveryDrive[0]);
		_tprintf(_T("Folder Name: %s\n"), tempNewPath);


		/*

		BOOL retVal = 0;

		retVal = CopyFileEx(fileList.begin()->filePath, tempNewPath, (LPPROGRESS_ROUTINE)CopyProgressRoutine, NULL, false, COPY_FILE_FAIL_IF_EXISTS);

		if (retVal) {
			_tprintf(_T("%s copied to current directory.\n"), tempNewPath);
		}
		else {
			_tprintf(_T("%s not copied to current directory.\n"), tempNewPath);
			printError(_T("CopyFileEx"));
		}
		*/

		/*
		for (std::list<FileDataStruct>::iterator it = fileList.begin(); it != fileList.end(); ++it)
		{
			if (it->fileType == FILETYPEFILE)
			{
				CovertPathToNewPath(it->filePath, recoveryPath, restoredPath, tempNewPath);
				CopyFileEx(it->filePath, tempNewPath, (LPPROGRESS_ROUTINE)CopyProgressRoutine, NULL, false, COPY_FILE_FAIL_IF_EXISTS);
			}
			else if (it->fileType == FILETYPEDIRECTORY)
			{
				// Create Directory
			}
		}
		*/


	}
	else
	{
		_tprintf(_T("Error not the correct number of arguments\n"));
		_tprintf(_T("%Ts [Recovery Drive] [Restored BASEPATH] [Recovery Path]\n"), PROGRAM_NAME);
		_tprintf(_T("Example:\n"));
		_tprintf(_T("        %Ts D:\\ \"C:\\Restored\\\" \"D:\\Music\\\" \n"), PROGRAM_NAME);
	}
}
