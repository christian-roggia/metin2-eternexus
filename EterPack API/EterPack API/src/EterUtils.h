#ifndef ETER_UTILS_H
#define ETER_UTILS_H

#include <windows.h>
#include "lzo\lzo1x.h"

#define COMP_BUF_CAL(in_buf_size) in_buf_size + in_buf_size / 16 + 64 + 3 + 4

int EterPackDecrypt(UINT32 *aRawData, UINT32 *vKey, UINT32 iSize);
int EterPackEncrypt(UINT32 *aRawData, UINT32 *vKey, UINT32 iSize);



#define ETER_PACK_COMPRESS_EMPTY_DATA_ERROR -1

int EterPackCompress(UINT8 *pRawData, UINT32 iRawSize, UINT8 **pCompressedData, UINT32 *iCompressedSize);

#endif // ETER_UTILS_H
