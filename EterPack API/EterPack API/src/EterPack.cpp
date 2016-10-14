#include "EterPack.h"

EterPack::EterPack(const char *szEPKName, const UINT32 *pKey, bool bReadFile)
	: szEterPackFilename(szEPKName), aEterPackData(0), pLastEterData(0), iEterPackSize(0), iDataSize(0), vKey(pKey)
{
	if(bReadFile) FastIO::FileRead(this->szEterPackFilename, "rb", (void **)&this->aEterPackData, &this->iEterPackSize);
	
	if(this->vKey == NULL) vKey = g_default_pack_key;
}

EterPack::~EterPack()
{
	if(this->aEterPackData) free(this->aEterPackData);
	if(this->pLastEterData) free(this->pLastEterData);
}

UINT8 *EterPack::GetStoredData(EterIndexItem *pEIXItem, bool bGenerateDump)
{
	if(this->aEterPackData == NULL || this->iEterPackSize == 0 || pEIXItem == NULL) return 0;
	
	EterPackHeader *EPK_Header = 0;
	UINT8 *pElaboratedData = 0;
	
	if(this->pLastEterData) free(this->pLastEterData);
	this->iDataSize = 0;
	this->pLastEterData = 0;

	if(pEIXItem->Offset + pEIXItem->StoredSize > this->iEterPackSize) return 0;
	sys_log(1, "pEIXItem = [%d, %s, %d, %d, %d, %d, %d, %d, %d]",
		pEIXItem->Index, pEIXItem->VirtualPath, pEIXItem->UnkZeroDword, pEIXItem->VirtualPathCRC32,
		pEIXItem->Size, pEIXItem->StoredSize, pEIXItem->CRC32, pEIXItem->Offset, pEIXItem->StorageType);
	
	switch(pEIXItem->StorageType)
	{
	case NO_COMPRESSION_NO_ENCODING:
		this->iDataSize = pEIXItem->StoredSize;
		this->pLastEterData = (UINT8 *)memcpy(malloc(pEIXItem->StoredSize), this->aEterPackData + pEIXItem->Offset, pEIXItem->StoredSize);
		break;
	case LZO_COMPRESSION_NO_ENCODING:
		EPK_Header = (EterPackHeader *)(this->aEterPackData + pEIXItem->Offset);

		if(EPK_Header->Mark != MARK_VALID_ETER_PACK)
		{
			sys_err(__FILE__, __LINE__, "Invalid EterPack header (offset: %d file: %s).", pEIXItem->Offset, pEIXItem->VirtualPath);
			return 0;
		}
		else if(EPK_Header->CompressedSize == 0 || EPK_Header->DecompressedSize == 0)
		{
			sys_err(__FILE__, __LINE__, "Invalid EterPack data (offset: %d file: %s) [compressed: %d decompressed: %d].", pEIXItem->Offset, pEIXItem->VirtualPath, EPK_Header->CompressedSize, EPK_Header->DecompressedSize);
			return 0;
		}
		
		pElaboratedData = EterPack::Decompress(EPK_Header, this->aEterPackData + pEIXItem->Offset + sizeof(EterPackHeader) + sizeof(UINT32));
		if(!pElaboratedData)
		{
			sys_err(__FILE__, __LINE__, "Pack decompression failed (offset: %d file: %s).", pEIXItem->Offset, pEIXItem->VirtualPath);
			return 0;
		}
		
		this->iDataSize = EPK_Header->DecompressedSize;
		this->pLastEterData = pElaboratedData;
		break;
	case LZO_COMPRESSION_XTEA_ENCODING:
		EPK_Header = (EterPackHeader *)(this->aEterPackData + pEIXItem->Offset);

		if(EPK_Header->Mark != MARK_VALID_ETER_PACK)
		{
			sys_err(__FILE__, __LINE__, "Invalid EterPack header (offset: %d file: %s).", pEIXItem->Offset, pEIXItem->VirtualPath);
			return 0;
		}
		else if(EPK_Header->CompressedSize == 0 || EPK_Header->DecompressedSize == 0 || EPK_Header->EncryptedSize == 0)
		{
			sys_err(__FILE__, __LINE__, "Invalid EterPack data (offset: %d file: %s) [encrypted: %d compressed: %d decompressed: %d].", pEIXItem->Offset, pEIXItem->VirtualPath, EPK_Header->EncryptedSize, EPK_Header->CompressedSize, EPK_Header->DecompressedSize);
			return 0;
		}

		pElaboratedData = EterPack::Decrypt(EPK_Header, this->vKey, this->aEterPackData + pEIXItem->Offset + sizeof(EterPackHeader));
		if(!pElaboratedData)
		{
			sys_err(__FILE__, __LINE__, "Pack decryption failed (offset: %d file: %s).", pEIXItem->Offset, pEIXItem->VirtualPath);
			return 0;
		}
		
		this->iDataSize = EPK_Header->DecompressedSize;
		this->pLastEterData = pElaboratedData;
		break;
	case NOT_SUPPORTED_PANAMA_ENCODING:
		sys_log(0, "Pack encryption: Panama (offset: %d file: %s)", pEIXItem->Offset, pEIXItem->VirtualPath);
		break;
	case NOT_SUPPORTED_UNKNOWN_ENCODING_1:
	case NOT_SUPPORTED_UNKNOWN_ENCODING_2:
	default:
		sys_log(0, "Pack encryption: Unknown (type %d offset: %d file: %s)", pEIXItem->StorageType, pEIXItem->Offset, pEIXItem->VirtualPath);
		break;
	}
	
	if(this->pLastEterData && this->iDataSize > 0) return this->pLastEterData;
	
	this->iDataSize = pEIXItem->StoredSize - sizeof(EterPackHeader);
	this->pLastEterData = (!bGenerateDump) ? 0 : (UINT8 *)memcpy
	(
		malloc(pEIXItem->StoredSize - sizeof(EterPackHeader)),
		this->aEterPackData + pEIXItem->Offset + sizeof(EterPackHeader),
		pEIXItem->StoredSize - sizeof(EterPackHeader)
	);
	
	return this->pLastEterData;
}

