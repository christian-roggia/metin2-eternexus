/******************************************************************************************
  Copyright 2012 Christian Roggia

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
******************************************************************************************/

#include "EterIndex.h"

EterIndex::EterIndex(const char *szFilename, const UINT32 *pKey)
 : EIX_Header(0), EIX_Info(0), pItemsList(0), pDecryptedData(0), pDecompressedData(0), iIndex(0), iPointer(0), szEterPackFilename(szFilename), vKey(pKey)
{
	if(this->vKey == NULL) this->vKey = g_default_index_key;
}


EterIndex::~EterIndex()
{
	if(this->pItemsList)
	{
		for(UINT32 i = 0; i < this->EIX_Info->FileCount; i++)
			free(this->pItemsList[i]);

		free(this->pItemsList);
	}
	
	if(this->pDecompressedData) free(this->pDecompressedData);
    if(this->pDecryptedData)    free(this->pDecryptedData);
	if(this->EIX_Header)        free(this->EIX_Header);

	this->EIX_Header = 0;
	this->EIX_Info = 0;
	this->pItemsList = 0;
	this->pDecryptedData = 0;
	this->pDecompressedData = 0;
	this->iIndex = 0;
    this->iPointer = 0;
	this->szEterPackFilename = 0;
	this->vKey = 0;
}

bool EterIndex::LoadFile()
{
	UINT8 *pRawData;
	UINT32 iFileSize;
	
	FastIO::FileRead(this->szEterPackFilename, "rb", (void **)&pRawData, &iFileSize);
	if(pRawData == NULL || iFileSize == 0)              return false;
	
	if(!this->Decrypt(pRawData, this->vKey, iFileSize)) return false;
	if(!this->Decompress())                             return false;
	
	return true;
}

bool EterIndex::Decrypt(UINT8 *aRawData, const UINT32 *vKey, UINT32 iSize, bool bFreeData)
{
	this->EIX_Header = (EterIndexHeader *)memcpy(malloc(sizeof(EterIndexHeader)), aRawData, sizeof(EterIndexHeader));

    if(this->EIX_Header->Mark != MARK_VALID_ETER_INDEX)
    {
        sys_err
		(
			__FILE__,
			__LINE__,
			"The file's mark doesn't match with the default EterIndex's mark (0x%08X != 0x%08X)!",
			this->EIX_Header->Mark,
			MARK_VALID_ETER_INDEX
		);
		
        return false;
    }

	EterPackDecrypt((UINT32 *)(aRawData + sizeof(EterIndexHeader)), (UINT32 *)vKey, iSize - sizeof(EterIndexHeader));
	
	this->pDecryptedData = (UINT8 *)memcpy(malloc(iSize - sizeof(EterIndexHeader)), aRawData + sizeof(EterIndexHeader), iSize - sizeof(EterIndexHeader));
	if(bFreeData) free(aRawData);

    if(*(UINT32 *)this->pDecryptedData != MARK_DECRYPTION_SUCCESS)
    {
        sys_err
		(
			__FILE__,
			__LINE__,
			"The decryption failed, the decrypted mark doesn't match with the default post-decryption's mark (0x%08X != 0x%08X)!",
			*(UINT32 *)this->pDecryptedData,
			MARK_DECRYPTION_SUCCESS
		);
		
        return false;
    }
    
	sys_log(0, "The EterIndex has been successfully decrypted (%d bytes).", iSize - sizeof(EterIndexHeader));
    return true;
}

