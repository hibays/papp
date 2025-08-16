#define _WIN32_WINNT  0x600
#define WINVER        0x600
#define _SHELL32_
#include "CoreLib.h"
//#include <vld.h>
#include "Elevate.h"

DLL_TYPE(Shell32, BOOL, IsUserAnAdmin)();
DLL_CACHEVAR(Shell32, IsUserAnAdmin);
#define DLL_ShIsUserAnAdmin        DLL_EXEC(Shell32, IsUserAnAdmin, DLL_EX_NONE)


BOOL __stdcall DllMain(HANDLE Module, DWORD Reason, LPVOID Reserved)
{
  GxApp.Init(Module, Reason, Reserved);

  return 1;
}

// SH_RegCreateKeyEx() and SH_RegOpenKeyEx() create/open a fake registry key.
// Actual registry key is created/opened in Elevate.exe.
// Only used by the ShellExecuteExElevated functions.
extern LONG __stdcall WINAPI SH_RegCreateKeyExElevatedA(
  HKEY hKey,
  LPCSTR lpSubKey,
  DWORD /*Reserved*/,
  LPSTR lpClass,
  DWORD dwOptions,
  REGSAM samDesired,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  PHKEY phkResult,
  LPDWORD lpdwDisposition)
{
  HKEY_Pack *Data = new HKEY_Pack;

  Data->MxTargetFunc = "SH_RegCreateKeyExA";
  if (hKey == HKEY_CLASSES_ROOT)  Data->MxRootKey = "HKCR";
  else if (hKey == HKEY_CURRENT_CONFIG)  Data->MxRootKey = "HKCC";
  else if (hKey == HKEY_CURRENT_USER)  Data->MxRootKey = "HKCU";
  else if (hKey == HKEY_LOCAL_MACHINE)  Data->MxRootKey = "HKLM";
  else if (hKey == HKEY_USERS)  Data->MxRootKey = "HKU";
  else
  {
    delete Data;
    return ERROR_INVALID_HANDLE;
  }
  if (!lpSubKey)
  {
    delete Data;
    return ERROR_INVALID_PARAMETER;
  }
  Data->MxSubKey = lpSubKey;
  if (lpClass)
  {
    Data->MxClassExists = 1;
    Data->MxClass = lpClass;
  }
  Data->MxOptions = dwOptions;
  Data->MxRegSam = (UInt64)samDesired;
  if (lpSecurityAttributes)
  {
    Data->MxAttrExists = 1;
    Data->MxAttrInherit = lpSecurityAttributes->bInheritHandle;
  }

  *phkResult = (HKEY)Data;
  if (lpdwDisposition)  *lpdwDisposition = REG_CREATED_NEW_KEY;

  return ERROR_SUCCESS;
}

extern LONG __stdcall WINAPI SH_RegCreateKeyExElevatedW(
  HKEY hKey,
  LPCWSTR lpSubKey,
  DWORD /*Reserved*/,
  LPWSTR lpClass,
  DWORD dwOptions,
  REGSAM samDesired,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  PHKEY phkResult,
  LPDWORD lpdwDisposition)
{
  HKEY_Pack *Data = new HKEY_Pack;

  Data->MxTargetFunc = "SH_RegCreateKeyExW";
  if (hKey == HKEY_CLASSES_ROOT)  Data->MxRootKey = "HKCR";
  else if (hKey == HKEY_CURRENT_CONFIG)  Data->MxRootKey = "HKCC";
  else if (hKey == HKEY_CURRENT_USER)  Data->MxRootKey = "HKCU";
  else if (hKey == HKEY_LOCAL_MACHINE)  Data->MxRootKey = "HKLM";
  else if (hKey == HKEY_USERS)  Data->MxRootKey = "HKU";
  else
  {
    delete Data;
    return ERROR_INVALID_HANDLE;
  }
  if (!lpSubKey)
  {
    delete Data;
    return ERROR_INVALID_PARAMETER;
  }
  Data->MxSubKey = System::Unicode2BString(lpSubKey);
  if (lpClass)
  {
    Data->MxClassExists = 1;
    Data->MxClass = System::Unicode2BString(lpClass);
  }
  Data->MxOptions = dwOptions;
  Data->MxRegSam = (UInt64)samDesired;
  if (lpSecurityAttributes)
  {
    Data->MxAttrExists = 1;
    Data->MxAttrInherit = lpSecurityAttributes->bInheritHandle;
  }

  *phkResult = (HKEY)Data;
  if (lpdwDisposition)  *lpdwDisposition = REG_CREATED_NEW_KEY;

  return ERROR_SUCCESS;
}

extern LONG __stdcall WINAPI SH_RegOpenKeyExElevatedA(
  HKEY hKey,
  LPCSTR lpSubKey,
  DWORD /*ulOptions*/,
  REGSAM samDesired,
  PHKEY phkResult)
{
  HKEY_Pack *Data = new HKEY_Pack;

  Data->MxTargetFunc = "SH_RegOpenKeyExA";
  if (hKey == HKEY_CLASSES_ROOT)  Data->MxRootKey = "HKCR";
  else if (hKey == HKEY_CURRENT_USER)  Data->MxRootKey = "HKCU";
  else if (hKey == HKEY_LOCAL_MACHINE)  Data->MxRootKey = "HKLM";
  else if (hKey == HKEY_USERS)  Data->MxRootKey = "HKU";
  else
  {
    delete Data;
    return ERROR_INVALID_HANDLE;
  }
  if (!lpSubKey)
  {
    delete Data;
    return ERROR_INVALID_PARAMETER;
  }
  Data->MxSubKey = lpSubKey;
  Data->MxRegSam = (UInt64)samDesired;

  *phkResult = (HKEY)Data;

  return ERROR_SUCCESS;
}

extern LONG __stdcall WINAPI SH_RegOpenKeyExElevatedW(
  HKEY hKey,
  LPCWSTR lpSubKey,
  DWORD /*ulOptions*/,
  REGSAM samDesired,
  PHKEY phkResult)
{
  HKEY_Pack *Data = new HKEY_Pack;

  Data->MxTargetFunc = "SH_RegOpenKeyExA";
  if (hKey == HKEY_CLASSES_ROOT)  Data->MxRootKey = "HKCR";
  else if (hKey == HKEY_CURRENT_USER)  Data->MxRootKey = "HKCU";
  else if (hKey == HKEY_LOCAL_MACHINE)  Data->MxRootKey = "HKLM";
  else if (hKey == HKEY_USERS)  Data->MxRootKey = "HKU";
  else
  {
    delete Data;
    return ERROR_INVALID_HANDLE;
  }
  if (!lpSubKey)
  {
    delete Data;
    return ERROR_INVALID_PARAMETER;
  }
  Data->MxSubKey = System::Unicode2BString(lpSubKey);
  Data->MxRegSam = (UInt64)samDesired;

  *phkResult = (HKEY)Data;

  return ERROR_SUCCESS;
}

