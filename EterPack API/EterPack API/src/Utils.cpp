#include "Utils.h"

DWORD GetModuleDirectory(DWORD nSize, LPTSTR lpFilename)
{
	DWORD nPathSize = GetModuleFileName(NULL, lpFilename, nSize);
	
	if(nPathSize == 0)
		return 0;
	
	lpFilename[nPathSize] = 0;

	for(DWORD i = nPathSize; i >= 0; i--)
	{
		if(lpFilename[i] == '\\' || lpFilename[i] == '/')
		{
			lpFilename[i] = '\0';
			nPathSize = i;
			
			break;
		}
	}
	
	return nPathSize;
}