bool EterIndex::Decompress(void)
{
    lzo_uint iOutSizeLZO = 0;

    if(this->pDecryptedData == NULL)
    {
        sys_err(__FILE__, __LINE__, "Data is NULL!");
        return false;
    }
    else if(this->EIX_Header == 0)
    {
        sys_err(__FILE__, __LINE__, "Object-Info is NULL!");
        return false;
    }
    else if(this->EIX_Header->DecompressedSize == 0)
    {
        sys_err(__FILE__, __LINE__, "Object-Info's decompressed size is zero!");
        return false;
    }
    else if(this->EIX_Header->CompressedSize == 0)
    {
        sys_err(__FILE__, __LINE__, "Object-Info compressed size is zero!");
        return false;
    }

    this->pDecompressedData = (unsigned char *)calloc(this->EIX_Header->DecompressedSize, sizeof(unsigned char));

    lzo1x_decompress(this->pDecryptedData + sizeof(UINT32), this->EIX_Header->CompressedSize, this->pDecompressedData, &iOutSizeLZO, 0);
    
    if(iOutSizeLZO < this->EIX_Header->DecompressedSize)
    {
        sys_err(__FILE__, __LINE__, "EterIndex decompression failed! Final decompressed size is too small [%d < %d]!", iOutSizeLZO, this->EIX_Header->DecompressedSize);
        return false;
    }
    else if(iOutSizeLZO > this->EIX_Header->DecompressedSize)
    {
        sys_err(__FILE__, __LINE__, "[WARNING] LZO decompressed size > Object-Info decompressed size!");
    }
    
    this->EIX_Info = reinterpret_cast<EterIndexInfo *>(this->pDecompressedData);

    if(this->EIX_Info->Mark != MARK_DECOMPRESSION_SUCCESS)
    {
        sys_err(__FILE__, __LINE__, "EterIndex decompression failed! Mark 0x%08X != 0x%08X!\n", this->EIX_Info->Mark, MARK_DECOMPRESSION_SUCCESS);
        return false;
    }
    else if(this->EIX_Info->Version != 2)
    {
        sys_err(__FILE__, __LINE__, "Invalid EterIndex Version! Version %d != 2!", this->EIX_Info->Version);
        return false;
    }
    else if(iOutSizeLZO != this->EIX_Header->DecompressedSize)
    {
        sys_err(__FILE__, __LINE__, "Decompressed Size (%d) and EterIndex Decompressed (%d) size differs!", iOutSizeLZO, this->EIX_Header->DecompressedSize);
        return false;
    }
	else if(iOutSizeLZO < this->GetFileDataSize() + sizeof(EterIndexInfo))
    {
        sys_err(__FILE__, __LINE__, "Decompressed Size (%d) should be %d bytes long at least!", iOutSizeLZO, this->GetFileDataSize() + sizeof(EterIndexInfo));
        return false;
    }
	else if(this->EIX_Info->FileCount == 0)
	{
        sys_err(__FILE__, __LINE__, "The EterIndex is empty!");
        return false;
    }
	else if(this->EIX_Info->FileCount * sizeof(EterIndexItem) + sizeof(EterIndexInfo) > this->EIX_Header->DecompressedSize)
	{
		sys_err(__FILE__, __LINE__, "Required %d bytes, file is only %d bytes long!", this->EIX_Info->FileCount * sizeof(EterIndexItem) + sizeof(EterIndexInfo), this->EIX_Header->DecompressedSize);
        return false;
	}
    
    sys_log(0, "EterIndex decompressed successfully [%d bytes, %d file(s)]", this->EIX_Header->DecompressedSize, this->EIX_Info->FileCount);
    return true;
}

EterIndexItem *EterIndex::GetNextFile(void)
{
    if(this->EIX_Info->FileCount < this->iPointer)
    {
        ResetPointer();
        return 0;
    }

    return (EterIndexItem *)(this->pDecompressedData + this->iPointer++ * sizeof(EterIndexItem) + sizeof(EterIndexInfo));
}

EterIndexItem **EterIndex::GetAllFiles(void)
{
	if(this->pItemsList) return this->pItemsList;

	this->ResetPointer();
	this->pItemsList = (EterIndexItem **)malloc(sizeof(EterIndexItem *) * this->EIX_Info->FileCount);

	for(UINT32 i = 0; i < this->EIX_Info->FileCount; i++)
		pItemsList[i] = (EterIndexItem *)memcpy(malloc(sizeof(EterIndexItem)), this->GetNextFile(), sizeof(EterIndexItem));

	this->ResetPointer();
	return pItemsList;
}

