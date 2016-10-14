#include "EterUtils.h"

void XTEA_DECRYPT(UINT32 *v, UINT32 *k)
{
    UINT32 sum = 0xC6EF3720, delta = 0x61C88647;
    UINT32 v0 = v[0], v1 = v[1];

    for (int i = 0; i < 32; i++)
    {
        v1 -= (v0 + (16 * v0 ^ (v0 >> 5))) ^ (sum + k[(sum >> 11) & 3]);
        sum += delta;
        v0 -= (v1 + (16 * v1 ^ (v1 >> 5))) ^ (sum + k[sum & 3]);
    }

    v[0] = v0; v[1] = v1;
}

void XTEA_ENCRYPT(UINT32 *v, UINT32 *k)
{
    UINT32 sum = 0, delta = 0x61C88647;
    UINT32 v0 = v[0], v1 = v[1];

    for (int i = 0; i < 32; i++)
    {
        v0 += (v1 + (16 * v1 ^ (v1 >> 5))) ^ (sum + k[sum & 3]);
        sum -= delta;
        v1 += (v0 + (16 * v0 ^ (v0 >> 5))) ^ (sum + k[(sum >> 11) & 3]);
    }

    v[0] = v0; v[1] = v1;
}

int EterPackDecrypt(UINT32 *aRawData, UINT32 *vKey, UINT32 iSize)
{
    if(aRawData == NULL || vKey == NULL || iSize == 0) return -1;
    if(iSize % 8)  iSize = iSize - (iSize % 8);

    for(UINT32 i = 0; i < iSize / 8; i++)
        XTEA_DECRYPT(&aRawData[i * 2], vKey);

	return 0;
}

int EterPackEncrypt(UINT32 *aRawData, UINT32 *vKey, UINT32 iSize)
{
    if(aRawData == NULL || vKey == NULL || iSize == 0) return -1;
    if(iSize % 8)  iSize = iSize - (iSize % 8) + 8;

    for(UINT32 i = 0; i < iSize / 8; i++)
        XTEA_ENCRYPT(&aRawData[i * 2], vKey);

	return 0;
}

int EterPackCompress(UINT8 *pRawData, UINT32 iRawSize, UINT8 **pCompressedData, UINT32 *iCompressedSize)
{
	if(pRawData == NULL || iRawSize == 0) return ETER_PACK_COMPRESS_EMPTY_DATA_ERROR;
	
	void *pWorkMemory = (lzo_voidp)calloc(LZO1X_999_MEM_COMPRESS, 1);
    *pCompressedData = (unsigned char *)calloc(COMP_BUF_CAL(iRawSize), 1);

	lzo_init();
    lzo1x_999_compress(pRawData, iRawSize, *pCompressedData, (lzo_uint *)iCompressedSize, pWorkMemory);

    if(*iCompressedSize <= 0)    return -2;
    if(*pCompressedData == NULL) return -3;

    if(pWorkMemory)           free(pWorkMemory);
	
	return 0;
}