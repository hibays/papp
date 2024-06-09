#include "CoreLib.h"
//#include <vld.h>

int main(int argc, char **argv, char **envp)
{
  GxApp.Init(argc, argv, envp);

  // stdout should be the original process' console.
  // stderr should be piped to the original process.
  fprintf(stdout, "TestChild:  Yo.\n");
  fprintf(stderr, "Booya!");

  return 0;
}
