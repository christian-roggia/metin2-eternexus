#ifndef ETER_PACK_API_H
#define ETER_PACK_API_H

#include <windows.h>

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;

struct EterIndexItem
{
	UINT32 Index;
	char   VirtualPath[160];
	UINT32 UnkZeroDword;
	UINT32 VirtualPathCRC32;
	UINT32 StoredSize; // to find another name
	UINT32 Size; // to find another name
	UINT32 CRC32;
	UINT32 Offset;
	UINT8  StorageType;
	/* 3 bytes padding */
	UINT8  _PADDING[3];
};

#define PLIST EterIndexItem **
#define EINSTANCE UINT32
#define DLL_IMPORT extern "C" __declspec(dllimport)

enum STORAGE_TYPE
{
    NO_COMPRESSION_NO_ENCODING       = 0x00,
    LZO_COMPRESSION_NO_ENCODING      = 0x01
};

#define MODE_READ  0
#define MODE_WRITE 1

DLL_IMPORT EINSTANCE LoadEterIndex(const char *szFilename, const UINT32 *vKey, int iMode);
DLL_IMPORT PLIST GetFilesList(EINSTANCE pInstance);
DLL_IMPORT UINT32 GetFileCount(EINSTANCE pInstance);
DLL_IMPORT void InitPack(EINSTANCE pInstance, UINT32 iFileCount);
DLL_IMPORT void PutFile(EINSTANCE pInstance, const char *szFilename, enum STORAGE_TYPE eType, UINT32 iFileSize, UINT32 iHashCRC32);
DLL_IMPORT void WritePack(EINSTANCE pInstance);
DLL_IMPORT void FreeEterIndex(EINSTANCE pInstance);

DLL_IMPORT EINSTANCE LoadEterPack(const char *szFilename, const UINT32 *vKey, int iMode);
DLL_IMPORT UINT8 *GetFileData(EINSTANCE pInstance, EterIndexItem *pEixItem);
DLL_IMPORT UINT32 GetDataSize(EINSTANCE pInstance);
DLL_IMPORT bool PutFileData(EINSTANCE pInstance, const char *szFilename, enum STORAGE_TYPE eType, UINT32 *iOutSize, UINT32 *iHashCRC32);
DLL_IMPORT void FreeEterPack(EINSTANCE pInstance);

#endif
