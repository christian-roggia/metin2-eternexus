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

#ifndef ETERINDEX_H
#define ETERINDEX_H

#include <windows.h>
#include "CRC32.h"
#include "EterInfo.h"
#include "EterUtils.h"
#include "log.h"
#include "io.h"

class EterIndex
{
public:
    EterIndex(const char *szFilename, const UINT32 *vKey = NULL);
    ~EterIndex();

	bool LoadFile();
	
    bool Decrypt(UINT8 *aRawData, const UINT32 *vKey, UINT32 iSize, bool bFreeData = true);
    bool Decompress(void);

    unsigned char *GetDecryptedData(void) const    { return this->pDecryptedData; }
    unsigned char *GetDecompressedData(void) const { return this->pDecompressedData; }
    
    UINT32 GetMark(void) const                     { return this->EIX_Header->Mark; }
    UINT32 GetDecryptedSize(void) const            { return this->EIX_Header->DecryptedSize; }
    UINT32 GetCompressedSize(void) const           { return this->EIX_Header->CompressedSize; }
    UINT32 GetDecompressedSize(void) const         { return this->EIX_Header->DecompressedSize; }
    
    UINT32 GetFileDataSize(void) const             { return this->EIX_Info->FileCount * sizeof(EterIndexItem); }
    UINT32 GetFileCount(void) const                { return this->EIX_Info->FileCount; }
    UINT32 GetVersion(void) const                  { return this->EIX_Info->Version; }
    UINT32 GetDecompressionMark(void) const        { return this->EIX_Info->Mark; }

    void ResetPointer(void)                        { this->iPointer = 0; }
	void SetPointer(UINT32 iNewPointer)            { this->iPointer = iNewPointer; }
	
    EterIndexItem *GetNextFile(void);
	EterIndexItem **GetAllFiles(void);
	
	void InitPack(UINT32 iFileCount, UINT32 iVersion = 2);
	void PutFile(const char *szFilename, enum STORAGE_TYPE eType, UINT32 iFileSize, UINT32 iHashCRC32);
	void WritePack(void);
	
	static UINT8 *Compress(UINT8 *pRawData, UINT32 iRawSize, UINT32 *iCompressedSize);
	
private:
    EterIndexHeader *EIX_Header;
	EterIndexInfo *EIX_Info;
	EterIndexItem **pItemsList;
	
    UINT8 *pDecryptedData, *pDecompressedData;
    UINT32 iIndex, iPointer;
	
	
	const char *szEterPackFilename;
	const UINT32 *vKey;
};

#endif // ETERINDEX_H
