/*
* File: FileManagement.cpp
* Description: Manages finding, copying files and directories.
*/

#include "stdafx.h"

#include"FileManagement.h"

#include "ErrorHandling.h"

#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <list>
#include <ctime>

const FileDataStruct FileDataNULL = {_T("NULL"), FILETYPENULL};

/*
 * Description: Below is required to call FindFirstFile on a different thread.
 * This is used in case the IO doesn't finish in a timely maner. The thread
 * will be terminated and main program will finish in a timely maner.
*/
enum FindFileThreadAction {FindFileThreadFind, FindFileThreadFindNext};
typedef struct {
	HANDLE hFind;
	_TCHAR szDir[MAX_PATH];
	FindFileThreadAction action;
	WIN32_FIND_DATA ffd;
	int retVal;
}FindFileThreadStruct, *PFindFileThreadStruct;

DWORD WINAPI FindFileThreadedFunction(LPVOID lpParam);

DWORD FindFileThreadHandler();

/*
* Description: Handles creating a thread to handle FindFileFirst and FindFileNext.
*	The thread must return within MAX_FINDFILE_THREAD_TIME or it will terminate the thread.
*/
DWORD WINAPI FindFileThreadedFunction(LPVOID lpParam) {
	PFindFileThreadStruct threadData = (PFindFileThreadStruct)lpParam;

	if (threadData->action == FindFileThreadFind)
	{
		threadData->hFind = FindFirstFile(threadData->szDir, &threadData->ffd);
		if (INVALID_HANDLE_VALUE == threadData->hFind)
		{
			threadData->retVal = GetLastError();
			if (threadData->retVal != ERROR_NO_MORE_FILES)
			{
				printError(TEXT("FindFileThreadedFunction:FindFirstFile"), threadData->retVal);
			}
			return threadData->retVal;
		}
		else
		{
			threadData->action = FindFileThreadFindNext;
			threadData->retVal = 0;
		}
	}
	else
	{
		threadData->retVal=FindNextFile(threadData->hFind, &threadData->ffd);

		if (threadData->retVal == 0)
		{
			threadData->retVal = GetLastError();
			if (threadData->retVal != ERROR_NO_MORE_FILES)
			{
				printError(TEXT("FindFileThreadedFunction:FindFirstFile"), threadData->retVal);
			}
			return threadData->retVal;
		}
		else 
		{
			threadData->retVal = 0;
		}
	}

	return 0;
}

/*
* Description: Handles creating a thread to handle FindFileFirst and FindFileNext.
*	The thread must return within MAX_FINDFILE_THREAD_TIME or it will terminate the thread.
*/
DWORD FindFileThreadHandler(PFindFileThreadStruct pThreadData) {
	DWORD   dwThreadId=0;
	HANDLE  hThread=NULL;
	int timeTaken = 0;
	DWORD dwWait = 0;
	bool hasThreadCompleted = false;
	clock_t startClock, stopClock;
	int timeTakenMs = 0;


	//Set this to ERROR_FINDFILE_THREAD_TIMEDOUT on each call. If it is successful then it will be zero.
	// If it fails it will be ERROR_FINDFILE_THREAD_TIMEDOUT or the value of GetLastError()
	pThreadData->retVal = ERROR_FINDFILE_THREAD_TIMEDOUT;

	// Create the thread to begin execution on its own.
	hThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		FindFileThreadedFunction,       // thread function name
		pThreadData,          // argument to thread function 
		0,                      // use default creation flags 
		&dwThreadId);   // returns the thread identifier 


	// Check the return value for success.
	if (hThread == NULL)
	{
		printError(TEXT("CreateThread"));
		return -1;
	}

	
	startClock = clock();

	while (timeTaken < MAX_FINDFILE_THREAD_TIME)
	{
		dwWait = WaitForSingleObject(hThread, 0);
		if (dwWait != WAIT_TIMEOUT)
		{
			hasThreadCompleted = true;
			break;
		}
		Sleep(FINDFILE_THREAD_SLEEP);
		timeTaken += FINDFILE_THREAD_SLEEP;
	}

	stopClock = clock();

	timeTakenMs = 1000*(stopClock - startClock)/ CLOCKS_PER_SEC;

	if (!hasThreadCompleted)
	{
		dwWait = WaitForSingleObject(hThread, 0);
		if (dwWait != WAIT_TIMEOUT)
		{
			hasThreadCompleted = true;
		}
		else
		{
			if (TerminateThread(hThread, -1) == 0)
			{
				printError(TEXT("FindFileThreadHandler:TerminateThread"));
			}
			else
			{
				_tprintf(_T("FindFileThreadHandler:TerminateThread Success\n"));
			}
		}
	}


	// Thread has completed
	// clean up thread handle
	// Technically should never be NULL at this point.
	if (hThread != NULL)
	{
		CloseHandle(hThread);
	}

	if (hasThreadCompleted)
	{
		//_tprintf(_T("FindFileThreadHandler:Thread completed in %d ms successfully\n"), timeTakenMs);
		return 0;
	}

	return -2;
}


