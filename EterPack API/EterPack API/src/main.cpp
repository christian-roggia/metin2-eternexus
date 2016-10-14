#include "windows.h"
#include "EterIndex.h"
#include "EterPack.h"

#pragma comment(lib, "lzo")

#define DLL_EXPORT extern "C" __declspec(dllexport)
#define PLIST EterIndexItem **
#define EINSTANCE UINT32
#define MODE_READ  0
#define MODE_WRITE 1

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch(fdwReason) 
    { 
        case DLL_PROCESS_ATTACH:
			return log_init() ? TRUE : FALSE;
            break;
        case DLL_PROCESS_DETACH:
			log_destroy();
            break;
    }

    return TRUE;
}

DLL_EXPORT const char *API_Copyright(void)
{
	return "EterPack API © 2013-2014, Crysus Technologies";
}

DLL_EXPORT const char *API_Version(void)
{
	return "1.2.0 Alpha 2 (Full version)";
}

DLL_EXPORT const char *API_Author(void)
{
	return "Crysus Technologies";
}

DLL_EXPORT EINSTANCE LoadEterIndex(const char *szFilename, const UINT32 *vKey, int iMode)
{
	sys_log(0, "====================== NEW INDEX CLASS ======================");

	EterIndex *pEixClass = new EterIndex(szFilename, vKey);

	if(!pEixClass) return 0;
	if(iMode == MODE_READ)
	{
		if(!pEixClass->LoadFile())
		{
			delete pEixClass;
			sys_log(1, "FAILED TO LOAD FILE %s", szFilename);
			return 0;
		}
	}
	
	sys_log(0, "ISTANCE AT 0x%p", pEixClass);
	sys_log(0, "ABSOLUTE PATH: %s", szFilename);
	
	return (EINSTANCE)pEixClass;
}

DLL_EXPORT PLIST GetFilesList(EINSTANCE pInstance)
{
	return (pInstance != 0) ? ((EterIndex *)pInstance)->GetAllFiles() : 0;
}

DLL_EXPORT UINT32 GetFileCount(EINSTANCE pInstance)
{
	return (pInstance != 0) ? ((EterIndex *)pInstance)->GetFileCount() : 0;
}

DLL_EXPORT void InitPack(EINSTANCE pInstance, UINT32 iFileCount)
{
	if(pInstance == 0) return;
	
	((EterIndex *)pInstance)->InitPack(iFileCount);
}

DLL_EXPORT void PutFile(EINSTANCE pInstance, const char *szFilename, enum STORAGE_TYPE eType, UINT32 iFileSize, UINT32 iHashCRC32)
{
	if(pInstance == 0) return;
	
	((EterIndex *)pInstance)->PutFile(szFilename, eType, iFileSize, iHashCRC32);
}

DLL_EXPORT void WritePack(EINSTANCE pInstance)
{
	if(pInstance == 0) return;
	
	((EterIndex *)pInstance)->WritePack();
}

DLL_EXPORT void FreeEterIndex(EINSTANCE pInstance)
{
	if(pInstance == 0) return;
	
	delete ((EterIndex *)pInstance);
}

DLL_EXPORT EINSTANCE LoadEterPack(const char *szFilename, const UINT32 *vKey, int iMode)
{
	EterPack *pEpkClass = new EterPack(szFilename, vKey, iMode == MODE_READ);

	sys_log(0, "====================== NEW PACK CLASS ======================");
	sys_log(0, "ISTANCE AT 0x%p", pEpkClass);
	sys_log(0, "ABSOLUTE PATH: %s", szFilename);
	
	return (EINSTANCE)pEpkClass;
}

DLL_EXPORT UINT8 *GetFileData(EINSTANCE pInstance, EterIndexItem *pEixItem)
{
	return (pInstance != 0) ? ((EterPack *)pInstance)->GetStoredData(pEixItem, true) : 0;
}

DLL_EXPORT UINT32 GetDataSize(EINSTANCE pInstance)
{
	return (pInstance != 0) ? ((EterPack *)pInstance)->GetStoredSize() : 0;
}

DLL_EXPORT bool PutFileData(EINSTANCE pInstance, const char *szFilename, enum STORAGE_TYPE eType, UINT32 *iOutSize, UINT32 *iHashCRC32)
{
	return (pInstance != 0) ? ((EterPack *)pInstance)->PutPack(szFilename, eType, iOutSize, iHashCRC32) : false;
}

DLL_EXPORT void FreeEterPack(EINSTANCE pInstance)
{
	if(pInstance == 0) return;
	
	delete ((EterPack *)pInstance);
}
