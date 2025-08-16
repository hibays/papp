// Packed data classes used to transport data.
// BStorage provides a clean, portable, and cross-platform
// data storage and transport mechanism.

class HKEY_Pack
{
public:
  HKEY_Pack();

  int Load(const BString &Data);
  int Save(BString &Result);

  BString MxTargetFunc;
  BString MxRootKey, MxSubKey;
  int MxClassExists;
  BString MxClass;
  UInt64 MxOptions, MxRegSam;
  int MxAttrExists, MxAttrInherit;
};

HKEY_Pack::HKEY_Pack()
{
  MxClassExists = 0;
  MxRegSam = 0;
  MxAttrExists = 0;
  MxAttrInherit = 0;
}

int HKEY_Pack::Load(const BString &Data)
{
  BStorage TempStorage;
  BString TempData;

  TempStorage.Load(Data);
  TempStorage.GetData(MxTargetFunc);
  TempStorage.GetData(MxRootKey);
  TempStorage.GetData(MxSubKey);
  TempStorage.GetData(TempData);
  MxClassExists = TempData;
  TempStorage.GetData(MxClass);
  TempStorage.GetData(TempData);
  MxOptions = TempData;
  TempStorage.GetData(TempData);
  MxRegSam = TempData;
  TempStorage.GetData(TempData);
  MxAttrExists = TempData;
  TempStorage.GetData(TempData);
  MxAttrInherit = TempData;

  return 1;
}

int HKEY_Pack::Save(BString &Result)
{
  BStorage TempStorage;

  TempStorage.SetStorageType(BStorage::StoreBlock);
  TempStorage.AddData(MxTargetFunc);
  TempStorage.AddData(MxRootKey);
  TempStorage.AddData(MxSubKey);
  TempStorage.AddData(MxClassExists);
  TempStorage.AddData(MxClass);
  TempStorage.AddData(MxOptions);
  TempStorage.AddData(MxRegSam);
  TempStorage.AddData(MxAttrExists);
  TempStorage.AddData(MxAttrInherit);
  TempStorage.Finalize();
  Result = TempStorage.Save();

  return 1;
}


class CreateProcessElevatedPack
{
public:
  class STARTUPINFO_Pack
  {
  public:
    STARTUPINFO_Pack();

    int Load(const BString &Data);
    int Save(BString &Result);

    int MxDesktopExists, MxTitleExists;
    BString MxDesktop, MxTitle;
    UInt64 MxX, MxY, MxXSize, MxYSize, MxXCountChars, MxYCountChars, MxFillAttr;
    UInt64 MxFlags;
    UInt64 MxShowWindow;
    int MxStdinExists, MxStdoutExists, MxStderrExists;
    BString MxStdin, MxStdout, MxStderr;
  };

  class SHELLEXECUTEINFO_Pack
  {
  public:
    SHELLEXECUTEINFO_Pack();

    int Load(const BString &Data);
    int Save(BString &Result);

    UInt64 MxMask;
    UInt64 MxHWnd;
    BString MxVerb;
    UInt64 MxShowWindow;
    BString MxIDList;
    BString MxClass;
    HKEY_Pack MxKeyClass;
    UInt64 MxHotkey;
    SInt64 MxMonitorPosX, MxMonitorPosY;
  };

  CreateProcessElevatedPack();

  int Load(const BString &Data);
  int Save(BString &Result);

  UInt64 MxCurrProcessID, MxCurrThreadID;
  BString MxTargetFunc;
  int MxApplicationNameExists, MxCommandLineExists;
  BString MxApplicationName, MxCommandLine;
  int MxProcAttrExists, MxProcAttrInherit;
  int MxThreadAttrExists, MxThreadAttrInherit;
  int MxInheritHandles;
  UInt64 MxCreationFlags;
  BString MxEnvironment;
  BString MxCurrDir;
  STARTUPINFO_Pack MxStartupInfo;
  SHELLEXECUTEINFO_Pack MxShellExecuteInfo;
};

CreateProcessElevatedPack::STARTUPINFO_Pack::STARTUPINFO_Pack()
{
  MxDesktopExists = 0;
  MxTitleExists = 0;
  MxX = 0;
  MxY = 0;
  MxXSize = 0;
  MxYSize = 0;
  MxXCountChars = 0;
  MxYCountChars = 0;
  MxFillAttr = 0;
  MxFlags = 0;
  MxShowWindow = 0;
  MxStdinExists = 0;
  MxStdoutExists = 0;
  MxStderrExists = 0;
}

