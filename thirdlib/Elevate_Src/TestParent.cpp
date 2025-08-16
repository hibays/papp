#include "CoreLib.h"
//#include <vld.h>

int main(int argc, char **argv, char **envp)
{
  GxApp.Init(argc, argv, envp);

  BString TempData;
  Process TempProcess;
  Process::ProcessData TempProcessData;

  TempData = System::AppDirectory("") + "TestChild.exe";

  if (!TempProcessData.Extract(TempData))
  {
    printf("Error extracting information.\n");

    return 1;
  }

  // Run the TestChild process with forced elevation,
  // waiting for it to complete,
  // attached to this console,
  // and pipe stderr to internal buffers.
  if (!TempProcess.StartProcess(TempProcessData.Command, TempProcessData.Argv, TempProcessData.Envp, TempProcessData.Directory, BString(), Process::Foreground, Process::Standard, Process::Piped, Process::NoDetachProcess, Process::ForceElevate))
  {
    printf("Error running process.\n");

    return 1;
  }

  // Data should have come across the named pipe for stderr.
  printf("Data from TestChild.exe:  \"%s\"", *TempProcess.ReadStderr());

  return 0;
}
