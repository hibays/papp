#define _WIN32_WINNT  0x600
#define WINVER        0x600
#include "CoreLib.h"
//#include <vld.h>
#include "Elevate.h"

typedef DWORD (WINAPI *DLLElevatedLink_SendDataType)(LPCVOID, DWORD);
typedef int (WINAPI *DLLElevatedLink_GetDataType)(LPVOID);
typedef void (WINAPI *DLLElevatedLink_SendFinalizeType)();

class LibFunc
{
public:
  DLLElevatedLink_SendDataType DLL_ElevatedLink_SendData;
  DLLElevatedLink_GetDataType DLL_ElevatedLink_GetData;
  DLLElevatedLink_SendFinalizeType DLL_ElevatedLink_SendFinalize;
};

int DumpSyntax()
{
  size_t x, y;
  BString Message;

  Message = "Error:  Elevate incorrectly called.\n";
  y = GxApp.MxArgs.GetSize();
  Message += "Parameters sent (" + BString(y) + "):\n";
  for (x = 0; x < y; x++)  Message += "  Arg[" + BString(x) + "] = '" + GxApp.MxArgs[x] + "'\n";
  ::MessageBox(::GetForegroundWindow(), *Message, "Elevation error", MB_OK | MB_ICONERROR);

  return 1;
}

HANDLE GetPipeOrStdHandleA(int PipeExists, BString PipeName, DWORD ReadWrite, DWORD StdHandle)
{
  HANDLE TempPipe;
  SECURITY_ATTRIBUTES SecurityAttr;

  SecurityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  SecurityAttr.bInheritHandle = TRUE;
  SecurityAttr.lpSecurityDescriptor = NULL;

  if (PipeExists && PipeName.GetSize())
  {
    TempPipe = CreateFileA(*PipeName, ReadWrite, 0, &SecurityAttr, OPEN_EXISTING, 0, NULL);
    while (TempPipe == INVALID_HANDLE_VALUE && ::GetLastError() == ERROR_PIPE_BUSY)
    {
      WaitNamedPipeA(*PipeName, NMPWAIT_USE_DEFAULT_WAIT);
      TempPipe = CreateFileA(*PipeName, ReadWrite, 0, &SecurityAttr, OPEN_EXISTING, 0, NULL);
    }
  }
  else if (PipeExists)  TempPipe = GetStdHandle(StdHandle);
  else  TempPipe = NULL;

  if (TempPipe == INVALID_HANDLE_VALUE)  TempPipe = NULL;

  return TempPipe;
}

HANDLE GetPipeOrStdHandleW(int PipeExists, BString PipeName, DWORD ReadWrite, DWORD StdHandle)
{
  HANDLE TempPipe;
  SECURITY_ATTRIBUTES SecurityAttr;

  SecurityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  SecurityAttr.bInheritHandle = TRUE;
  SecurityAttr.lpSecurityDescriptor = NULL;

  if (PipeExists && PipeName.GetSize())
  {
    TempPipe = CreateFileW((LPWSTR)*PipeName, ReadWrite, 0, &SecurityAttr, OPEN_EXISTING, 0, NULL);
    while (TempPipe == INVALID_HANDLE_VALUE && ::GetLastError() == ERROR_PIPE_BUSY)
    {
      WaitNamedPipeW((LPWSTR)*PipeName, NMPWAIT_USE_DEFAULT_WAIT);
      TempPipe = CreateFileW((LPWSTR)*PipeName, ReadWrite, 0, &SecurityAttr, OPEN_EXISTING, 0, NULL);
    }
  }
  else if (PipeExists)  TempPipe = GetStdHandle(StdHandle);
  else  TempPipe = NULL;

  if (TempPipe == INVALID_HANDLE_VALUE)  TempPipe = NULL;

  return TempPipe;
}

