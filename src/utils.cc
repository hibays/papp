#include <windows.h>
#include <winnt.h>

#include "utils.hh"

Path program_location() {
	char *path = nullptr;
	if (_get_pgmptr(&path) == 0) {
		return path;
	}
	char _execpath[680] = {0};
	GetModuleFileName(NULL, _execpath, 679);
	return _execpath;
}