extern LONG __stdcall WINAPI SH_RegCloseKeyElevated(HKEY hKey)
{
  if (hKey == NULL)  return ERROR_INVALID_HANDLE;
  HKEY_Pack *Data = (HKEY_Pack *)hKey;

  delete Data;

  return ERROR_SUCCESS;
}


#ifndef EXTENDED_STARTUPINFO_PRESENT
  #define EXTENDED_STARTUPINFO_PRESENT   0x00080000
#endif

#ifndef SEE_MASK_ICON
  #define SEE_MASK_ICON   0x00000010
#endif

DWORD FixCreationFlags(DWORD Flags)
{
  // Remove the flag that indicates a STARTUPINFOEX structure.
  return Flags & ~EXTENDED_STARTUPINFO_PRESENT;
}

// Processes the environment parameter into a BString.
void ProcessEnvironment(BString &Result, UInt64 CreationFlags, LPVOID lpEnvironment)
{
  size_t x;

  Result.Empty();
  if (lpEnvironment != NULL)
  {
    if (CreationFlags & CREATE_UNICODE_ENVIRONMENT)
    {
      WCHAR *Environ = static_cast<WCHAR *>(lpEnvironment);
      for (x = 0; Environ[x] != L'\0' || Environ[x + 1] != L'\0'; x++);
      x += 2;
      Result = BString(static_cast<SByte *>(lpEnvironment), x * 2);
    }
    else
    {
      CHAR *Environ = static_cast<CHAR *>(lpEnvironment);
      for (x = 0; Environ[x] != '\0' || Environ[x + 1] != '\0'; x++);
      x += 2;
      Result = BString(static_cast<SByte *>(lpEnvironment), x);
    }
  }
}

// Gets the binary length of a PIDL for serialization.
size_t PIDL_GetSize(LPCITEMIDLIST TempPIDL)
{
  if (TempPIDL == NULL)  return 0;

  size_t TempSize = sizeof(USHORT);
  USHORT cb;
  do
  {
    cb = TempPIDL->mkid.cb;
    TempPIDL = (LPCITEMIDLIST)((LPBYTE)TempPIDL + cb);
    TempSize += (size_t)cb;
  } while (cb);

  return TempSize;
}

// Manages the named-pipe and event objects.
class ElevatedLink
{
public:
  ElevatedLink()
  {
    MxConnected = 0;
  }

  ~ElevatedLink()
  {
    if (MxConnected)  Destroy();
  }

  // Called before creating Elevate.exe.
  int Create(UInt64 ProcessID, UInt64 ThreadID)
  {
    if (!MxProcessComm.Create("585656DF-ADC4-48BC-AE19-7CF6310BAFB2_" + BString(ProcessID) + "_" + BString(ThreadID)))
    {
      return 0;
    }
    MxProcessConnectReady.Create("ConnectReady_585656DF-ADC4-48BC-AE19-7CF6310BAFB2_" + BString(ProcessID) + "_" + BString(ThreadID));
    MxProcessResultsReady.Create("ResultsReady_585656DF-ADC4-48BC-AE19-7CF6310BAFB2_" + BString(ProcessID) + "_" + BString(ThreadID));
    MxProcessResultReceived.Create("ResultRecv_585656DF-ADC4-48BC-AE19-7CF6310BAFB2_" + BString(ProcessID) + "_" + BString(ThreadID));
    MxConnected = 1;

    return 1;
  }

  // Called immediately after creating Elevate.exe.
  int WaitForProcess(DWORD Timeout = 10000)
  {
    if (!MxConnected)  return 0;

    // Wait for process connection readiness.
#ifdef _DEBUG
    UNREFERENCED_PARAMETER(Timeout);
    if (!MxProcessConnectReady.Wait())
#else
    if (!MxProcessConnectReady.Wait((UInt32)Timeout))
#endif
    {
      // Process wasn't started or something went horribly wrong.
      ::SetLastError(WAIT_TIMEOUT);
      MxProcessResultReceived.FireEvent();
      MxConnected = 0;
      return 0;
    }

    // Wait for a connection on the named pipe server.
    if (!MxProcessComm.GetClient(MxProcessCommClient))
    {
      ::SetLastError(WAIT_TIMEOUT);
      MxProcessResultReceived.FireEvent();
      MxConnected = 0;
      return 0;
    }

    return 1;
  }

  // Sends a data packet to Elevate.exe.
  DWORD SendData(BString Command, BString Data, DWORD Timeout = 10000)
  {
    if (!MxConnected)  return 0;

    BStorage TempStorage;

    // Pack and send the data to Elevate.exe.
    TempStorage.SetStorageType(BStorage::StoreBlock);
    TempStorage.AddData(Command);
    TempStorage.AddData(Data);
    TempStorage.Finalize();
    Data = TempStorage.Save();
    MxProcessCommClient.WritePacket(Data);

    // Retrieve process information from Elevate.exe.
#ifdef _DEBUG
    UNREFERENCED_PARAMETER(Timeout);
    if (!MxProcessResultsReady.Wait())
#else
    if (!MxProcessResultsReady.Wait((UInt32)Timeout))
#endif
    {
      // Process wasn't started or something went horribly wrong.
      ::SetLastError(WAIT_TIMEOUT);
      MxProcessResultReceived.FireEvent();
      MxConnected = 0;
      return 0;
    }
    MxProcessCommClient.ReadPacket(MxResult);
    if (!MxResult.GetSize())  MxProcessResultReceived.FireEvent();

    return (DWORD)MxResult.GetSize();
  }

  // Copies the data to the target buffer.
  int GetData(LPVOID Result)
  {
    if (!MxConnected)  return 0;

    memcpy(Result, MxResult.RawBytes(), MxResult.GetSize());
    MxResult.Empty();

    return 1;
  }

  // Notifies Elevate.exe that the data has been received.
  int SendFinalize()
  {
    if (!MxConnected)  return 0;

    MxProcessResultReceived.FireEvent();

    return 1;
  }

  // Gracefully closes the link to Elevate.exe.
  int Destroy()
  {
    if (!MxConnected)  return 0;

    SendData("SelfTerminate", "");
    SendFinalize();
    MxConnected = 0;

    return 1;
  }

private:
  int MxConnected;
  Pipe MxProcessComm, MxProcessCommClient;
  FastEvent MxProcessConnectReady, MxProcessResultsReady, MxProcessResultReceived;
  BString MxResult;
};