HKEY MakeHKEY(HKEY_Pack &TempPack)
{
  HKEY RootKey, Result;
  SECURITY_ATTRIBUTES SecAttr, *SecAttrPtr;

  if (TempPack.MxRootKey == "HKCR")  RootKey = HKEY_CLASSES_ROOT;
  else if (TempPack.MxRootKey == "HKCC")  RootKey = HKEY_CURRENT_CONFIG;
  else if (TempPack.MxRootKey == "HKCU")  RootKey = HKEY_CURRENT_USER;
  else if (TempPack.MxRootKey == "HKLM")  RootKey = HKEY_LOCAL_MACHINE;
  else if (TempPack.MxRootKey == "HKU")  RootKey = HKEY_USERS;
  else  RootKey = NULL;

  SecAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  SecAttr.bInheritHandle = TempPack.MxAttrInherit;
  SecAttr.lpSecurityDescriptor = NULL;
  SecAttrPtr = (TempPack.MxAttrExists ? &SecAttr : NULL);

  Result = NULL;
  if (TempPack.MxTargetFunc == "RegCreateKeyExA")
  {
    ::RegCreateKeyExA(RootKey, (LPSTR)*TempPack.MxSubKey, 0,
                      (LPSTR)(TempPack.MxClassExists ? *TempPack.MxClass : NULL),
                      (DWORD)TempPack.MxOptions, (REGSAM)TempPack.MxRegSam, SecAttrPtr,
                      &Result, NULL);
  }
  else if (TempPack.MxTargetFunc == "RegCreateKeyExW")
  {
    ::RegCreateKeyExW(RootKey, (LPWSTR)*TempPack.MxSubKey, 0,
                      (LPWSTR)(TempPack.MxClassExists ? *TempPack.MxClass : NULL),
                      (DWORD)TempPack.MxOptions, (REGSAM)TempPack.MxRegSam, SecAttrPtr,
                      &Result, NULL);
  }
  else if (TempPack.MxTargetFunc == "RegOpenKeyExA")
  {
    ::RegOpenKeyExA(RootKey, (LPSTR)*TempPack.MxSubKey, 0,
                    (REGSAM)TempPack.MxRegSam, &Result);
  }
  else if (TempPack.MxTargetFunc == "RegOpenKeyExW")
  {
    ::RegOpenKeyExW(RootKey, (LPWSTR)*TempPack.MxSubKey, 0,
                    (REGSAM)TempPack.MxRegSam, &Result);
  }

  return Result;
}