UINT8 *EterPack::Decrypt(EterPackHeader *pEpkHeader, const UINT32 *vKey, UINT8 *pPureEterData)
{
	UINT32 iEncryptedSize = pEpkHeader->EncryptedSize;
	UINT8 *pEterData = (UINT8 *)memcpy(malloc(iEncryptedSize), pPureEterData, iEncryptedSize);
	
	if(!pEterData) return 0;
	EterPackDecrypt((UINT32 *)pEterData, (UINT32 *)vKey, iEncryptedSize);
	
	if(*(UINT32 *)pEterData != MARK_DECRYPTION_SUCCESS)
	{
		free(pEterData);
		return 0;
	}

	UINT8 *pRealEterData = EterPack::Decompress(pEpkHeader, pEterData + sizeof(UINT32));
	if(pEterData) free(pEterData);

	return pRealEterData;
}

UINT8 *EterPack::Decompress(EterPackHeader *pEpkHeader, UINT8 *pEterData)
{
	lzo_uint iOutputLZO;
	unsigned char *pOutputData = (unsigned char *)malloc(pEpkHeader->DecompressedSize);
	
	lzo1x_decompress((unsigned char *)pEterData, pEpkHeader->CompressedSize, pOutputData, &iOutputLZO, 0);

	if(pEpkHeader->DecompressedSize != iOutputLZO)
	{
		if(pOutputData) free(pOutputData);
		return 0;
	}
	return pOutputData;
}

bool EterPack::PutPack(const char *szFilename, enum STORAGE_TYPE eType, UINT32 *iOutSize, UINT32 *iHashCRC32)
{
    size_t iFileSize = 0, iPaddedSize = 0;
    UINT8 *pFileData = 0;
    EterPackHeader m_info;
	bool bOK = false;

	if(!FastIO::FileIsEmtpy(szFilename))
	{
		FastIO::FileRead(szFilename, "rb", (void **)&pFileData, &iFileSize);
		if(pFileData == NULL)
		{
			sys_err(__FILE__, __LINE__, "Impossible to read the file: %s", szFilename);
			return false;
		}
	}

	UINT8 *pPaddedData = 0;
	UINT8 *pCompressedData = 0, *pEterPackData = 0;
	UINT32 iCompressedSize = 0, iTotalSize = 0;
	int iCompressStatus = 0;

    switch(eType)
    {
    case NO_COMPRESSION_NO_ENCODING:
        iPaddedSize = iFileSize - (iFileSize % 256) + 256;
        pPaddedData = (UINT8 *)memcpy(calloc(iPaddedSize, 1), pFileData, iFileSize);

        FastIO::FileWrite(this->szEterPackFilename, "ab", pPaddedData, iPaddedSize);

        *iOutSize   = iFileSize;
        *iHashCRC32 = GetCRC32((const char *)pFileData, iFileSize);

        if(pPaddedData) free(pPaddedData);
        if(pFileData)   free(pFileData);

        bOK = true;
        break;
    case LZO_COMPRESSION_NO_ENCODING:
		iCompressStatus = EterPackCompress(pFileData, iFileSize, &pCompressedData, &iCompressedSize);
		if(iCompressStatus < 0 && iCompressStatus != ETER_PACK_COMPRESS_EMPTY_DATA_ERROR)
		{
			sys_err(__FILE__, __LINE__, "LZO compression error [%d]", iCompressStatus);
			return false;
		}

		iTotalSize  = iCompressedSize + sizeof(EterPackHeader) + sizeof(UINT32);
        iPaddedSize = (iTotalSize - (iTotalSize % 256)) + 256;

        pEterPackData = (UINT8 *)calloc(iPaddedSize, 1);

        m_info.Mark             = MARK_VALID_ETER_PACK;
		m_info.EncryptedSize    = 0;
		m_info.CompressedSize   = iCompressedSize;
        m_info.DecompressedSize = iFileSize;

        memcpy(pEterPackData, (char *)&m_info, sizeof(EterPackHeader));
		(*(UINT32 *)(pEterPackData + sizeof(EterPackHeader))) = MARK_VALID_ETER_PACK;
        memcpy(pEterPackData + sizeof(EterPackHeader) + sizeof(UINT32), pCompressedData, iCompressedSize);

        FastIO::FileWrite(this->szEterPackFilename, "ab", pEterPackData, iPaddedSize);

		*iOutSize   = iCompressedSize + sizeof(EterPackHeader) + sizeof(UINT32);
        *iHashCRC32 = GetCRC32((const char *)pEterPackData, iTotalSize);

        if(pEterPackData)   free(pEterPackData);
        if(pCompressedData) free(pCompressedData);
		if(pFileData)       free(pFileData);

        bOK = true;
        break;
    }

    if(!bOK) sys_err(__FILE__, __LINE__, "Put data FROM [%s] failed", szFilename);
	else sys_log(0, "Put data FROM [%s]", szFilename);
	
    return bOK;
}