/*
 * Description: Finds files in a given path and adds intermediate files and directories in fileList.
 *  If FindFileNext fails it adds FindFileNull to fileList.
 */
int FindFilesInDirectory(const _TCHAR* path,  std::list<FileDataStruct> &fileList)
{
	LARGE_INTEGER filesize;
	size_t length_of_arg=0;
	_TCHAR szDir[MAX_PATH] = { 0 };
	DWORD dwError = 0;
	PFindFileThreadStruct pThreadData;

	pThreadData = (PFindFileThreadStruct)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
		sizeof(FindFileThreadStruct));

	if (pThreadData == NULL)
	{
		// If the array allocation fails, the system is out of memory
		// so there is no point in trying to print an error message.
		return -1;
	}

	pThreadData->action = FindFileThreadFind;

	StringCchLength(path, MAX_PATH, &length_of_arg);

	if (length_of_arg > (MAX_PATH - 3))
	{
		_tprintf(TEXT("\nError: Directory path is too long.%s\n", path));
		return (-1);
	}

	//_tprintf(TEXT("\nTarget directory is %s\n\n"), path);

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	StringCchCopy(pThreadData->szDir, MAX_PATH, path);
	StringCchCat(pThreadData->szDir, MAX_PATH, TEXT("\\*"));

	// Find the first file in the directory.
	FindFileThreadHandler(pThreadData);

	if (pThreadData->retVal != 0)
	{
		if (pThreadData->retVal == ERROR_NO_MORE_FILES)
		{
			return 0;
		}
		return -1;
	}

	// List all the files in the directory with some info about them.

	do
	{
		FileDataStruct data;
		//StringCchCopy(data.filePath, MAX_PATH, pThreadData->ffd.cFileName);
		if (!IsPathModifier(pThreadData->ffd.cFileName))
		{
			StringCchPrintf(data.filePath, MAX_PATH, _T("%s\\%s"), path, pThreadData->ffd.cFileName);
			if (pThreadData->ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				data.fileType = FILETYPEDIRECTORY;
				//_tprintf(TEXT("  %s   <DIR>\n"), pThreadData->ffd.cFileName);
			}
			else
			{
				data.fileType = FILETYPEFILE;
				filesize.LowPart = pThreadData->ffd.nFileSizeLow;
				filesize.HighPart = pThreadData->ffd.nFileSizeHigh;
				//_tprintf(TEXT("  %s   %ld bytes\n"), pThreadData->ffd.cFileName, filesize.QuadPart);
			}
			fileList.push_back(data);
		}
		
		FindFileThreadHandler(pThreadData);
	} while (pThreadData->retVal == 0);

	if (pThreadData->retVal != ERROR_NO_MORE_FILES)
	{
		fileList.push_back(FileDataNULL);
		_tprintf(TEXT("FindFirstFile"));
		dwError = pThreadData->retVal;
	}

	if (pThreadData->hFind != NULL)
	{
		FindClose(pThreadData->hFind);
	}

	if (pThreadData != NULL)
	{
		HeapFree(GetProcessHeap(), 0, pThreadData);
	}
	return dwError;
}

/*
 * Description: Gets the volume info from a given path.
 * This info is used by to determine if the drive is the same between sessions.
 */