int APIENTRY WinMain(HINSTANCE CurrModule, HINSTANCE PrevModule, LPSTR CmdLine, int CmdShow)
{
#ifdef _DEBUG
  ::MessageBox(::GetForegroundWindow(), "Hook in debugger now.", "Elevate.exe", MB_OK);
#endif

  GxApp.Init(CurrModule, PrevModule, CmdLine, CmdShow);

  if (GxApp.MxArgs.GetSize() != 3)
  {
    // Handle the case where Elevate.exe was started as a different user.
    if (GxApp.MxArgs.GetSize() == 4 && GxApp.MxArgs[1] == "link")
    {
      BString Filename, Directory, Parameters;
      SHELLEXECUTEINFOW TempInfo = {0};

      Directory = System::ANSI2Unicode(System::AppDirectory(""));
      Filename = System::ANSI2Unicode(System::AppDirectory("") + "Elevate.exe");
      Parameters = System::ANSI2Unicode(GxApp.MxArgs[2] + " " + GxApp.MxArgs[3]);
      TempInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
      TempInfo.fMask = 0;
      TempInfo.hwnd = NULL;
      TempInfo.lpVerb = L"runas";
      TempInfo.lpFile = (LPWSTR)*Filename;
      TempInfo.lpParameters = (LPWSTR)*Parameters;
      TempInfo.lpDirectory = (LPWSTR)*Directory;
      TempInfo.nShow = SW_NORMAL;

      // NOTE:  ShellExecuteEx() returns only when Elevate.exe has been started.
      if (::ShellExecuteExW(&TempInfo))  return 0;

      return 1;
    }

    DumpSyntax();

    return 1;
  }

  BString Data, TempData, Command;
  BStorage TempStorage;
  CreateProcessElevatedPack PackData;
  Pipe ProcessComm;
  FastEvent ProcessConnectReady, ProcessResultsReady, ProcessResultReceived;
  Block<LibFunc> LibFuncs;

  PackData.MxCurrProcessID = GxApp.MxArgs[1];
  PackData.MxCurrThreadID = GxApp.MxArgs[2];

  ProcessConnectReady.Create("ConnectReady_585656DF-ADC4-48BC-AE19-7CF6310BAFB2_" + BString(PackData.MxCurrProcessID) + "_" + BString(PackData.MxCurrThreadID));
  ProcessConnectReady.FireEvent();
  ProcessComm.Open(*BString("585656DF-ADC4-48BC-AE19-7CF6310BAFB2_" + BString(PackData.MxCurrProcessID) + "_" + BString(PackData.MxCurrThreadID)), "rw");
  ProcessResultsReady.Create("ResultsReady_585656DF-ADC4-48BC-AE19-7CF6310BAFB2_" + BString(PackData.MxCurrProcessID) + "_" + BString(PackData.MxCurrThreadID));
  ProcessResultReceived.Create("ResultRecv_585656DF-ADC4-48BC-AE19-7CF6310BAFB2_" + BString(PackData.MxCurrProcessID) + "_" + BString(PackData.MxCurrThreadID));

  // Get and extract the packed data from Elevate.dll.
  while (ProcessComm.ReadPacket(Data) && TempStorage.Load(Data) && TempStorage.GetData(Command) && Command != "SelfTerminate" && TempStorage.GetData(Data))
  {
    if (Command == "CreateProcess")
    {
      CreateProcessResultPack TempResultPack;
      SECURITY_ATTRIBUTES ProcAttrs, ThreadAttrs, *ProcAttrsPtr, *ThreadAttrsPtr;
      STARTUPINFOA StartupInfoA = {0};
      STARTUPINFOW StartupInfoW = {0};
      SHELLEXECUTEINFOA ShellExecuteInfoA = {0};
      SHELLEXECUTEINFOW ShellExecuteInfoW = {0};
      PROCESS_INFORMATION TempProcessInfo = {0};
      BOOL RetCode;

      PackData.Load(Data);

      // Set up for process creation.
      ProcAttrs.nLength = sizeof(SECURITY_ATTRIBUTES);
      ProcAttrs.bInheritHandle = PackData.MxProcAttrInherit;
      ProcAttrs.lpSecurityDescriptor = NULL;

      ThreadAttrs.nLength = sizeof(SECURITY_ATTRIBUTES);
      ThreadAttrs.bInheritHandle = PackData.MxThreadAttrInherit;
      ThreadAttrs.lpSecurityDescriptor = NULL;

      ProcAttrsPtr = (PackData.MxProcAttrExists ? &ProcAttrs : NULL);
      ThreadAttrsPtr = (PackData.MxThreadAttrExists ? &ThreadAttrs : NULL);

      // Attempt to connect this application to an existing console.
      // Upon success, the current process obtains the standard
      // handles associated with console applications (stdin, stdout, stderr).
      if (!(PackData.MxCreationFlags & CREATE_NEW_CONSOLE) && !(PackData.MxCreationFlags & DETACHED_PROCESS) && !(PackData.MxShellExecuteInfo.MxMask & SEE_MASK_NO_CONSOLE))
      {
        ::AttachConsole((DWORD)PackData.MxCurrProcessID);
      }

      // Load the appropriate STARTUPINFO and SHELLEXECUTEINFO structures.
      if (PackData.MxTargetFunc.GetSize() && PackData.MxTargetFunc[PackData.MxTargetFunc.GetSize() - 1] == 'A')
      {
        StartupInfoA.cb = sizeof(STARTUPINFOA);
        StartupInfoA.lpReserved = NULL;
        StartupInfoA.lpDesktop = (LPSTR)(PackData.MxStartupInfo.MxDesktopExists ? *PackData.MxStartupInfo.MxDesktop : NULL);
        StartupInfoA.lpTitle = (LPSTR)(PackData.MxStartupInfo.MxTitleExists ? *PackData.MxStartupInfo.MxTitle : NULL);
        StartupInfoA.dwX = (DWORD)PackData.MxStartupInfo.MxX;
        StartupInfoA.dwY = (DWORD)PackData.MxStartupInfo.MxY;
        StartupInfoA.dwXSize = (DWORD)PackData.MxStartupInfo.MxXSize;
        StartupInfoA.dwYSize = (DWORD)PackData.MxStartupInfo.MxYSize;
        StartupInfoA.dwXCountChars = (DWORD)PackData.MxStartupInfo.MxXCountChars;
        StartupInfoA.dwYCountChars = (DWORD)PackData.MxStartupInfo.MxYCountChars;
        StartupInfoA.dwFillAttribute = (DWORD)PackData.MxStartupInfo.MxFillAttr;
        StartupInfoA.dwFlags = (DWORD)PackData.MxStartupInfo.MxFlags;
        StartupInfoA.wShowWindow = (WORD)PackData.MxStartupInfo.MxShowWindow;
        StartupInfoA.cbReserved2 = 0;
        StartupInfoA.lpReserved2 = NULL;
        if (StartupInfoA.dwFlags & STARTF_USESTDHANDLES)
        {
          StartupInfoA.hStdInput = GetPipeOrStdHandleA(PackData.MxStartupInfo.MxStdinExists, PackData.MxStartupInfo.MxStdin, GENERIC_READ, STD_INPUT_HANDLE);
          StartupInfoA.hStdOutput = GetPipeOrStdHandleA(PackData.MxStartupInfo.MxStdoutExists, PackData.MxStartupInfo.MxStdout, GENERIC_WRITE, STD_OUTPUT_HANDLE);
          StartupInfoA.hStdError = GetPipeOrStdHandleA(PackData.MxStartupInfo.MxStderrExists, PackData.MxStartupInfo.MxStderr, GENERIC_WRITE, STD_ERROR_HANDLE);
        }

        ShellExecuteInfoA.cbSize = sizeof(SHELLEXECUTEINFOA);
        ShellExecuteInfoA.fMask = (ULONG)PackData.MxShellExecuteInfo.MxMask;
        ShellExecuteInfoA.hwnd = (HWND)PackData.MxShellExecuteInfo.MxHWnd;
        ShellExecuteInfoA.lpVerb = (LPSTR)*PackData.MxShellExecuteInfo.MxVerb;
        ShellExecuteInfoA.lpFile = (LPSTR)(PackData.MxApplicationNameExists ? *PackData.MxApplicationName : NULL);
        ShellExecuteInfoA.lpParameters = (LPSTR)(PackData.MxCommandLineExists ? *PackData.MxCommandLine : NULL);
        ShellExecuteInfoA.lpDirectory = (LPSTR)(PackData.MxCurrDir.GetSize() ? *PackData.MxCurrDir : NULL);
        ShellExecuteInfoA.nShow = (int)PackData.MxShellExecuteInfo.MxShowWindow;
        if ((ShellExecuteInfoA.fMask & SEE_MASK_IDLIST) || (ShellExecuteInfoA.fMask & SEE_MASK_INVOKEIDLIST))
        {
          ShellExecuteInfoA.lpIDList = PackData.MxShellExecuteInfo.MxIDList.RawBytes();
        }
        if (ShellExecuteInfoA.fMask & SEE_MASK_CLASSNAME)
        {
          ShellExecuteInfoA.lpClass = (LPSTR)*PackData.MxShellExecuteInfo.MxClass;
        }
        if (ShellExecuteInfoA.fMask & SEE_MASK_CLASSKEY)
        {
          ShellExecuteInfoA.hkeyClass = MakeHKEY(PackData.MxShellExecuteInfo.MxKeyClass);
          if (ShellExecuteInfoA.hkeyClass == NULL)  ShellExecuteInfoA.fMask = ShellExecuteInfoA.fMask & ~SEE_MASK_CLASSKEY;
        }
        if (ShellExecuteInfoA.fMask & SEE_MASK_HOTKEY)
        {
          ShellExecuteInfoA.dwHotKey = (DWORD)PackData.MxShellExecuteInfo.MxHotkey;
        }
        if (ShellExecuteInfoA.fMask & SEE_MASK_HMONITOR)
        {
          POINT TempPt;
          TempPt.x = (LONG)PackData.MxShellExecuteInfo.MxMonitorPosX;
          TempPt.y = (LONG)PackData.MxShellExecuteInfo.MxMonitorPosY;
          ShellExecuteInfoA.hMonitor = MonitorFromPoint(TempPt, MONITOR_DEFAULTTONEAREST);
        }
      }
      else
      {
        StartupInfoW.cb = sizeof(STARTUPINFOW);
        StartupInfoW.lpReserved = NULL;
        StartupInfoW.lpDesktop = (LPWSTR)(PackData.MxStartupInfo.MxDesktopExists ? *PackData.MxStartupInfo.MxDesktop : NULL);
        StartupInfoW.lpTitle = (LPWSTR)(PackData.MxStartupInfo.MxTitleExists ? *PackData.MxStartupInfo.MxTitle : NULL);
        StartupInfoW.dwX = (DWORD)PackData.MxStartupInfo.MxX;
        StartupInfoW.dwY = (DWORD)PackData.MxStartupInfo.MxY;
        StartupInfoW.dwXSize = (DWORD)PackData.MxStartupInfo.MxXSize;
        StartupInfoW.dwYSize = (DWORD)PackData.MxStartupInfo.MxYSize;
        StartupInfoW.dwXCountChars = (DWORD)PackData.MxStartupInfo.MxXCountChars;
        StartupInfoW.dwYCountChars = (DWORD)PackData.MxStartupInfo.MxYCountChars;
        StartupInfoW.dwFillAttribute = (DWORD)PackData.MxStartupInfo.MxFillAttr;
        StartupInfoW.dwFlags = (DWORD)PackData.MxStartupInfo.MxFlags;
        StartupInfoW.wShowWindow = (WORD)PackData.MxStartupInfo.MxShowWindow;
        StartupInfoW.cbReserved2 = 0;
        StartupInfoW.lpReserved2 = NULL;
        if (StartupInfoW.dwFlags & STARTF_USESTDHANDLES)
        {
          StartupInfoW.hStdInput = GetPipeOrStdHandleW(PackData.MxStartupInfo.MxStdinExists, PackData.MxStartupInfo.MxStdin, GENERIC_READ, STD_INPUT_HANDLE);
          StartupInfoW.hStdOutput = GetPipeOrStdHandleW(PackData.MxStartupInfo.MxStdoutExists, PackData.MxStartupInfo.MxStdout, GENERIC_WRITE, STD_OUTPUT_HANDLE);
          StartupInfoW.hStdError = GetPipeOrStdHandleW(PackData.MxStartupInfo.MxStderrExists, PackData.MxStartupInfo.MxStderr, GENERIC_WRITE, STD_ERROR_HANDLE);
        }

        ShellExecuteInfoW.cbSize = sizeof(SHELLEXECUTEINFOW);
        ShellExecuteInfoW.fMask = (ULONG)PackData.MxShellExecuteInfo.MxMask;
        ShellExecuteInfoW.hwnd = (HWND)PackData.MxShellExecuteInfo.MxHWnd;
        ShellExecuteInfoW.lpVerb = (LPWSTR)*PackData.MxShellExecuteInfo.MxVerb;
        ShellExecuteInfoW.lpFile = (LPWSTR)(PackData.MxApplicationNameExists ? *PackData.MxApplicationName : NULL);
        ShellExecuteInfoW.lpParameters = (LPWSTR)(PackData.MxCommandLineExists ? *PackData.MxCommandLine : NULL);
        ShellExecuteInfoW.lpDirectory = (LPWSTR)(PackData.MxCurrDir.GetSize() ? *PackData.MxCurrDir : NULL);
        ShellExecuteInfoW.nShow = (int)PackData.MxShellExecuteInfo.MxShowWindow;
        if ((ShellExecuteInfoW.fMask & SEE_MASK_IDLIST) || (ShellExecuteInfoW.fMask & SEE_MASK_INVOKEIDLIST))
        {
          ShellExecuteInfoW.lpIDList = PackData.MxShellExecuteInfo.MxIDList.RawBytes();
        }
        if (ShellExecuteInfoW.fMask & SEE_MASK_CLASSNAME)
        {
          ShellExecuteInfoW.lpClass = (LPWSTR)*PackData.MxShellExecuteInfo.MxClass;
        }
        if (ShellExecuteInfoW.fMask & SEE_MASK_CLASSKEY)
        {
          ShellExecuteInfoW.hkeyClass = MakeHKEY(PackData.MxShellExecuteInfo.MxKeyClass);
          if (ShellExecuteInfoW.hkeyClass == NULL)  ShellExecuteInfoW.fMask = ShellExecuteInfoW.fMask & ~SEE_MASK_CLASSKEY;
        }
        if (ShellExecuteInfoW.fMask & SEE_MASK_HOTKEY)
        {
          ShellExecuteInfoW.dwHotKey = (DWORD)PackData.MxShellExecuteInfo.MxHotkey;
        }
        if (ShellExecuteInfoW.fMask & SEE_MASK_HMONITOR)
        {
          POINT TempPt;
          TempPt.x = (LONG)PackData.MxShellExecuteInfo.MxMonitorPosX;
          TempPt.y = (LONG)PackData.MxShellExecuteInfo.MxMonitorPosY;
          ShellExecuteInfoW.hMonitor = MonitorFromPoint(TempPt, MONITOR_DEFAULTTONEAREST);
        }
      }

      // Start the process.
      if (PackData.MxTargetFunc == "CreateProcessA")
      {
        RetCode = ::CreateProcessA(
                      (PackData.MxApplicationNameExists ? *PackData.MxApplicationName : NULL),
                      (PackData.MxCommandLineExists ? *PackData.MxCommandLine : NULL),
                      ProcAttrsPtr, ThreadAttrsPtr, PackData.MxInheritHandles,
                      (DWORD)PackData.MxCreationFlags | CREATE_SUSPENDED,
                      (PackData.MxEnvironment.GetSize() ? *PackData.MxEnvironment : NULL),
                      (PackData.MxCurrDir.GetSize() ? *PackData.MxCurrDir : NULL),
                      &StartupInfoA, &TempProcessInfo);
      }
      else if (PackData.MxTargetFunc == "CreateProcessW")
      {
        RetCode = ::CreateProcessW(
                      (LPWSTR)(PackData.MxApplicationNameExists ? *PackData.MxApplicationName : NULL),
                      (LPWSTR)(PackData.MxCommandLineExists ? *PackData.MxCommandLine : NULL),
                      ProcAttrsPtr, ThreadAttrsPtr, PackData.MxInheritHandles,
                      (DWORD)PackData.MxCreationFlags | CREATE_SUSPENDED,
                      (PackData.MxEnvironment.GetSize() ? *PackData.MxEnvironment : NULL),
                      (LPWSTR)(PackData.MxCurrDir.GetSize() ? *PackData.MxCurrDir : NULL),
                      &StartupInfoW, &TempProcessInfo);
      }
      else if (PackData.MxTargetFunc == "ShellExecuteA")
      {
        RetCode = (BOOL)::ShellExecuteA(ShellExecuteInfoA.hwnd, ShellExecuteInfoA.lpVerb,
                                        ShellExecuteInfoA.lpFile, ShellExecuteInfoA.lpParameters,
                                        ShellExecuteInfoA.lpDirectory, ShellExecuteInfoA.nShow);
      }
      else if (PackData.MxTargetFunc == "ShellExecuteW")
      {
        RetCode = (BOOL)::ShellExecuteW(ShellExecuteInfoW.hwnd, ShellExecuteInfoW.lpVerb,
                                        ShellExecuteInfoW.lpFile, ShellExecuteInfoW.lpParameters,
                                        ShellExecuteInfoW.lpDirectory, ShellExecuteInfoW.nShow);
      }
      else if (PackData.MxTargetFunc == "ShellExecuteExA")
      {
        ::ShellExecuteExA(&ShellExecuteInfoA);
        RetCode = (BOOL)ShellExecuteInfoA.hInstApp;
        if ((PackData.MxShellExecuteInfo.MxMask & SEE_MASK_NOCLOSEPROCESS) && ShellExecuteInfoA.hProcess)
        {
          TempProcessInfo.dwProcessId = ::GetProcessId(ShellExecuteInfoA.hProcess);
          ::CloseHandle(ShellExecuteInfoA.hProcess);
        }
      }
      else if (PackData.MxTargetFunc == "ShellExecuteExW")
      {
        ::ShellExecuteExW(&ShellExecuteInfoW);
        RetCode = (BOOL)ShellExecuteInfoW.hInstApp;
        if ((PackData.MxShellExecuteInfo.MxMask & SEE_MASK_NOCLOSEPROCESS) && ShellExecuteInfoW.hProcess)
        {
          TempProcessInfo.dwProcessId = ::GetProcessId(ShellExecuteInfoW.hProcess);
          ::CloseHandle(ShellExecuteInfoW.hProcess);
        }
      }
      else
      {
        ::SetLastError(ERROR_INVALID_FUNCTION);
        RetCode = 0;
      }

      // Close handles (if necessary).
      if (PackData.MxTargetFunc.GetSize() && PackData.MxTargetFunc[PackData.MxTargetFunc.GetSize() - 1] == 'A')
      {
        if (StartupInfoA.dwFlags & STARTF_USESTDHANDLES)
        {
          if (StartupInfoA.hStdInput && PackData.MxStartupInfo.MxStdinExists && PackData.MxStartupInfo.MxStdin.GetSize())
          {
            ::CloseHandle(StartupInfoA.hStdInput);
          }
          if (StartupInfoA.hStdOutput && PackData.MxStartupInfo.MxStdoutExists && PackData.MxStartupInfo.MxStdout.GetSize())
          {
            ::CloseHandle(StartupInfoA.hStdOutput);
          }
          if (StartupInfoA.hStdError && PackData.MxStartupInfo.MxStderrExists && PackData.MxStartupInfo.MxStderr.GetSize())
          {
            ::CloseHandle(StartupInfoA.hStdError);
          }
        }
      }
      else
      {
        if (StartupInfoW.dwFlags & STARTF_USESTDHANDLES)
        {
          if (StartupInfoW.hStdInput && PackData.MxStartupInfo.MxStdinExists && PackData.MxStartupInfo.MxStdin.GetSize())
          {
            ::CloseHandle(StartupInfoW.hStdInput);
          }
          if (StartupInfoW.hStdOutput && PackData.MxStartupInfo.MxStdoutExists && PackData.MxStartupInfo.MxStdout.GetSize())
          {
            ::CloseHandle(StartupInfoW.hStdOutput);
          }
          if (StartupInfoW.hStdError && PackData.MxStartupInfo.MxStderrExists && PackData.MxStartupInfo.MxStderr.GetSize())
          {
            ::CloseHandle(StartupInfoW.hStdError);
          }
        }
      }

      // Send the results of the API call back to Elevate.dll.
      ProcessResultsReady.FireEvent();
      TempResultPack.MxRetCode = RetCode;
      TempResultPack.MxErrorCode = ::GetLastError();
      TempResultPack.MxProcessID = TempProcessInfo.dwProcessId;
      TempResultPack.MxThreadID = TempProcessInfo.dwThreadId;
      TempResultPack.Save(Data);
      ProcessComm.WritePacket(Data);
#ifdef _DEBUG
      ProcessResultReceived.Wait();
#else
      ProcessResultReceived.Wait(10000);
#endif

      // Resume the thread if the process isn't supposed to be suspended.
      if (TempProcessInfo.hThread && !(PackData.MxCreationFlags & CREATE_SUSPENDED))
      {
        ::ResumeThread(TempProcessInfo.hThread);
      }

      if (TempProcessInfo.hProcess)  ::CloseHandle(TempProcessInfo.hProcess);
      if (TempProcessInfo.hThread)  ::CloseHandle(TempProcessInfo.hThread);
    }
    else if (Command == "LoadLibrary")
    {
      HMODULE Result;
      size_t LibNum;
      LibFunc TempLibFunc;

      Result = ::LoadLibraryW((LPCWSTR)*Data);
      if (Result == NULL)  LibNum = 0;
      else
      {
        TempLibFunc.DLL_ElevatedLink_SendData = (DLLElevatedLink_SendDataType)::GetProcAddress(Result, "ElevatedLink_SendData");
        TempLibFunc.DLL_ElevatedLink_GetData = (DLLElevatedLink_GetDataType)::GetProcAddress(Result, "ElevatedLink_GetData");
        if (TempLibFunc.DLL_ElevatedLink_SendData == NULL || TempLibFunc.DLL_ElevatedLink_GetData == NULL)
        {
          LibNum = 0;
        }
        else
        {
          TempLibFunc.DLL_ElevatedLink_SendFinalize = (DLLElevatedLink_SendFinalizeType)::GetProcAddress(Result, "ElevatedLink_SendFinalize");
          LibFuncs[LibFuncs.GetSize()] = TempLibFunc;
          LibNum = LibFuncs.GetSize();
        }
      }

      // Send the results of the API call back to Elevate.dll.
      ProcessResultsReady.FireEvent();
      ProcessComm.WritePacket(LibNum);
#ifdef _DEBUG
      ProcessResultReceived.Wait();
#else
      ProcessResultReceived.Wait(10000);
#endif
    }
    else if (Command == "LibFunc")
    {
      BStorage TempStorage;
      size_t LibNum;
      DWORD TempSize;
      BString Result;

      TempStorage.Load(Data);
      TempStorage.GetData(TempData);
      LibNum = TempData;
      if (LibNum > 0 && LibNum <= LibFuncs.GetSize())
      {
        TempStorage.GetData(Data);
        TempSize = LibFuncs[LibNum].DLL_ElevatedLink_SendData(Data.RawBytes(), Data.GetSize());
        if (TempSize)
        {
          Result.SetSize((size_t)TempSize);
          if (!LibFuncs[LibNum].DLL_ElevatedLink_GetData(Result.RawBytes()))  Result.Empty();
        }

        // Send the results of the call back to Elevate.dll.
        ProcessResultsReady.FireEvent();
        ProcessComm.WritePacket(Result);
#ifdef _DEBUG
        ProcessResultReceived.Wait();
#else
        ProcessResultReceived.Wait(10000);
#endif

        // Let the DLL clean up any resources from the call.
        if (LibFuncs[LibNum].DLL_ElevatedLink_SendFinalize)  LibFuncs[LibNum].DLL_ElevatedLink_SendFinalize();
      }
      else
      {
        // Send the results of the call back to Elevate.dll.
        ProcessResultsReady.FireEvent();
        ProcessComm.WritePacket(Result);
#ifdef _DEBUG
        ProcessResultReceived.Wait();
#else
        ProcessResultReceived.Wait(10000);
#endif
      }
    }
  }

  // Send the results of the call back to Elevate.dll.
  ProcessResultsReady.FireEvent();
  ProcessComm.WritePacket(BString());
#ifdef _DEBUG
  ProcessResultReceived.Wait();
#else
  ProcessResultReceived.Wait(10000);
#endif

  return 0;
}
