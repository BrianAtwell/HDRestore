#include "stdafx.h"
#include "CppUnitTest.h"
#include "tchar.h"
#include "..\CHDRestore\FileManagement.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	_TCHAR* DirectoryPaths_images[] = {
		_T("C:\\temp\\images"),
		_T("C:\\temp\\images\\"),
		_T("C:\\temp\\images\\animation.gif"),
	};

	_TCHAR* DirectoryPaths_root[] = {
		_T("C:\\"),
		_T("C:\\test.txt")
	};


	TEST_CLASS(FileUnitTest)
	{
	public:
		
		TEST_METHOD(TestMethodDirectoryName_images)
		{
			// TODO: Your test code here
			Logger::WriteMessage(_T("Start TestMethodDirectoryName_images\n"));

			_TCHAR tempNewPath[MAX_PATH] = { 0 };

			const int arrayLen = sizeof(DirectoryPaths_images) / sizeof(TCHAR*);

			for (int i = 0; i < arrayLen; i++)
			{
				StringGetDirectoryName(tempNewPath, MAX_PATH, DirectoryPaths_images[i]);
				Logger::WriteMessage(_T("Directory Name: \""));
				Logger::WriteMessage(tempNewPath);
				Logger::WriteMessage(_T("\" Path: \""));
				Logger::WriteMessage(DirectoryPaths_images[i]);
				Logger::WriteMessage(_T("\""));
				Logger::WriteMessage(_T("\n"));
				Assert::AreEqual(tempNewPath, _T("images"));
			}
		}

		TEST_METHOD(TestMethodDirectoryName_root)
		{
			// TODO: Your test code here
			Logger::WriteMessage(_T("Start TestMethodDirectoryName_root\n"));

			_TCHAR tempNewPath[MAX_PATH] = { 0 };

			const int arrayLen = sizeof(DirectoryPaths_root) / sizeof(TCHAR*);

			for (int i = 0; i < arrayLen; i++)
			{
				StringGetDirectoryName(tempNewPath, MAX_PATH, DirectoryPaths_root[i]);
				Logger::WriteMessage(_T("Directory Name: \""));
				Logger::WriteMessage(tempNewPath);
				Logger::WriteMessage(_T("\" Path: \""));
				Logger::WriteMessage(DirectoryPaths_root[i]);
				Logger::WriteMessage(_T("\""));
				Logger::WriteMessage(_T("\n"));
				Assert::AreEqual(tempNewPath, _T("C:\\"));
			}
		}

		TEST_METHOD(TestMethodConvertToNewPath)
		{
			_TCHAR recoveryDrive[4] = _T("C:\\");
			_TCHAR restoredPath[MAX_PATH] = _T("C:\\testRecovery");
			_TCHAR tempNewPath[MAX_PATH] = { 0 };
			int resultCount = 0;
			bool resultVal = 0;
			size_t tempStrLength = 0;

			_TCHAR* recoveryPath[] = {
				_T("C:\\"),
				_T("C:\\temp") 
			};

			_TCHAR* fileList[] = {
				_T("C:\\test.txt"),
				_T("C:\\temp\\images"),
				_T("C:\\temp\\images\\"),
				_T("C:\\temp\\images\\animation.gif"),
			};

			_TCHAR* resultList[] = {
				_T("C:\\testRecovery\\rootC\\test.txt"),
				_T("C:\\testRecovery\\rootC\\temp\\images"),
				_T("C:\\testRecovery\\rootC\\temp\\images\\"),
				_T("C:\\testRecovery\\rootC\\temp\\images\\animation.gif"),
				_T(""),
				_T("C:\\testRecovery\\temp\\images"),
				_T("C:\\testRecovery\\temp\\images\\"),
				_T("C:\\testRecovery\\temp\\images\\animation.gif"),
			};


			int fileListLen = sizeof(fileList) / sizeof(_TCHAR*);
			int recoveryPathLen = sizeof(recoveryPath) / sizeof(_TCHAR*);
			int resultLen = sizeof(tempNewPath) / sizeof(_TCHAR*);

			FILE* file = _tfreopen(_T("log.txt"), _T("w"), stdout);

			for (int recoveryPathPos = 0; recoveryPathPos < recoveryPathLen; recoveryPathPos++)
			{
				for (int fileListPos = 0; fileListPos < fileListLen; fileListPos++)
				{
					ZeroMemory(tempNewPath, resultLen);
					resultVal = CovertPathToNewPath(fileList[fileListPos], recoveryPath[recoveryPathPos], restoredPath, tempNewPath);

					Logger::WriteMessage(_T("File: \""));
					Logger::WriteMessage(fileList[fileListPos]);
					Logger::WriteMessage(_T("\" recoveryPath: \""));
					Logger::WriteMessage(recoveryPath[recoveryPathPos]);
					Logger::WriteMessage(_T("\" restoredPath: \""));
					Logger::WriteMessage(restoredPath);
					Logger::WriteMessage(_T("\" tempNewPath: \""));
					Logger::WriteMessage(tempNewPath);
					Logger::WriteMessage(_T("\" Expected: \""));
					if (resultCount < resultLen)
					{
						if (StringCchLength(tempNewPath, MAX_PATH, &tempStrLength) == S_OK)
						{
							if (tempStrLength == 0 && resultVal)
							{
								Assert::Fail(_T("Failed to return false for empty result."));
							}
							if (tempStrLength > 0 && !resultVal)
							{
								Assert::Fail(_T("Failed to return true for result that is not empty."));
							}
						}
						Logger::WriteMessage(resultList[resultCount]);
						Assert::AreEqual(resultList[resultCount], tempNewPath);
					}
					Logger::WriteMessage(_T("\" \n"));
					resultCount++;
				}
			}

			fclose(file);
		}

	};
}