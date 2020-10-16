// CHDRestore.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"

#include "SystemUtilities.h"
#include "StringUtilities.h"
#include "FileManagement.h"
#include "ErrorHandling.h"

#include <cstdio>
#include <tchar.h>
#include <windows.h>
#include <strsafe.h>
#include <list>

#include <cstdlib>
#include <signal.h>

// Pass in (Recovery Drive, Restore BASEPATH,  RDrive path)

#define MAX_DRIVE   4

const _TCHAR PROGRAM_NAME[] = _T("CHDRestore");
_TCHAR recoveryDrive[MAX_DRIVE] = _T("C:\\");
_TCHAR restoredPath[MAX_PATH] = _T("C:\\testRecovery");
_TCHAR recoveryPath[MAX_PATH] = _T("C:\\temp");

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
	if (argc == 4)
	{

		std::list<FileDataStruct> fileList;

		std::list<FileDataStruct> curFileList;

		std::list<FileDataStruct>::iterator beforeInsert;

		FileDataStruct baseDirectoryData;

		// Register signal and signal handler
		signal(SIGINT, signal_callback_handler);

		StringCopyExcept(recoveryDrive, MAX_DRIVE, _T('"'), argv[1]);
		StringCopyExcept(restoredPath, MAX_PATH, _T('"'), argv[2]);
		StringCopyExcept(recoveryPath, MAX_PATH, _T('"'), argv[3]);

		if (!DirectoryExists(recoveryDrive))
		{
			_tprintf(_T("RecoveryDrive path '%s' does not exist!"), recoveryDrive);
		}

		if (!DirectoryExists(restoredPath))
		{
			_tprintf(_T("RestoredPath path '%s' does not exist!"), restoredPath);
		}

		if (!DirectoryExists(recoveryPath))
		{
			_tprintf(_T("RecoveryPath path '%s' does not exist!"), recoveryPath);
		}
		
		baseDirectoryData.fileType = FILETYPEDIRECTORY;
		StringCchCopy(baseDirectoryData.filePath, MAX_PATH, recoveryPath);

		FindFilesInDirectory(recoveryPath, fileList);

		//CovertPathToNewPath(recoveryPath, recoveryPath, restoredPath, baseDirectoryData.filePath);

		// This following code is needed to recursivly scan the directory
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
			if (is_process_terminated() == SIGINT)
			{
				_tprintf(_T("Received Terminate Signal Main function Exit!\n"));
				break;
			}

		}

		fileList.push_front(baseDirectoryData);
		_tprintf(_T("baseDirectory pushed to fileList: %s\n"), baseDirectoryData.filePath);

		CopyAllFiles(recoveryDrive, recoveryPath, restoredPath, fileList);


	}
	else
	{
		_tprintf(_T("Error not the correct number of arguments\n"));
		_tprintf(_T("%Ts [Recovery Drive] [Restored BASEPATH] [Recovery Path]\n"), PROGRAM_NAME);
		_tprintf(_T("Example:\n"));
		_tprintf(_T("        %Ts D:\\ \"C:\\Restored\\\" \"D:\\Music\\\" \n"), PROGRAM_NAME);
	}
}