int CreateProcessElevatedPack::STARTUPINFO_Pack::Load(const BString &Data)
{
  BStorage TempStorage;
  BString TempData;

  TempStorage.Load(Data);
  TempStorage.GetData(TempData);
  MxDesktopExists = TempData;
  TempStorage.GetData(TempData);
  MxTitleExists = TempData;
  TempStorage.GetData(MxDesktop);
  TempStorage.GetData(MxTitle);
  TempStorage.GetData(TempData);
  MxX = TempData;
  TempStorage.GetData(TempData);
  MxY = TempData;
  TempStorage.GetData(TempData);
  MxXSize = TempData;
  TempStorage.GetData(TempData);
  MxYSize = TempData;
  TempStorage.GetData(TempData);
  MxXCountChars = TempData;
  TempStorage.GetData(TempData);
  MxYCountChars = TempData;
  TempStorage.GetData(TempData);
  MxFillAttr = TempData;
  TempStorage.GetData(TempData);
  MxFlags = TempData;
  TempStorage.GetData(TempData);
  MxShowWindow = TempData;
  TempStorage.GetData(TempData);
  MxStdinExists = TempData;
  TempStorage.GetData(TempData);
  MxStdoutExists = TempData;
  TempStorage.GetData(TempData);
  MxStderrExists = TempData;
  TempStorage.GetData(MxStdin);
  TempStorage.GetData(MxStdout);
  TempStorage.GetData(MxStderr);

  return 1;
}

int CreateProcessElevatedPack::STARTUPINFO_Pack::Save(BString &Result)
{
  BStorage TempStorage;

  TempStorage.SetStorageType(BStorage::StoreBlock);
  TempStorage.AddData(MxDesktopExists);
  TempStorage.AddData(MxTitleExists);
  TempStorage.AddData(MxDesktop);
  TempStorage.AddData(MxTitle);
  TempStorage.AddData(MxX);
  TempStorage.AddData(MxY);
  TempStorage.AddData(MxXSize);
  TempStorage.AddData(MxYSize);
  TempStorage.AddData(MxXCountChars);
  TempStorage.AddData(MxYCountChars);
  TempStorage.AddData(MxFillAttr);
  TempStorage.AddData(MxFlags);
  TempStorage.AddData(MxShowWindow);
  TempStorage.AddData(MxStdinExists);
  TempStorage.AddData(MxStdoutExists);
  TempStorage.AddData(MxStderrExists);
  TempStorage.AddData(MxStdin);
  TempStorage.AddData(MxStdout);
  TempStorage.AddData(MxStderr);
  TempStorage.Finalize();
  Result = TempStorage.Save();

  return 1;
}


CreateProcessElevatedPack::SHELLEXECUTEINFO_Pack::SHELLEXECUTEINFO_Pack()
{
  MxMask = 0;
  MxHWnd = NULL;
  MxShowWindow = 0;
  MxHotkey = 0;
  MxMonitorPosX = 0;
  MxMonitorPosY = 0;
}

int CreateProcessElevatedPack::SHELLEXECUTEINFO_Pack::Load(const BString &Data)
{
  BStorage TempStorage;
  BString TempData;

  TempStorage.Load(Data);
  TempStorage.GetData(TempData);
  MxMask = TempData;
  TempStorage.GetData(TempData);
  MxHWnd = TempData;
  TempStorage.GetData(MxVerb);
  TempStorage.GetData(TempData);
  MxShowWindow = TempData;
  TempStorage.GetData(MxIDList);
  TempStorage.GetData(MxClass);
  TempStorage.GetData(TempData);
  MxKeyClass.Load(TempData);
  TempStorage.GetData(TempData);
  MxHotkey = TempData;
  TempStorage.GetData(TempData);
  MxMonitorPosX = TempData;
  TempStorage.GetData(TempData);
  MxMonitorPosY = TempData;

  return 1;
}

int CreateProcessElevatedPack::SHELLEXECUTEINFO_Pack::Save(BString &Result)
{
  BStorage TempStorage;
  BString TempData;

  TempStorage.SetStorageType(BStorage::StoreBlock);
  TempStorage.AddData(MxMask);
  TempStorage.AddData(MxHWnd);
  TempStorage.AddData(MxVerb);
  TempStorage.AddData(MxShowWindow);
  TempStorage.AddData(MxIDList);
  TempStorage.AddData(MxClass);
  MxKeyClass.Save(TempData);
  TempStorage.AddData(TempData);
  TempStorage.AddData(MxHotkey);
  TempStorage.AddData(MxMonitorPosX);
  TempStorage.AddData(MxMonitorPosY);
  TempStorage.Finalize();
  Result = TempStorage.Save();

  return 1;
}


