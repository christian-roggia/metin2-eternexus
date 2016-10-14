#include <Windows.h>
#include <fstream>
#include "log.h"

namespace FastIO {
	bool FileIsEmtpy(const TCHAR *szFilename);
	void FileRead(const char *szFilename, const char *szMode, void **pOutput, UINT32 *iFileSize);
	void FileWrite(const char *szFilename, const char *szMode, void *pInput, UINT32 iFileSize);
}