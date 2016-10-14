#include "Global.h"

const char *g_protocol  = "http://";
const char *g_website   = "Crysus.net";
const char *g_ads_addr  = "x/ads";
const char *g_info_addr = "x/eternexus/info";

char g_index_ext[5] = {'.', 'e', 'i', 'x', '\0'};
char g_pack_ext[5]  = {'.', 'e', 'p', 'k', '\0'};

UINT32 g_index_key[4] = {0x02b09eb9, 0x0581696f, 0x289b9863, 0x001a1879};
UINT32 g_pack_key[4]  = {0x04b4b822, 0x1f6eb264, 0x0018eaae, 0x1cfbf6a6};

STORAGE_TYPE g_storage_type = LZO_COMPRESSION_NO_ENCODING;

bool g_show_ads = true;
