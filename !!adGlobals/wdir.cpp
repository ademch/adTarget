//

#include "stdafx.h"
#include "wdir.h"

//CString projDir;
//
//CString getProjectDir()
//{
//const DWORD count=600;
//char *exepath;
//char *path;
//int i;
//CString subtle_dir;
//
//	if (projDir.GetLength() > 0) return projDir;
//
//	exepath = new char[count];
//	path    = new char[count];
//
//	GetModuleFileName(GetModuleHandle(NULL), exepath, count);  // handle to module
//
//	for (i=lstrlen(exepath)-1;i>=0;i--)
//	{
//		if (exepath[i]=='\\')
//		{
//			strncpy(path,exepath,i+1);
//			path[i+1]=0;				//add terminator
//			break;
//		}
//	}
//
//delete [] exepath;
////subtle_dir = CString(path);
////delete [] path;
////return subtle_dir;
//projDir = CString(path);
//return projDir;
//}

TCHAR dir_project[600] = "";

TCHAR* getProjectDir()
{
TCHAR exepath[MAX_PATH];

int i;

	if (_tcslen(dir_project) == 0)
	{
		GetModuleFileNameA(GetModuleHandle(NULL), exepath, MAX_PATH);  // handle to module

		for (i=_tcslen(exepath)-1; i>=0; i--)
		{
			if (exepath[i]=='\\')
			{
				exepath[i+1]=0;				//place terminator
				break;
			}
		}
		sprintf(dir_project, "%s", exepath);		
	}

return dir_project;
}


char* FullPathToFile(const char* fileName)
{
	static char strProjDir[512];

	strcpy(strProjDir, getProjectDir());
	return strcat(strProjDir, fileName);
}


char* GetFileExtension(const char* fileName)
{
	static char strFileExt[12];

	const char* extPos = strrchr(fileName, '.');
	if (extPos == NULL) return NULL;

	return strcpy(strFileExt, extPos);
}