// Creates Elevate.exe and returns a persistent communication link.
// This allows for all sorts of possibilities:
//   - Creating multiple elevated processes from the non-elevated
//     process using only one elevation dialog.
//   - Loading DLLs that execute API functions in the elevated process space.
//   - Loading multiple Hook DLLs.
//   - Using DLLs that display dialogs in the elevated process space
//     and communicate results and data with the non-elevated app.
extern HANDLE __stdcall WINAPI Link_Create()
{
  UInt64 ProcessID, ThreadID;
  BString Filename, Directory, Parameters;
  SHELLEXECUTEINFOW TempInfo = {0};

  ProcessID = Process::CurrProcessID();
  ThreadID = Thread::CurrThreadID();
  Directory = System::ANSI2Unicode(System::AppDirectory(""));
  Filename = System::ANSI2Unicode(System::AppDirectory("") + "Elevate.exe");
  Parameters = System::ANSI2Unicode(BString(ProcessID) + " " + BString(ThreadID));
  TempInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
  TempInfo.fMask = 0;
  TempInfo.hwnd = NULL;
  TempInfo.lpVerb = L"runas";
  TempInfo.lpFile = (LPWSTR)*Filename;
  TempInfo.lpParameters = (LPWSTR)*Parameters;
  TempInfo.lpDirectory = (LPWSTR)*Directory;
  TempInfo.nShow = SW_NORMAL;

  ElevatedLink *LinkPtr = new ElevatedLink;
  if (LinkPtr->Create(ProcessID, ThreadID))
  {
    // NOTE:  ShellExecuteEx() returns only when Elevate.exe has been started.
    if (::ShellExecuteExW(&TempInfo))
    {
      if (LinkPtr->WaitForProcess())  return (HANDLE)LinkPtr;
    }
  }

  delete LinkPtr;

  return NULL;
}

// Special functions that create Elevate.exe at a medium IL as a different user.
// Then Elevate.exe turns around, elevates another Elevate.exe instance, and then exits.
// The resulting Elevate.exe runs elevated as the different user.
// Returns a persistent communication link to Elevate.exe.
extern HANDLE __stdcall WINAPI Link_CreateAsUser(HANDLE hToken)
{
  UInt64 ProcessID, ThreadID;
  BString Filename, Directory, Parameters;
  SECURITY_ATTRIBUTES SecurityAttr = {0};
  LPWCH Environ;
  STARTUPINFOW StartInfo = {0};
  PROCESS_INFORMATION ProcessInfo = {0};
  BOOL Result;
  DWORD ExitCode;

  ProcessID = Process::CurrProcessID();
  ThreadID = Thread::CurrThreadID();
  Directory = System::ANSI2Unicode(System::AppDirectory(""));
  Filename = System::ANSI2Unicode(System::AppDirectory("") + "Elevate.exe");
  Parameters = System::ANSI2Unicode("\"" + Filename + "\" link " + BString(ProcessID) + " " + BString(ThreadID));
  SecurityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  SecurityAttr.bInheritHandle = TRUE;
  SecurityAttr.lpSecurityDescriptor = NULL;
  StartInfo.cb = sizeof(STARTUPINFOW);
  StartInfo.wShowWindow = SW_SHOWDEFAULT;

  ElevatedLink *LinkPtr = new ElevatedLink;
  if (LinkPtr->Create(ProcessID, ThreadID))
  {
    // Create the process as a different user.
    Environ = GetEnvironmentStringsW();
    Result = ::CreateProcessAsUserW(hToken, (LPWSTR)*Filename, (LPWSTR)*Parameters,
                                    &SecurityAttr, &SecurityAttr, FALSE,
                                    0, Environ, (LPWSTR)*Directory,
                                    &StartInfo, &ProcessInfo);
    ::FreeEnvironmentStringsW(Environ);
    if (Result)
    {
      // Wait for Elevate.exe to finish.
      ::CloseHandle(ProcessInfo.hThread);
      if (::WaitForSingleObject(ProcessInfo.hProcess, INFINITE) != WAIT_OBJECT_0)  CloseHandle(ProcessInfo.hProcess);
      else
      {
        Result = ::GetExitCodeProcess(ProcessInfo.hProcess, &ExitCode);
        CloseHandle(ProcessInfo.hProcess);
        if (Result && !ExitCode)
        {
          if (LinkPtr->WaitForProcess())  return (HANDLE)LinkPtr;
        }
      }
    }
  }

  delete LinkPtr;

  return NULL;
}

extern HANDLE __stdcall WINAPI Link_CreateWithLogon(
  LPCWSTR lpUsername,
  LPCWSTR lpDomain,
  LPCWSTR lpPassword,
  DWORD dwLogonFlags)
{
  UInt64 ProcessID, ThreadID;
  BString Filename, Directory, Parameters;
  SECURITY_ATTRIBUTES SecurityAttr = {0};
  LPWCH Environ;
  STARTUPINFOW StartInfo = {0};
  PROCESS_INFORMATION ProcessInfo = {0};
  BOOL Result;
  DWORD ExitCode;

  ProcessID = Process::CurrProcessID();
  ThreadID = Thread::CurrThreadID();
  Directory = System::ANSI2Unicode(System::AppDirectory(""));
  Filename = System::ANSI2Unicode(System::AppDirectory("") + "Elevate.exe");
  Parameters = System::ANSI2Unicode("\"" + Filename + "\" link " + BString(ProcessID) + " " + BString(ThreadID));
  SecurityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  SecurityAttr.bInheritHandle = TRUE;
  SecurityAttr.lpSecurityDescriptor = NULL;
  StartInfo.cb = sizeof(STARTUPINFOW);
  StartInfo.wShowWindow = SW_SHOWDEFAULT;

  ElevatedLink *LinkPtr = new ElevatedLink;
  if (LinkPtr->Create(ProcessID, ThreadID))
  {
    // Create the process as a different user.
    Environ = GetEnvironmentStringsW();
    Result = ::CreateProcessWithLogonW(lpUsername, lpDomain, lpPassword,
                                       dwLogonFlags, (LPWSTR)*Filename, (LPWSTR)*Parameters,
                                       0, Environ, (LPWSTR)*Directory,
                                       &StartInfo, &ProcessInfo);
    ::FreeEnvironmentStringsW(Environ);
    if (Result)
    {
      // Wait for Elevate.exe to finish.
      ::CloseHandle(ProcessInfo.hThread);
      if (::WaitForSingleObject(ProcessInfo.hProcess, INFINITE) != WAIT_OBJECT_0)  CloseHandle(ProcessInfo.hProcess);
      else
      {
        Result = ::GetExitCodeProcess(ProcessInfo.hProcess, &ExitCode);
        CloseHandle(ProcessInfo.hProcess);
        if (Result && !ExitCode)
        {
          if (LinkPtr->WaitForProcess())  return (HANDLE)LinkPtr;
        }
      }
    }
  }

  delete LinkPtr;

  return NULL;
}

