#ifndef GLOBAL_H
#define GLOBAL_H

#include <windows.h>
#include "API/EterPackAPI.hpp"
#include "Global/Version.h"

extern const char *g_protocol;
extern const char *g_website;
extern const char *g_ads_addr;
extern const char *g_info_addr;

extern char g_index_ext[5];
extern char g_pack_ext[5];

extern UINT32 g_index_key[4];
extern UINT32 g_pack_key[4];

extern STORAGE_TYPE g_storage_type;

extern bool g_show_ads;

#endif // GLOBAL_H