DWORD GetVolumeInfo(VolumeInfoStruct* volumeInfo)
{
	DWORD retVal = 0;
	BOOL result = 0;
	result = GetVolumeInformation(volumeInfo->volumePath, volumeInfo->volumeNameBuffer, MAX_VOLUME_BUFFER_SIZE, &(volumeInfo->volumeSerialNumber),
		&(volumeInfo->maximumComponentLength), &(volumeInfo->fileSystemFlags), volumeInfo->fileSystemNameBuffer, MAX_VOLUME_BUFFER_SIZE);
	if (result == 0)
	{
		retVal=printError(_T("GetVolumeInformation"));
		_tprintf(_T("PATH: %s"), volumeInfo->volumePath);
	}
	else
	{
		_tprintf(_T("Path: %Ts\n"), volumeInfo->volumePath);
		_tprintf(_T("VolumeName: %Ts\n"), volumeInfo->volumeNameBuffer);
		_tprintf(_T("SerialNumber: %0.4X-%0.4X\n"), (volumeInfo->volumeSerialNumber >> 16) & 0xFFFF, volumeInfo->volumeSerialNumber & 0xFFFF);
		_tprintf(_T("FileSystemName: %Ts\n"), volumeInfo->fileSystemNameBuffer);
		_tprintf(_T("lpFileSystemFlags: %X\n"), volumeInfo->fileSystemFlags);
	}

	return retVal;
}

/*
* Description: Compares two volumeInfoStructs for equality.
* This info is used by to determine if the drive is the same between sessions.
* Return: zero if the same.
*/
BOOL CompareVolumeInfo(VolumeInfoStruct* volumeInfo1, VolumeInfoStruct* volumeInfo2)
{
	int strResult = 0;
	strResult = _tcscmp(volumeInfo1->volumeNameBuffer, volumeInfo2->volumeNameBuffer);

	if (strResult != 0)
	{
		if (strResult < 0)
		{
			return 1<<0;
		}
		else
		{
			return 1<<1;
		}
	}

	if (volumeInfo1->volumeSerialNumber != volumeInfo2->volumeSerialNumber)
	{
		if (volumeInfo1->volumeSerialNumber < volumeInfo2->volumeSerialNumber)
		{
			return 1 << 2;
		}
		else
		{
			return 1 << 3;
		}
	}

	strResult = _tcscmp(volumeInfo1->fileSystemNameBuffer, volumeInfo2->fileSystemNameBuffer);

	if (strResult != 0)
	{
		if (strResult < 0)
		{
			return 1 << 4;
		}
		else
		{
			return 1 << 5;
		}
	}

	if (volumeInfo1->fileSystemFlags != volumeInfo2->fileSystemFlags)
	{
		if (volumeInfo1->fileSystemFlags < volumeInfo2->fileSystemFlags)
		{
			return 1 << 6;
		}
		else
		{
			return 1 << 7;
		}
	}
}

/*
* Description: Compares two FileDataStructs for equality
*/
BOOL CompareFileData(const FileDataStruct* data1, const FileDataStruct* data2)
{
	int strResult=0;
	strResult = _tcscmp(data1->filePath, data2->filePath);

	if (strResult < 0)
	{
		return 1;
	}
	else if (strResult > 0)
	{
		return 2;
	}

	if (data1->fileType < data2->fileType)
	{
		return 4;
	}
	else if (data1->fileType > data2->fileType)
	{
		return 8;
	}
	return 0;
}

/*
* Description: Checks if the FileDataStruct is a Null element.
* a null element is assigned when FindFileNext fails.
*/
BOOL IsFileDataNull(FileDataStruct* data1)
{
	return CompareFileData(data1, &FileDataNULL);
}

/*
* Description: Checks if the file is a directory and if it is "." or "..".
*/
BOOL IsPathModifier(FileDataStruct* data1)
{
	int strResult = 0;

	if (data1->fileType == FILETYPEDIRECTORY)
	{
		if (_tcscmp(data1->filePath, _T(".")) == 0)
		{
			return true;
		}

		if (_tcscmp(data1->filePath, _T("..")) == 0)
		{
			return true;
		}
	}

	return false;
}

/*
* Description: Checks if it is "." or "..".
*/
BOOL IsPathModifier(_TCHAR* filename)
{
	if (_tcscmp(filename, _T(".")) == 0)
	{
		return true;
	}

	if (_tcscmp(filename, _T("..")) == 0)
	{
		return true;
	}

	return false;
}