extern HANDLE __stdcall WINAPI Link_CreateWithToken(HANDLE hToken, DWORD dwLogonFlags)
{
  UInt64 ProcessID, ThreadID;
  BString Filename, Directory, Parameters;
  SECURITY_ATTRIBUTES SecurityAttr = {0};
  LPWCH Environ;
  STARTUPINFOW StartInfo = {0};
  PROCESS_INFORMATION ProcessInfo = {0};
  BOOL Result;
  DWORD ExitCode;

  ProcessID = Process::CurrProcessID();
  ThreadID = Thread::CurrThreadID();
  Directory = System::ANSI2Unicode(System::AppDirectory(""));
  Filename = System::ANSI2Unicode(System::AppDirectory("") + "Elevate.exe");
  Parameters = System::ANSI2Unicode("\"" + Filename + "\" link " + BString(ProcessID) + " " + BString(ThreadID));
  SecurityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  SecurityAttr.bInheritHandle = TRUE;
  SecurityAttr.lpSecurityDescriptor = NULL;
  StartInfo.cb = sizeof(STARTUPINFOW);
  StartInfo.wShowWindow = SW_SHOWDEFAULT;

  ElevatedLink *LinkPtr = new ElevatedLink;
  if (LinkPtr->Create(ProcessID, ThreadID))
  {
    // Create the process as a different user.
    Environ = GetEnvironmentStringsW();
    Result = ::CreateProcessWithTokenW(hToken, dwLogonFlags, (LPWSTR)*Filename, (LPWSTR)*Parameters,
                                       0, Environ, (LPWSTR)*Directory,
                                       &StartInfo, &ProcessInfo);
    ::FreeEnvironmentStringsW(Environ);
    if (Result)
    {
      // Wait for Elevate.exe to finish.
      ::CloseHandle(ProcessInfo.hThread);
      if (::WaitForSingleObject(ProcessInfo.hProcess, INFINITE) != WAIT_OBJECT_0)  CloseHandle(ProcessInfo.hProcess);
      else
      {
        Result = ::GetExitCodeProcess(ProcessInfo.hProcess, &ExitCode);
        CloseHandle(ProcessInfo.hProcess);
        if (Result && !ExitCode)
        {
          if (LinkPtr->WaitForProcess())  return (HANDLE)LinkPtr;
        }
      }
    }
  }

  delete LinkPtr;

  return NULL;
}

// Stops Elevate.exe and cleans up and destroys the link.
extern BOOL __stdcall WINAPI Link_Destroy(HANDLE hLink)
{
  if (hLink == NULL)  return FALSE;

  ElevatedLink *LinkPtr = (ElevatedLink *)hLink;
  delete LinkPtr;

  return TRUE;
}

// GenerateElevatedPack() generates a package of data.
// The package is serialized and sent to Elevate.exe.
void GenerateElevatedPackA(
  CreateProcessElevatedPack &Result,
  BString TargetFunc,
  LPCSTR lpApplicationName,
  LPCSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCSTR lpCurrentDirectory,
  LPSTARTUPINFOA lpStartupInfo,
  LPSHELLEXECUTEINFOA lpExecInfo)
{
  Result.MxCurrProcessID = Process::CurrProcessID();
  Result.MxCurrThreadID = Thread::CurrThreadID();
  Result.MxTargetFunc = TargetFunc;
  if (lpApplicationName)
  {
    Result.MxApplicationNameExists = 1;
    Result.MxApplicationName = lpApplicationName;
  }
  if (lpCommandLine)
  {
    Result.MxCommandLineExists = 1;
    Result.MxCommandLine = lpCommandLine;
  }
  if (lpProcessAttributes)
  {
    Result.MxProcAttrExists = 1;
    Result.MxProcAttrInherit = lpProcessAttributes->bInheritHandle;
  }
  if (lpThreadAttributes)
  {
    Result.MxThreadAttrExists = 1;
    Result.MxThreadAttrInherit = lpThreadAttributes->bInheritHandle;
  }
  Result.MxInheritHandles = bInheritHandles;
  Result.MxCreationFlags = FixCreationFlags(dwCreationFlags);
  ProcessEnvironment(Result.MxEnvironment, Result.MxCreationFlags, lpEnvironment);
  if (lpCurrentDirectory)  Result.MxCurrDir = lpCurrentDirectory;
  else  Result.MxCurrDir = System::CurrDirectory();

  if (lpStartupInfo)
  {
    if (lpStartupInfo->lpDesktop)
    {
      Result.MxStartupInfo.MxDesktopExists = 1;
      Result.MxStartupInfo.MxDesktop = lpStartupInfo->lpDesktop;
    }
    if (lpStartupInfo->lpTitle)
    {
      Result.MxStartupInfo.MxTitleExists = 1;
      Result.MxStartupInfo.MxTitle = lpStartupInfo->lpTitle;
    }

    Result.MxStartupInfo.MxX = lpStartupInfo->dwX;
    Result.MxStartupInfo.MxY = lpStartupInfo->dwY;
    Result.MxStartupInfo.MxXSize = lpStartupInfo->dwXSize;
    Result.MxStartupInfo.MxYSize = lpStartupInfo->dwYSize;
    Result.MxStartupInfo.MxXCountChars = lpStartupInfo->dwXCountChars;
    Result.MxStartupInfo.MxYCountChars = lpStartupInfo->dwYCountChars;
    Result.MxStartupInfo.MxFillAttr = lpStartupInfo->dwFillAttribute;
    Result.MxStartupInfo.MxFlags = lpStartupInfo->dwFlags;
    Result.MxStartupInfo.MxShowWindow = lpStartupInfo->wShowWindow;
    if (Result.MxStartupInfo.MxFlags & STARTF_USESTDHANDLES)
    {
      if (lpStartupInfo->hStdInput != NULL && lpStartupInfo->hStdInput != INVALID_HANDLE_VALUE)
      {
        Result.MxStartupInfo.MxStdinExists = 1;
        Result.MxStartupInfo.MxStdin = (LPCSTR)lpStartupInfo->hStdInput;
      }
      if (lpStartupInfo->hStdOutput != NULL && lpStartupInfo->hStdOutput != INVALID_HANDLE_VALUE)
      {
        Result.MxStartupInfo.MxStdoutExists = 1;
        Result.MxStartupInfo.MxStdout = (LPCSTR)lpStartupInfo->hStdOutput;
      }
      if (lpStartupInfo->hStdError != NULL && lpStartupInfo->hStdError != INVALID_HANDLE_VALUE)
      {
        Result.MxStartupInfo.MxStderrExists = 1;
        Result.MxStartupInfo.MxStderr = (LPCSTR)lpStartupInfo->hStdError;
      }
    }
  }

  if (lpExecInfo)
  {
    lpExecInfo->fMask = lpExecInfo->fMask & ~SEE_MASK_ICON;
    Result.MxShellExecuteInfo.MxHWnd = (UInt64)lpExecInfo->hwnd;
    if (lpExecInfo->lpVerb)  Result.MxShellExecuteInfo.MxVerb = lpExecInfo->lpVerb;
    Result.MxShellExecuteInfo.MxShowWindow = lpExecInfo->nShow;
    if (((lpExecInfo->fMask & SEE_MASK_IDLIST) || (lpExecInfo->fMask & SEE_MASK_INVOKEIDLIST)) && lpExecInfo->lpIDList)
    {
      // Serialize the PIDL.
      Result.MxShellExecuteInfo.MxIDList = BString((SByte *)lpExecInfo->lpIDList, PIDL_GetSize((LPITEMIDLIST)lpExecInfo->lpIDList));
    }
    else
    {
      lpExecInfo->fMask = lpExecInfo->fMask & ~SEE_MASK_IDLIST;
      lpExecInfo->fMask = lpExecInfo->fMask & ~SEE_MASK_INVOKEIDLIST;
    }
    if ((lpExecInfo->fMask & SEE_MASK_CLASSNAME) && lpExecInfo->lpClass)
    {
      Result.MxShellExecuteInfo.MxClass = lpExecInfo->lpClass;
    }
    else  lpExecInfo->fMask = lpExecInfo->fMask & ~SEE_MASK_CLASSNAME;
    if ((lpExecInfo->fMask & SEE_MASK_CLASSKEY) && lpExecInfo->hkeyClass)
    {
      HKEY_Pack *Data = (HKEY_Pack *)(lpExecInfo->hkeyClass);
      Result.MxShellExecuteInfo.MxKeyClass = *Data;
    }
    else  lpExecInfo->fMask = lpExecInfo->fMask & ~SEE_MASK_CLASSKEY;
    if (lpExecInfo->fMask & SEE_MASK_HOTKEY)  Result.MxShellExecuteInfo.MxHotkey = lpExecInfo->dwHotKey;
    if ((lpExecInfo->fMask & SEE_MASK_HMONITOR) && lpExecInfo->hMonitor)
    {
      MONITORINFO TempInfo;
      TempInfo.cbSize = sizeof(MONITORINFO);
      GetMonitorInfo((HMONITOR)lpExecInfo->hMonitor, &TempInfo);
      Result.MxShellExecuteInfo.MxMonitorPosX = (TempInfo.rcMonitor.left + TempInfo.rcMonitor.right + 1) / 2;
      Result.MxShellExecuteInfo.MxMonitorPosY = (TempInfo.rcMonitor.top + TempInfo.rcMonitor.bottom + 1) / 2;
    }
    else  lpExecInfo->fMask = lpExecInfo->fMask & ~SEE_MASK_HMONITOR;
    Result.MxShellExecuteInfo.MxMask = lpExecInfo->fMask;
  }
}