CreateProcessElevatedPack::CreateProcessElevatedPack()
{
  MxCurrProcessID = 0;
  MxCurrThreadID = 0;
  MxApplicationNameExists = 0;
  MxCommandLineExists = 0;
  MxProcAttrExists = 0;
  MxProcAttrInherit = 0;
  MxThreadAttrExists = 0;
  MxThreadAttrInherit = 0;
  MxInheritHandles = 0;
  MxCreationFlags = 0;
}

int CreateProcessElevatedPack::Load(const BString &Data)
{
  BStorage TempStorage;
  BString TempData;

  TempStorage.Load(Data);
  TempStorage.GetData(TempData);
  MxCurrProcessID = TempData;
  TempStorage.GetData(TempData);
  MxCurrThreadID = TempData;
  TempStorage.GetData(MxTargetFunc);
  TempStorage.GetData(TempData);
  MxApplicationNameExists = TempData;
  TempStorage.GetData(TempData);
  MxCommandLineExists = TempData;
  TempStorage.GetData(MxApplicationName);
  TempStorage.GetData(MxCommandLine);
  TempStorage.GetData(TempData);
  MxProcAttrExists = TempData;
  TempStorage.GetData(TempData);
  MxProcAttrInherit = TempData;
  TempStorage.GetData(TempData);
  MxThreadAttrExists = TempData;
  TempStorage.GetData(TempData);
  MxThreadAttrInherit = TempData;
  TempStorage.GetData(TempData);
  MxInheritHandles = TempData;
  TempStorage.GetData(TempData);
  MxCreationFlags = TempData;
  TempStorage.GetData(MxEnvironment);
  TempStorage.GetData(MxCurrDir);
  TempStorage.GetData(TempData);
  MxStartupInfo.Load(TempData);
  TempStorage.GetData(TempData);
  MxShellExecuteInfo.Load(TempData);

  return 1;
}

int CreateProcessElevatedPack::Save(BString &Result)
{
  BStorage TempStorage;
  BString TempData;

  TempStorage.SetStorageType(BStorage::StoreBlock);
  TempStorage.AddData(MxCurrProcessID);
  TempStorage.AddData(MxCurrThreadID);
  TempStorage.AddData(MxTargetFunc);
  TempStorage.AddData(MxApplicationNameExists);
  TempStorage.AddData(MxCommandLineExists);
  TempStorage.AddData(MxApplicationName);
  TempStorage.AddData(MxCommandLine);
  TempStorage.AddData(MxProcAttrExists);
  TempStorage.AddData(MxProcAttrInherit);
  TempStorage.AddData(MxThreadAttrExists);
  TempStorage.AddData(MxThreadAttrInherit);
  TempStorage.AddData(MxInheritHandles);
  TempStorage.AddData(MxCreationFlags);
  TempStorage.AddData(MxEnvironment);
  TempStorage.AddData(MxCurrDir);
  MxStartupInfo.Save(TempData);
  TempStorage.AddData(TempData);
  MxShellExecuteInfo.Save(TempData);
  TempStorage.AddData(TempData);
  TempStorage.Finalize();
  Result = TempStorage.Save();

  return 1;
}


class CreateProcessResultPack
{
public:
  CreateProcessResultPack();

  int Load(const BString &Data);
  int Save(BString &Result);

  int MxRetCode;
  UInt64 MxErrorCode;
  UInt64 MxProcessID, MxThreadID;
};

CreateProcessResultPack::CreateProcessResultPack()
{
  MxRetCode = 0;
  MxErrorCode = 0;
  MxProcessID = 0;
  MxThreadID = 0;
}

int CreateProcessResultPack::Load(const BString &Data)
{
  BStorage TempStorage;
  BString TempData;

  TempStorage.Load(Data);
  TempStorage.GetData(TempData);
  MxRetCode = TempData;
  TempStorage.GetData(TempData);
  MxErrorCode = TempData;
  TempStorage.GetData(TempData);
  MxProcessID = TempData;
  TempStorage.GetData(TempData);
  MxThreadID = TempData;

  return 1;
}

int CreateProcessResultPack::Save(BString &Result)
{
  BStorage TempStorage;

  TempStorage.SetStorageType(BStorage::StoreBlock);
  TempStorage.AddData(MxRetCode);
  TempStorage.AddData(MxErrorCode);
  TempStorage.AddData(MxProcessID);
  TempStorage.AddData(MxThreadID);
  TempStorage.Finalize();
  Result = TempStorage.Save();

  return 1;
}
