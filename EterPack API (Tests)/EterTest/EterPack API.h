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

enum STORAGE_TYPE
{
    NO_COMPRESSION_NO_ENCODING       = 0x00,
    LZO_COMPRESSION_NO_ENCODING      = 0x01
};

#define MODE_READ  0
#define MODE_WRITE 1

typedef EINSTANCE (*_tLoadEterIndex)(const char *szFilename, const UINT32 *vKey, int iMode);
typedef PLIST (*_GetFilesList)(EINSTANCE pInstance);
typedef UINT32 (*_GetFileCount)(EINSTANCE pInstance);
typedef void (*_tInitPack)(EINSTANCE pInstance, UINT32 iFileCount);
typedef void (*_tPutFile)(EINSTANCE pInstance, const char *szFilename, enum STORAGE_TYPE eType, UINT32 iFileSize, UINT32 iHashCRC32);
typedef void (*_tWritePack)(EINSTANCE pInstance);
typedef void (*_tFreeEterIndex)(EINSTANCE pInstance);

typedef EINSTANCE (*_tLoadEterPack)(const char *szFilename, const UINT32 *vKey, int iMode);
typedef UINT8 *(*_tGetFileData)(EINSTANCE pInstance, EterIndexItem *pEixItem);
typedef UINT32 (*_tGetDataSize)(EINSTANCE pInstance);
typedef bool (*_tPutFileData)(EINSTANCE pInstance, const char *szFilename, enum STORAGE_TYPE eType, UINT32 *iOutSize, UINT32 *iHashCRC32);
typedef void (*_tFreeEterPack)(EINSTANCE pInstance);

namespace EterPackAPI
{
    HMODULE hAPI;

    _tLoadEterIndex LoadEterIndex;
    _GetFilesList   GetFilesList;
    _GetFileCount   GetFileCount;
    _tInitPack      InitPack;
    _tPutFile       PutFile;
    _tWritePack     WritePack;
    _tFreeEterIndex FreeEterIndex;

    _tLoadEterPack LoadEterPack;
    _tGetFileData  GetFileData;
    _tGetDataSize  GetDataSize;
    _tPutFileData  PutFileData;
    _tFreeEterPack FreeEterPack;

    void LoadEterPackAPI(void)
    {
        hAPI = LoadLibraryA("EterPack API.dll");

        LoadEterIndex = (_tLoadEterIndex)GetProcAddress(hAPI, "LoadEterIndex");
        GetFilesList  = (_GetFilesList)GetProcAddress(hAPI, "GetFilesList");
        GetFileCount  = (_GetFileCount)GetProcAddress(hAPI, "GetFileCount");
        InitPack      = (_tInitPack)GetProcAddress(hAPI, "InitPack");
        PutFile       = (_tPutFile)GetProcAddress(hAPI, "PutFile");
        WritePack     = (_tWritePack)GetProcAddress(hAPI, "WritePack");
        FreeEterIndex = (_tFreeEterIndex)GetProcAddress(hAPI, "FreeEterIndex");

        LoadEterPack  = (_tLoadEterPack)GetProcAddress(hAPI, "LoadEterPack");
        GetFileData   = (_tGetFileData)GetProcAddress(hAPI, "GetFileData");
        GetDataSize   = (_tGetDataSize)GetProcAddress(hAPI, "GetDataSize");
        PutFileData   = (_tPutFileData)GetProcAddress(hAPI, "PutFileData");
        FreeEterPack  = (_tFreeEterPack)GetProcAddress(hAPI, "FreeEterPack");
    }

    void FreeEterPackAPI(void)
    {
        FreeLibrary(hAPI);
    }
}
