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

#ifndef ETER_INFO_H
#define ETER_INFO_H

#include <windows.h>

#define	MARK_MIPX = 0x5850494D, // MIPX
#define	MARK_MMPT = 0x54504D4D, // MMPT
#define	MARK_MIPT = 0x5450494D  // MIPT

#define	MARK_VALID_ETER_PACK       0x5A4F434D // MCOZ
#define	MARK_VALID_ETER_INDEX      0x5A4F434D // MCOZ
#define	MARK_DECRYPTION_SUCCESS    0x5A4F434D // MCOZ
#define	MARK_DECOMPRESSION_SUCCESS 0x444b5045 // EPKD

enum STORAGE_TYPE
{
	NO_COMPRESSION_NO_ENCODING       = 0x00,
	LZO_COMPRESSION_NO_ENCODING      = 0x01,
	LZO_COMPRESSION_XTEA_ENCODING    = 0x02,
	
	/* =======>> NOT SOPPORTED YET <<======= */
	/* --> Panama encoding confirmed, unknown key <-- */
	NOT_SUPPORTED_PANAMA_ENCODING    = 0x03,
	/* --> Storage type confirmed, unknown algorithm, unknown key <-- */
	NOT_SUPPORTED_UNKNOWN_ENCODING_1 = 0x04, // 
	NOT_SUPPORTED_UNKNOWN_ENCODING_2 = 0x05  // 
};

const UINT32 g_default_index_key[4] = {0x02B09EB9, 0x0581696F, 0x289B9863, 0x001A1879};
const UINT32 g_default_pack_key[4]  = {0x04B4B822, 0x1F6EB264, 0x0018EAAE, 0x1CFBF6A6};

struct EterIndexHeader
{
	UINT32 Mark;
	UINT32 DecryptedSize;
	UINT32 CompressedSize;
	UINT32 DecompressedSize;
};

struct EterIndexInfo
{
	UINT32 Mark;
	UINT32 Version;
	UINT32 FileCount;
};

struct EterIndexItem
{
	UINT32 Index;
	char   VirtualPath[160];
	UINT32 UnkZeroDword;
	UINT32 VirtualPathCRC32;
	UINT32 Size;
	UINT32 StoredSize;
	UINT32 CRC32;
	UINT32 Offset;
	UINT8  StorageType;
	/* 3 bytes padding */
	UINT8  _PADDING[3];
};

struct EterPackHeader
{
	UINT32 Mark;
	UINT32 EncryptedSize;
	UINT32 CompressedSize;
	UINT32 DecompressedSize;
};

#endif // ETER_INFO_H