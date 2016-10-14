#ifndef ETER_PACK_H
#define ETER_PACK_H

#include "CRC32.h"
#include "EterInfo.h"
#include "EterUtils.h"
#include "EterIndex.h"
#include "log.h"
#include "io.h"

class EterPack
{
public:
    EterPack(const char *szEPKName, const UINT32 *pKey, bool bReadFile = false);
    ~EterPack();
	
	UINT8 *GetStoredData(EterIndexItem *pEIXItem, bool bGenerateDump = false);
	UINT32 GetStoredSize(void) { return this->iDataSize; }
	
	static UINT8 *Decrypt(EterPackHeader *pEpkHeader, const UINT32 *vKey, UINT8 *pPureEterData);
	static UINT8 *Decompress(EterPackHeader *pEpkHeader, UINT8 *pEterData);

	bool PutPack(const char *szFilename, enum STORAGE_TYPE eType, UINT32 *iOutSize, UINT32 *iHashCRC32);
	
private:
	const char *szEterPackFilename;
	UINT8 *aEterPackData, *pLastEterData;
	UINT32 iEterPackSize, iDataSize;
	const UINT32 *vKey;
};

#endif // ETER_PACK_H