void EterIndex::InitPack(UINT32 iFileCount, UINT32 iVersion)
{
    UINT32 iRawSize = sizeof(EterIndexItem) * iFileCount + sizeof(EterIndexInfo);

    this->pDecompressedData = (UINT8 *)calloc(iRawSize, 1);
	this->EIX_Info = (EterIndexInfo *)this->pDecompressedData;

	this->EIX_Info->Mark      = MARK_DECOMPRESSION_SUCCESS;
	this->EIX_Info->Version   = iVersion;
	this->EIX_Info->FileCount = iFileCount;

	this->iIndex   = 0;
	this->iPointer = 0;
}

void EterIndex::PutFile(const char *szFilename, enum STORAGE_TYPE eType, UINT32 iFileSize, UINT32 iHashCRC32)
{
	if(this->EIX_Info->FileCount <= this->iIndex)
	{
		sys_err(__FILE__, __LINE__, "Array out of range: [size: %d index: %d]", this->EIX_Info->FileCount, this->iIndex);
		return;
	}
	
    EterIndexItem hEterIndexItem;

    hEterIndexItem.Index             = this->iIndex;
    hEterIndexItem.UnkZeroDword      = 0;
    hEterIndexItem.VirtualPathCRC32  = GetCRC32(szFilename, strlen(szFilename));
    hEterIndexItem.Offset            = this->iPointer;
    hEterIndexItem.Size              = iFileSize;
    hEterIndexItem.StoredSize        = iFileSize - (iFileSize % 256) + 256;
    hEterIndexItem.CRC32             = iHashCRC32;
    hEterIndexItem.StorageType       = eType;
	strncpy(hEterIndexItem.VirtualPath, szFilename, 160);

    this->iPointer += iFileSize - (iFileSize % 256) + 256;
    memcpy(this->pDecompressedData + (sizeof(EterIndexItem) * this->iIndex++) + sizeof(EterIndexInfo), &hEterIndexItem, sizeof(EterIndexItem));
}

void EterIndex::WritePack(void)
{
    EterIndexHeader m_info;
	UINT32 iCompressedSize = 0, iEncryptedSize = 0, iEterPackSize = 0;
	UINT8 *pCompressedData, *pEterPackData, *pEncryptedData;
	int iCompressStatus = 0;

    /************************************
    * Compress
    ************************************/
	
	iCompressStatus = EterPackCompress(this->pDecompressedData, this->GetFileDataSize() + sizeof(EterIndexInfo), &pCompressedData, &iCompressedSize);
	if(iCompressStatus < 0)
	{
		sys_err(__FILE__, __LINE__, "LZO compression error [%d]", iCompressStatus);
		return;
	}

    iEncryptedSize = iCompressedSize + sizeof(UINT32);
    iEncryptedSize = iEncryptedSize - (iEncryptedSize % 8) + 8;
	
    pEncryptedData = (UINT8 *)calloc(iEncryptedSize, 1);
	
	(*(UINT32 *)pEncryptedData) = MARK_DECRYPTION_SUCCESS;
    memcpy(pEncryptedData + 4, pCompressedData, iCompressedSize);
	
    if(pCompressedData) free(pCompressedData);

    /************************************
    * Encrypt
    ************************************/
	
	EterPackEncrypt((UINT32 *)pEncryptedData, (UINT32 *)this->vKey, iEncryptedSize);

	iEterPackSize = iEncryptedSize + sizeof(EterIndexHeader);
	pEterPackData = (UINT8 *)malloc(iEterPackSize);
	
    memcpy(pEterPackData + sizeof(EterIndexHeader), pEncryptedData, iEncryptedSize);
	
	if(pEncryptedData) free(pEncryptedData);
	
    /************************************
    * Put Header Data
    ************************************/
	
    m_info.Mark             = MARK_VALID_ETER_INDEX;
    m_info.DecompressedSize = this->GetFileDataSize() + sizeof(EterIndexInfo);
    m_info.DecryptedSize    = iEncryptedSize;
    m_info.CompressedSize   = iCompressedSize;
	memcpy(pEterPackData, (void *)&m_info, sizeof(EterIndexHeader));
	
	FastIO::FileWrite(this->szEterPackFilename, "wb", pEterPackData, iEterPackSize);
	free(pEterPackData);
}
