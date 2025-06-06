﻿#include "xload.h"
/*
Copyright (C) 2024 The XLang Foundation
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "xload.h"
#include "xhost.h"

#if (WIN32)
#include <Windows.h>
#define Path_Sep_S "\\"
#define Path_Sep '\\'
#define LibPrefix ""
#define ShareLibExt ".dll"
#define LOADLIB(path) LoadLibraryEx(path,NULL,LOAD_WITH_ALTERED_SEARCH_PATH)
#define GetProc(handle,funcName) GetProcAddress((HMODULE)handle, funcName)
#define UNLOADLIB(h) FreeLibrary((HMODULE)h)
#else
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <dlfcn.h>
#include <dirent.h>
#define Path_Sep_S "/"
#define Path_Sep '/'
#define LibPrefix "lib"
#if defined(__APPLE__)
#define ShareLibExt ".dylib"
#else
#define ShareLibExt ".so"
#endif
#define LOADLIB(path) dlopen(path, RTLD_LAZY)
#define GetProc(handle,funcName) dlsym(handle, funcName)
#define UNLOADLIB(handle) dlclose(handle)
#endif
#include <sstream>
#include <vector>

namespace X
{
	static bool dir(std::string search_path,
		std::vector<std::string>& subfolders,
		std::vector<std::string>& files)
	{
		bool ret = false;
#if (WIN32)
		BOOL result = TRUE;
		WIN32_FIND_DATA ff;
		std::string search_pat = search_path + Path_Sep_S + "*.*";
		HANDLE findhandle = FindFirstFile(search_pat.c_str(), &ff);
		if (findhandle != INVALID_HANDLE_VALUE)
		{
			ret = true;
			BOOL res = TRUE;
			while (res)
			{
				// We only want directories
				if (ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					std::string fileName(ff.cFileName);
					if (fileName != "." && fileName != "..")
					{
						subfolders.push_back(fileName);
					}
				}
				else
				{
					std::string fileName(ff.cFileName);
					files.push_back(fileName);
				}
				res = FindNextFile(findhandle, &ff);
			}
			FindClose(findhandle);
		}
#else
		DIR* dir;
		struct dirent* ent;
		if ((dir = opendir(search_path.c_str())) != NULL)
		{
			ret = true;
			while ((ent = readdir(dir)) != NULL)
			{
				if (ent->d_type == DT_DIR)
				{
					std::string fileName(ent->d_name);
					if (fileName != "." && fileName != "..")
					{
						subfolders.push_back(fileName);
					}
				}
				else if (ent->d_type == DT_REG)//A regular file
				{
					std::string fileName(ent->d_name);
					files.push_back(fileName);
				}
			}
			closedir(dir);
		}
#endif
		return ret;
	}
	static bool file_search(std::string folder,
		std::string fileName,
		std::vector<std::string>& outFiles,
		bool findAll)
	{
		bool bFind = false;
		std::vector<std::string> subfolders;
		std::vector<std::string> files;
		bool bOK = dir(folder, subfolders, files);
		if (bOK)
		{
			for (auto& f : files)
			{
				if (f == fileName)
				{
					outFiles.push_back(folder + Path_Sep_S + f);
					bFind = true;
					break;
				}
			}
			for (auto& fd : subfolders)
			{
				bool bRet = file_search(folder + Path_Sep_S + fd, fileName, outFiles, findAll);
				if (bRet)
				{
					bFind = true;
					if (!findAll)
					{
						break;
					}
				}
			}
		}
		return bFind;
	}
	XHost* g_pXHost = nullptr;
	XLoad::XLoad()
	{

	}
	AppEventCode XLoad::HandleAppEvent(int signum)
	{
		return g_pXHost?g_pXHost->HandleAppEvent(signum): AppEventCode::Error;
	}
	static std::vector<std::string> SplitStr(const std::string& str, char delim)
	{
		std::vector<std::string> list;
		std::string temp;
		std::stringstream ss(str);
		while (std::getline(ss, temp, delim))
		{
			list.push_back(temp);
		}
		return list;
	}
	static bool SearchDll(std::string& dllMainName, std::string& searchPath,
		std::string& findFileName)
	{
		bool bHaveDll = false;
		std::vector<std::string> candiateFiles;
		bool bRet = file_search(searchPath,
			LibPrefix+dllMainName + ShareLibExt, candiateFiles,false);
		if (bRet && candiateFiles.size() > 0)
		{
			findFileName = candiateFiles[0];
			bHaveDll = true;
		}
		return bHaveDll;
	}
	void XLoad::Unload()
	{
		if (xlangLibHandler)
		{
			typedef void (*Unload)();
			Unload unload = (Unload)GetProc(xlangLibHandler, "Unload");
			if (unload)
			{
				unload();
			}
			UNLOADLIB(xlangLibHandler);
			xlangLibHandler = nullptr;
		}
	}
	int XLoad::Load(Config* pCfg)
	{
		m_pConfig = pCfg;
		std::string engName("xlang_eng");
		std::string loadDllName;
		std::string searchPath(m_pConfig->appPath);
		bool bFound = SearchDll(engName, searchPath, loadDllName);
		if (!bFound)
		{
			if (pCfg->dllSearchPath)
			{
				std::string strPaths(pCfg->dllSearchPath);
				std::vector<std::string> otherSearchPaths = SplitStr(strPaths, '\n');
				for (auto& pa : otherSearchPaths)
				{
					bFound = SearchDll(engName, pa, loadDllName);
					if (bFound)
					{
						break;
					}
				}
			}
		}
		if (!bFound)
		{
			return -1;
		}
		typedef void (*LOAD)(void* pXload,void** pXHost);
		void* libHandle = LOADLIB(loadDllName.c_str());
		if (libHandle)
		{
			LOAD load = (LOAD)GetProc(libHandle, "Load");
			if (load)
			{
				load((void*)this,(void**)&g_pXHost);
			}
			SetXLangLibHandler(libHandle);
			return 0;
		}
		else
		{
#if (WIN32)
			auto err = GetLastError();
			fprintf(stderr, "LoadLibrary failed: %d\n", err);
#else
			auto err = dlerror();
			fprintf(stderr, "dlopen failed: %s\n", err);
#endif
		}
		return -1;
	}
	void XLoad::SetLogFuncs(void* lock, void* unlock, void* logWrite)
	{
		if (xlangLibHandler == nullptr)
		{
			return;
		}
		typedef void (*SetLogFuncs)(void* lock, void* unlock, void* logWrite);
		SetLogFuncs func = (SetLogFuncs)GetProc(xlangLibHandler, "SetLogFuncs");
		if (func)
		{
			func(lock,unlock,logWrite);
		}
	}
	void XLoad::EventLoop()
	{
		if (xlangLibHandler == nullptr)
		{
			return;
		}
		typedef void (*EventLoop)();
		EventLoop loop = (EventLoop)GetProc(xlangLibHandler, "EventLoop");
		if (loop)
		{
			loop();
		}
	}
	int XLoad::Run()
	{
		if (xlangLibHandler == nullptr)
		{
			return -1;
		}
		typedef void (*XRun)();
		XRun run = (XRun)GetProc(xlangLibHandler, "Run");
		if (run)
		{
			run();
			return 0;
		}
		else
		{
			return -1;
		}
	}
}