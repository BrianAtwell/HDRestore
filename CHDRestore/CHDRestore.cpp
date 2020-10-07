// CHDRestore.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"

#include "FileManagement.h"

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

		for (std::list<FileDataStruct>::iterator it = fileList.begin(); it != fileList.end(); ++it)
		{

		}


	}
	else
	{
		_tprintf(_T("Error not the correct number of arguments\n"));
		_tprintf(_T("%Ts [Recovery Drive] [Restored BASEPATH] [Recovery Path]\n"), PROGRAM_NAME);
		_tprintf(_T("Example:\n"));
		_tprintf(_T("        %Ts D:\\ \"C:\\Restored\\\" \"D:\\Music\\\" \n"), PROGRAM_NAME);
	}
}