void GenerateElevatedPackW(
  CreateProcessElevatedPack &Result, 
  BString TargetFunc,
  LPCWSTR lpApplicationName,
  LPCWSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCWSTR lpCurrentDirectory,
  LPSTARTUPINFOW lpStartupInfo,
  LPSHELLEXECUTEINFOW lpExecInfo)
{
  Result.MxCurrProcessID = Process::CurrProcessID();
  Result.MxCurrThreadID = Thread::CurrThreadID();
  Result.MxTargetFunc = TargetFunc;
  if (lpApplicationName)
  {
    Result.MxApplicationNameExists = 1;
    Result.MxApplicationName = System::Unicode2BString(lpApplicationName);
  }
  if (lpCommandLine)
  {
    Result.MxCommandLineExists = 1;
    Result.MxCommandLine = System::Unicode2BString(lpCommandLine);
  }
  if (lpProcessAttributes)
  {
    Result.MxProcAttrExists = 1;
    Result.MxProcAttrInherit = lpProcessAttributes->bInheritHandle;
  }
  if (lpThreadAttributes)
  {
    Result.MxThreadAttrExists = 1;
    Result.MxThreadAttrInherit = lpThreadAttributes->bInheritHandle;
  }
  Result.MxInheritHandles = bInheritHandles;
  Result.MxCreationFlags = FixCreationFlags(dwCreationFlags);
  ProcessEnvironment(Result.MxEnvironment, Result.MxCreationFlags, lpEnvironment);
  if (lpCurrentDirectory)  Result.MxCurrDir = System::Unicode2BString(lpCurrentDirectory);
  else if (lpExecInfo == NULL)  Result.MxCurrDir = System::ANSI2Unicode(System::CurrDirectory());

  if (lpStartupInfo)
  {
    if (lpStartupInfo->lpDesktop)
    {
      Result.MxStartupInfo.MxDesktopExists = 1;
      Result.MxStartupInfo.MxDesktop = System::Unicode2BString(lpStartupInfo->lpDesktop);
    }
    if (lpStartupInfo->lpTitle)
    {
      Result.MxStartupInfo.MxTitleExists = 1;
      Result.MxStartupInfo.MxTitle = System::Unicode2BString(lpStartupInfo->lpTitle);
    }

    Result.MxStartupInfo.MxX = lpStartupInfo->dwX;
    Result.MxStartupInfo.MxY = lpStartupInfo->dwY;
    Result.MxStartupInfo.MxXSize = lpStartupInfo->dwXSize;
    Result.MxStartupInfo.MxYSize = lpStartupInfo->dwYSize;
    Result.MxStartupInfo.MxXCountChars = lpStartupInfo->dwXCountChars;
    Result.MxStartupInfo.MxYCountChars = lpStartupInfo->dwYCountChars;
    Result.MxStartupInfo.MxFillAttr = lpStartupInfo->dwFillAttribute;
    Result.MxStartupInfo.MxFlags = lpStartupInfo->dwFlags;
    Result.MxStartupInfo.MxShowWindow = lpStartupInfo->wShowWindow;
    if (Result.MxStartupInfo.MxFlags & STARTF_USESTDHANDLES)
    {
      if (lpStartupInfo->hStdInput != NULL && lpStartupInfo->hStdInput != INVALID_HANDLE_VALUE)
      {
        Result.MxStartupInfo.MxStdinExists = 1;
        Result.MxStartupInfo.MxStdin = System::Unicode2BString((LPCWSTR)lpStartupInfo->hStdInput);
        if (Result.MxStartupInfo.MxStdin.GetSize() <= 2)  Result.MxStartupInfo.MxStdin.Empty();
      }
      if (lpStartupInfo->hStdOutput != NULL && lpStartupInfo->hStdOutput != INVALID_HANDLE_VALUE)
      {
        Result.MxStartupInfo.MxStdoutExists = 1;
        Result.MxStartupInfo.MxStdout = System::Unicode2BString((LPCWSTR)lpStartupInfo->hStdOutput);
        if (Result.MxStartupInfo.MxStdout.GetSize() <= 2)  Result.MxStartupInfo.MxStdout.Empty();
      }
      if (lpStartupInfo->hStdError != NULL && lpStartupInfo->hStdError != INVALID_HANDLE_VALUE)
      {
        Result.MxStartupInfo.MxStderrExists = 1;
        Result.MxStartupInfo.MxStderr = System::Unicode2BString((LPCWSTR)lpStartupInfo->hStdError);
        if (Result.MxStartupInfo.MxStderr.GetSize() <= 2)  Result.MxStartupInfo.MxStderr.Empty();
      }
    }
  }

  if (lpExecInfo)
  {
    lpExecInfo->fMask = lpExecInfo->fMask & ~SEE_MASK_ICON;
    Result.MxShellExecuteInfo.MxHWnd = (UInt64)lpExecInfo->hwnd;
    if (lpExecInfo->lpVerb)  Result.MxShellExecuteInfo.MxVerb = System::Unicode2BString(lpExecInfo->lpVerb);
    Result.MxShellExecuteInfo.MxShowWindow = lpExecInfo->nShow;
    if (((lpExecInfo->fMask & SEE_MASK_IDLIST) || (lpExecInfo->fMask & SEE_MASK_INVOKEIDLIST)) && lpExecInfo->lpIDList)
    {
      // Serialize the PIDL.
      Result.MxShellExecuteInfo.MxIDList = BString((SByte *)lpExecInfo->lpIDList, PIDL_GetSize((LPITEMIDLIST)lpExecInfo->lpIDList));
    }
    else
    {
      lpExecInfo->fMask = lpExecInfo->fMask & ~SEE_MASK_IDLIST;
      lpExecInfo->fMask = lpExecInfo->fMask & ~SEE_MASK_INVOKEIDLIST;
    }
    if ((lpExecInfo->fMask & SEE_MASK_CLASSNAME) && lpExecInfo->lpClass)
    {
      Result.MxShellExecuteInfo.MxClass = System::Unicode2BString(lpExecInfo->lpClass);
    }
    else  lpExecInfo->fMask = lpExecInfo->fMask & ~SEE_MASK_CLASSNAME;
    if ((lpExecInfo->fMask & SEE_MASK_CLASSKEY) && lpExecInfo->hkeyClass)
    {
      HKEY_Pack *Data = (HKEY_Pack *)(lpExecInfo->hkeyClass);
      Result.MxShellExecuteInfo.MxKeyClass = *Data;
    }
    else  lpExecInfo->fMask = lpExecInfo->fMask & ~SEE_MASK_CLASSKEY;
    if (lpExecInfo->fMask & SEE_MASK_HOTKEY)  Result.MxShellExecuteInfo.MxHotkey = lpExecInfo->dwHotKey;
    if ((lpExecInfo->fMask & SEE_MASK_HMONITOR) && lpExecInfo->hMonitor)
    {
      MONITORINFO TempInfo;
      TempInfo.cbSize = sizeof(MONITORINFO);
      GetMonitorInfo((HMONITOR)lpExecInfo->hMonitor, &TempInfo);
      Result.MxShellExecuteInfo.MxMonitorPosX = (TempInfo.rcMonitor.left + TempInfo.rcMonitor.right + 1) / 2;
      Result.MxShellExecuteInfo.MxMonitorPosY = (TempInfo.rcMonitor.top + TempInfo.rcMonitor.bottom + 1) / 2;
    }
    else  lpExecInfo->fMask = lpExecInfo->fMask & ~SEE_MASK_HMONITOR;
    Result.MxShellExecuteInfo.MxMask = lpExecInfo->fMask;
  }
}

