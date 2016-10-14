#include <Windows.h>
#include <fstream>
#include "log.h"

typedef unsigned char UINT8;

namespace FastIO {
	void FileRead(const char *szFilename, const char *szMode, void **pOutput, UINT32 *iFileSize);
	void FileWrite(const char *szFilename, const char *szMode, void *pInput, UINT32 iFileSize);
}