// Serializes a package and sends it to Elevate.exe via a persistent link.
BOOL RunElevatedPack(
  HANDLE hLink,
  CreateProcessElevatedPack &PackData,
  LPPROCESS_INFORMATION lpProcessInformation,
  LPSHELLEXECUTEINFOW lpExecInfo)
{
  if (hLink == NULL)
  {
    ::SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  BString Data;
  DWORD TempSize;
  ElevatedLink *LinkPtr = (ElevatedLink *)hLink;

  PackData.Save(Data);
  TempSize = LinkPtr->SendData("CreateProcess", Data);
  if (!TempSize)
  {
    ::SetLastError(ERROR_PIPE_NOT_CONNECTED);
    return FALSE;
  }
  Data.SetSize((size_t)TempSize);
  if (!LinkPtr->GetData(Data.RawBytes()))
  {
    ::SetLastError(ERROR_PIPE_NOT_CONNECTED);
    return FALSE;
  }

  CreateProcessResultPack TempResultPack;

  if (!TempResultPack.Load(Data))
  {
    ::SetLastError(ERROR_INVALID_DATA);
    return FALSE;
  }
  if (lpProcessInformation)
  {
    lpProcessInformation->dwProcessId = (DWORD)TempResultPack.MxProcessID;
    lpProcessInformation->dwThreadId = (DWORD)TempResultPack.MxThreadID;
    lpProcessInformation->hProcess = ::OpenProcess(SYNCHRONIZE, TRUE, lpProcessInformation->dwProcessId);
    lpProcessInformation->hThread = ::OpenThread((PackData.MxCreationFlags & CREATE_SUSPENDED ? SYNCHRONIZE | THREAD_SUSPEND_RESUME : SYNCHRONIZE), TRUE, lpProcessInformation->dwThreadId);
  }
  if (lpExecInfo)
  {
    lpExecInfo->hInstApp = (HINSTANCE)TempResultPack.MxRetCode;
    if (PackData.MxShellExecuteInfo.MxMask & SEE_MASK_NOCLOSEPROCESS)
    {
      lpExecInfo->hProcess = ::OpenProcess(SYNCHRONIZE, TRUE, (DWORD)TempResultPack.MxProcessID);
    }
  }
  ::SetLastError((DWORD)TempResultPack.MxErrorCode);

  if (!LinkPtr->SendFinalize())
  {
    ::SetLastError(ERROR_PIPE_NOT_CONNECTED);
    return FALSE;
  }

  return (BOOL)(lpExecInfo ? (TempResultPack.MxRetCode >= 32) : TempResultPack.MxRetCode);
}

BOOL RunElevatedPack(
  HANDLE hLink,
  CreateProcessElevatedPack &PackData,
  LPSHELLEXECUTEINFOA lpExecInfo)
{
  BOOL Result;
  SHELLEXECUTEINFOW TempInfo = {0};

  Result = RunElevatedPack(hLink, PackData, NULL, &TempInfo);
  lpExecInfo->hInstApp = TempInfo.hInstApp;
  lpExecInfo->hProcess = TempInfo.hProcess;

  return Result;
}

// These functions use the persistent link to execute commands.
extern BOOL __stdcall WINAPI Link_CreateProcessA(
  HANDLE hLink,
  LPCSTR lpApplicationName,
  LPSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCSTR lpCurrentDirectory,
  LPSTARTUPINFOA lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation)
{
  CreateProcessElevatedPack TempPack;

  GenerateElevatedPackA(TempPack, "CreateProcessA", lpApplicationName,
                        lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                        bInheritHandles, dwCreationFlags, lpEnvironment,
                        lpCurrentDirectory, lpStartupInfo, NULL);

  return RunElevatedPack(hLink, TempPack, lpProcessInformation, NULL);
}

extern BOOL __stdcall WINAPI Link_CreateProcessW(
  HANDLE hLink,
  LPCWSTR lpApplicationName,
  LPWSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCWSTR lpCurrentDirectory,
  LPSTARTUPINFOW lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation)
{
  CreateProcessElevatedPack TempPack;

  GenerateElevatedPackW(TempPack, "CreateProcessW", lpApplicationName,
                        lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                        bInheritHandles, dwCreationFlags, lpEnvironment,
                        lpCurrentDirectory, lpStartupInfo, NULL);

  return RunElevatedPack(hLink, TempPack, lpProcessInformation, NULL);
}

extern BOOL __stdcall WINAPI Link_ShellExecuteExA(HANDLE hLink, LPSHELLEXECUTEINFOA lpExecInfo)
{
  CreateProcessElevatedPack TempPack;

  GenerateElevatedPackA(TempPack, "ShellExecuteExA",
                        (lpExecInfo ? lpExecInfo->lpFile : NULL),
                        (lpExecInfo ? lpExecInfo->lpParameters : NULL),
                        NULL, NULL, 0, 0, NULL,
                        (lpExecInfo ? lpExecInfo->lpDirectory : NULL),
                        NULL, lpExecInfo);

  return RunElevatedPack(hLink, TempPack, lpExecInfo);
}

extern BOOL __stdcall WINAPI Link_ShellExecuteExW(HANDLE hLink, LPSHELLEXECUTEINFOW lpExecInfo)
{
  CreateProcessElevatedPack TempPack;

  GenerateElevatedPackW(TempPack, "ShellExecuteExW",
                        (lpExecInfo ? lpExecInfo->lpFile : NULL),
                        (lpExecInfo ? lpExecInfo->lpParameters : NULL),
                        NULL, NULL, 0, 0, NULL,
                        (lpExecInfo ? lpExecInfo->lpDirectory : NULL),
                        NULL, lpExecInfo);

  return RunElevatedPack(hLink, TempPack, NULL, lpExecInfo);
}

extern HINSTANCE __stdcall WINAPI Link_ShellExecuteA(
  HANDLE hLink,
  HWND hwnd,
  LPSTR lpOperation,
  LPCSTR lpFile,
  LPCSTR lpParameters,
  LPCSTR lpDirectory,
  INT nShowCmd)
{
  SHELLEXECUTEINFOA ExecInfo = {0};
  CreateProcessElevatedPack TempPack;

  ExecInfo.cbSize = sizeof(LPSHELLEXECUTEINFOA);
  ExecInfo.hwnd = hwnd;
  ExecInfo.lpVerb = lpOperation;
  ExecInfo.nShow = nShowCmd;
  GenerateElevatedPackA(TempPack, "ShellExecuteA",
                        lpFile, lpParameters,
                        NULL, NULL, 0, 0, NULL, lpDirectory,
                        NULL, &ExecInfo);

  RunElevatedPack(hLink, TempPack, &ExecInfo);

  return ExecInfo.hInstApp;
}

extern HINSTANCE __stdcall WINAPI Link_ShellExecuteW(
  HANDLE hLink,
  HWND hwnd,
  LPWSTR lpOperation,
  LPCWSTR lpFile,
  LPCWSTR lpParameters,
  LPCWSTR lpDirectory,
  INT nShowCmd)
{
  SHELLEXECUTEINFOW ExecInfo = {0};
  CreateProcessElevatedPack TempPack;

  ExecInfo.cbSize = sizeof(LPSHELLEXECUTEINFOW);
  ExecInfo.hwnd = hwnd;
  ExecInfo.lpVerb = lpOperation;
  ExecInfo.nShow = nShowCmd;
  GenerateElevatedPackW(TempPack, "ShellExecuteW",
                        lpFile, lpParameters,
                        NULL, NULL, 0, 0, NULL, lpDirectory,
                        NULL, &ExecInfo);

  RunElevatedPack(hLink, TempPack, NULL, &ExecInfo);

  return ExecInfo.hInstApp;
}


DWORD Link_LoadLibrary(HANDLE hLink, BString Filename)
{
  if (hLink == NULL)
  {
    ::SetLastError(ERROR_INVALID_HANDLE);
    return 0;
  }

  BString Data;
  DWORD TempSize;
  ElevatedLink *LinkPtr = (ElevatedLink *)hLink;

  TempSize = LinkPtr->SendData("LoadLibrary", Filename);
  if (!TempSize)
  {
    ::SetLastError(ERROR_PIPE_NOT_CONNECTED);
    return 0;
  }
  Data.SetSize((size_t)TempSize);
  if (!LinkPtr->GetData(Data.RawBytes()))
  {
    ::SetLastError(ERROR_PIPE_NOT_CONNECTED);
    return 0;
  }

  return (DWORD)(UInt64)Data;
}

extern DWORD __stdcall WINAPI Link_LoadLibraryA(HANDLE hLink, LPCSTR lpFileName)
{
  return Link_LoadLibrary(hLink, System::ANSI2Unicode(lpFileName));
}

extern DWORD __stdcall WINAPI Link_LoadLibraryW(HANDLE hLink, LPCWSTR lpFileName)
{
  return Link_LoadLibrary(hLink, System::Unicode2BString(lpFileName));
}

extern DWORD __stdcall WINAPI Link_SendData(
  HANDLE hLink,
  DWORD dwLibNum,
  LPCVOID lpData,
  DWORD dwDataSize,
  DWORD dwTimeout)
{
  if (hLink == NULL)
  {
    ::SetLastError(ERROR_INVALID_HANDLE);
    return 0;
  }

  BStorage TempStorage;
  DWORD TempSize;
  ElevatedLink *LinkPtr = (ElevatedLink *)hLink;

  TempStorage.SetStorageType(BStorage::StoreBlock);
  TempStorage.AddData(dwLibNum);
  TempStorage.AddData(BString((SByte *)lpData, (size_t)dwDataSize));
  TempStorage.Finalize();
  TempSize = LinkPtr->SendData("LibFunc", TempStorage.Save(), dwTimeout);
  if (!TempSize)
  {
    ::SetLastError(ERROR_PIPE_NOT_CONNECTED);
    return 0;
  }

  return TempSize;
}

extern BOOL __stdcall WINAPI Link_GetData(HANDLE hLink, LPVOID lpData)
{
  if (hLink == NULL)
  {
    ::SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  ElevatedLink *LinkPtr = (ElevatedLink *)hLink;
  if (!LinkPtr->GetData(lpData))
  {
    ::SetLastError(ERROR_PIPE_NOT_CONNECTED);
    return FALSE;
  }

  return TRUE;
}

extern BOOL __stdcall WINAPI Link_SendFinalize(HANDLE hLink)
{
  if (hLink == NULL)
  {
    ::SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  ElevatedLink *LinkPtr = (ElevatedLink *)hLink;
  if (!LinkPtr->SendFinalize())
  {
    ::SetLastError(ERROR_PIPE_NOT_CONNECTED);
    return FALSE;
  }

  return TRUE;
}


// Functions that simplify the process of elevating an executable.
// Creates a persistent link, starts the executable, and closes the link.
extern BOOL __stdcall WINAPI CreateProcessElevatedA(
  LPCSTR lpApplicationName,
  LPSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCSTR lpCurrentDirectory,
  LPSTARTUPINFOA lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation)
{
  HANDLE hLink = Link_Create();
  if (hLink == NULL)  return FALSE;

  BOOL Result;
  Result = Link_CreateProcessA(hLink, lpApplicationName, lpCommandLine,
                               lpProcessAttributes, lpThreadAttributes, bInheritHandles,
                               dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                               lpStartupInfo, lpProcessInformation);

  Link_Destroy(hLink);

  return Result;
}

extern BOOL __stdcall WINAPI CreateProcessElevatedW(
  LPCWSTR lpApplicationName,
  LPWSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCWSTR lpCurrentDirectory,
  LPSTARTUPINFOW lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation)
{
  HANDLE hLink = Link_Create();
  if (hLink == NULL)  return FALSE;

  BOOL Result;
  Result = Link_CreateProcessW(hLink, lpApplicationName, lpCommandLine,
                               lpProcessAttributes, lpThreadAttributes, bInheritHandles,
                               dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                               lpStartupInfo, lpProcessInformation);

  Link_Destroy(hLink);

  return Result;
}

extern BOOL __stdcall WINAPI CreateProcessAsUserElevatedA(
  HANDLE hToken,
  LPCSTR lpApplicationName,
  LPSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCSTR lpCurrentDirectory,
  LPSTARTUPINFOA lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation)
{
  HANDLE hLink = Link_CreateAsUser(hToken);
  if (hLink == NULL)  return FALSE;

  BOOL Result;
  Result = Link_CreateProcessA(hLink, lpApplicationName, lpCommandLine,
                               lpProcessAttributes, lpThreadAttributes, bInheritHandles,
                               dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                               lpStartupInfo, lpProcessInformation);

  Link_Destroy(hLink);

  return Result;
}

extern BOOL __stdcall WINAPI CreateProcessAsUserElevatedW(
  HANDLE hToken,
  LPCWSTR lpApplicationName,
  LPWSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCWSTR lpCurrentDirectory,
  LPSTARTUPINFOW lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation)
{
  HANDLE hLink = Link_CreateAsUser(hToken);
  if (hLink == NULL)  return FALSE;

  BOOL Result;
  Result = Link_CreateProcessW(hLink, lpApplicationName, lpCommandLine,
                               lpProcessAttributes, lpThreadAttributes, bInheritHandles,
                               dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                               lpStartupInfo, lpProcessInformation);

  Link_Destroy(hLink);

  return Result;
}

extern BOOL __stdcall WINAPI CreateProcessWithLogonElevatedW(
  LPCWSTR lpUsername,
  LPCWSTR lpDomain,
  LPCWSTR lpPassword,
  DWORD dwLogonFlags,
  LPCWSTR lpApplicationName,
  LPWSTR lpCommandLine,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCWSTR lpCurrentDirectory,
  LPSTARTUPINFOW lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInfo)
{
  HANDLE hLink = Link_CreateWithLogon(lpUsername, lpDomain, lpPassword, dwLogonFlags);
  if (hLink == NULL)  return FALSE;

  BOOL Result;
  SECURITY_ATTRIBUTES SecurityAttr = {0};
  SecurityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  SecurityAttr.bInheritHandle = TRUE;
  SecurityAttr.lpSecurityDescriptor = NULL;
  Result = Link_CreateProcessW(hLink, lpApplicationName, lpCommandLine,
                               &SecurityAttr, &SecurityAttr, (lpStartupInfo ? (lpStartupInfo->dwFlags & STARTF_USESTDHANDLES ? TRUE : FALSE) : FALSE),
                               dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                               lpStartupInfo, lpProcessInfo);

  Link_Destroy(hLink);

  return Result;
}

extern BOOL __stdcall WINAPI CreateProcessWithTokenElevatedW(
  HANDLE hToken,
  DWORD dwLogonFlags,
  LPCWSTR lpApplicationName,
  LPWSTR lpCommandLine,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCWSTR lpCurrentDirectory,
  LPSTARTUPINFOW lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInfo)
{
  HANDLE hLink = Link_CreateWithToken(hToken, dwLogonFlags);
  if (hLink == NULL)  return FALSE;

  BOOL Result;
  SECURITY_ATTRIBUTES SecurityAttr = {0};
  SecurityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  SecurityAttr.bInheritHandle = TRUE;
  SecurityAttr.lpSecurityDescriptor = NULL;
  Result = Link_CreateProcessW(hLink, lpApplicationName, lpCommandLine,
                               &SecurityAttr, &SecurityAttr, (lpStartupInfo ? (lpStartupInfo->dwFlags & STARTF_USESTDHANDLES ? TRUE : FALSE) : FALSE),
                               dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                               lpStartupInfo, lpProcessInfo);

  Link_Destroy(hLink);

  return Result;
}

extern HINSTANCE __stdcall WINAPI ShellExecuteElevatedA(
  HWND hwnd,
  LPSTR lpOperation,
  LPCSTR lpFile,
  LPCSTR lpParameters,
  LPCSTR lpDirectory,
  INT nShowCmd)
{
  HANDLE hLink = Link_Create();
  if (hLink == NULL)  return FALSE;

  HINSTANCE Result;
  Result = Link_ShellExecuteA(hLink, hwnd, lpOperation, lpFile,
                              lpParameters, lpDirectory, nShowCmd);

  Link_Destroy(hLink);

  return Result;
}

extern HINSTANCE __stdcall WINAPI ShellExecuteElevatedW(
  HWND hwnd,
  LPWSTR lpOperation,
  LPCWSTR lpFile,
  LPCWSTR lpParameters,
  LPCWSTR lpDirectory,
  INT nShowCmd)
{
  HANDLE hLink = Link_Create();
  if (hLink == NULL)  return FALSE;

  HINSTANCE Result;
  Result = Link_ShellExecuteW(hLink, hwnd, lpOperation, lpFile,
                              lpParameters, lpDirectory, nShowCmd);

  Link_Destroy(hLink);

  return Result;
}

extern BOOL __stdcall WINAPI ShellExecuteExElevatedA(LPSHELLEXECUTEINFOA lpExecInfo)
{
  HANDLE hLink = Link_Create();
  if (hLink == NULL)  return FALSE;

  BOOL Result;
  Result = Link_ShellExecuteExA(hLink, lpExecInfo);

  Link_Destroy(hLink);

  return Result;
}

extern BOOL __stdcall WINAPI ShellExecuteExElevatedW(LPSHELLEXECUTEINFOW lpExecInfo)
{
  HANDLE hLink = Link_Create();
  if (hLink == NULL)  return FALSE;

  BOOL Result;
  Result = Link_ShellExecuteExW(hLink, lpExecInfo);

  Link_Destroy(hLink);

  return Result;
}

extern BOOL __stdcall WINAPI IsUserAnAdmin()
{
  DLL_Shell32_Set();

  if (DLL_Test(ShIsUserAnAdmin))
  {
    return DLL_ShIsUserAnAdmin();
  }

  BOOL RetCode = FALSE;
  SID_IDENTIFIER_AUTHORITY NTAuth = SECURITY_NT_AUTHORITY;
  PSID AdminSID;
  if (::AllocateAndInitializeSid(&NTAuth, 2,
                                 SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
                                 0, 0, 0, 0, 0, 0, &AdminSID))
  {
    if (!::CheckTokenMembership(NULL, AdminSID, &RetCode))  RetCode = FALSE;
    ::FreeSid(AdminSID);
  }

  return RetCode;
}
