#include "log.h"

char log_dir[256], sys_log_header_string[32]; // SIZE TO CHECK
SUnkStruct1 *log_file_sys = NULL, *log_file_err = NULL, *log_file_pt = NULL;
unsigned int log_level_bits = 0;
int log_keep_days = 365;

void log_set_level(int a1)
{
    log_level_bits |= a1;
}

void log_unset_level(int a1)
{
    log_level_bits &= ~a1;
}

void log_set_expiration_days(int a1)
{
    log_keep_days = a1;
}

int log_get_expiration_days(void)
{
    return log_keep_days;
}

void sys_log_header(const char *src)
{
    strncpy(sys_log_header_string, src, 32);
}

void log_file_destroy(SUnkStruct1 *ptr)
{
    if(ptr->Unk0)
    {
        free(ptr->Unk0);
        ptr->Unk0 = 0;
    }

    if(ptr->Unk4)
    {
        fclose(ptr->Unk4);
        ptr->Unk4 = 0;
    }

    free(ptr);
}

void log_destroy(void)
{
    log_file_destroy(log_file_sys);
    log_file_destroy(log_file_err);
    //log_file_destroy(log_file_pt);

    log_file_sys = 0;
    log_file_err = 0;
    //log_file_pt = 0;
}

/*FILE *log_file_check(int a1)
{
    FILE *result; // eax@1
    char buf; // [sp+14h] [bp-64h]@1

    result = (FILE *)stat(*(const char **)a1, (struct stat *)&buf);
    if(result)
    {
        result = (FILE *)__error();
        if(result->_flags == 2)
        {
            fclose(*(FILE **)(a1 + 4));
            result = fopen(*(const char **)a1, "a+");
            *(_DWORD *)(a1 + 4) = result;
        }
    }
    return result;
}*/

SUnkStruct1 *log_file_init(const char *s, const char *modes)
{
    SUnkStruct1 *v2; // ebx@1
    struct tm *v3; // eax@1
    int v4; // edi@1
    FILE *v5; // esi@1
    int v8; // [sp+8h] [bp-20h]@1
    time_t timer; // [sp+18h] [bp-10h]@1

    v2 = 0;
    timer = time(0);
    v3 = localtime(&timer);
    v4 = v3->tm_hour;
    v8 = v3->tm_mday;
    v5 = fopen(s, modes);

    if(v5)
    {
        v2 = (SUnkStruct1 *)malloc(sizeof(SUnkStruct1));

        v2->Unk4 = v5;
        v2->Unk8 = v4;
        v2->Unk0 = strdup(s);
        v2->Unk12 = v8;
    }

    return v2;
}

void pt_log(const char *format, ...)
{
    va_list va; // [sp+34h] [bp+Ch]@1

    va_start(va, format);

    if(log_file_pt)
    {
        vfprintf(log_file_pt->Unk4, format, va);
        fputc(10, log_file_pt->Unk4);
        fflush(log_file_pt->Unk4);
    }
}

void sys_log(int a1, const char *format, ...)
{
    char *v4; // ebx@4
    time_t timer; // [sp+18h] [bp-10h]@4
    va_list va; // [sp+38h] [bp+10h]@1

    va_start(va, format);
    if(!a1 || a1 & log_level_bits)
    {
        if(log_file_sys)
        {
            timer = time(0);
            v4 = asctime(localtime(&timer));

            fprintf(log_file_sys->Unk4, sys_log_header_string);
            v4[strlen(v4) - 1] = 0;

            fprintf(log_file_sys->Unk4, "%-15.15s :: ", v4 + 4);
            vfprintf(log_file_sys->Unk4, format, va);
            fputc(10, log_file_sys->Unk4);
            fflush(log_file_sys->Unk4);

            if(log_level_bits > 1)
            {
                fprintf(stdout, sys_log_header_string);
                vfprintf(stdout, format, va);
                fputc(10, stdout);
                fflush(stdout);
            }
        }
    }
}

/*FILE *log_file_rotate(int a1)
{
    struct tm *v1; // eax@1
    int v2; // ecx@1
    int v3; // edx@1
    int v4; // eax@2
    FILE *result; // eax@3
    char arg; // [sp+20h] [bp-198h]@2
    char path; // [sp+A0h] [bp-118h]@4
    char buf; // [sp+120h] [bp-98h]@4
    __int16 v9; // [sp+128h] [bp-90h]@5
    int src; // [sp+180h] [bp-38h]@1
    int v11; // [sp+184h] [bp-34h]@1
    int v12; // [sp+188h] [bp-30h]@1
    int v13; // [sp+18Ch] [bp-2Ch]@1
    int v14; // [sp+190h] [bp-28h]@1
    int v15; // [sp+194h] [bp-24h]@1
    int v16; // [sp+198h] [bp-20h]@1
    int v17; // [sp+19Ch] [bp-1Ch]@1
    int v18; // [sp+1A0h] [bp-18h]@1
    __int32 v19; // [sp+1A4h] [bp-14h]@1
    const char *v20; // [sp+1A8h] [bp-10h]@1
    time_t timer; // [sp+1ACh] [bp-Ch]@1

    timer = time(0);
    v1 = localtime(&timer);
    src = v1->tm_sec;
    v11 = v1->tm_min;
    v12 = v1->tm_hour;
    v2 = v1->tm_mday;
    v13 = v2;
    v14 = v1->tm_mon;
    v15 = v1->tm_year;
    v16 = v1->tm_wday;
    v17 = v1->tm_yday;
    v18 = v1->tm_isdst;
    v3 = v1->tm_gmtoff;
    v19 = v1->tm_gmtoff;
    v20 = v1->tm_zone;
    if(v2 != *(_DWORD *)(a1 + 12))
    {
        v4 = tm_calc(v2, v3, &src, -3);
        sprintf(
            &arg,
            "rm -rf %s/%04d%02d%02d",
            log_dir,
            *(_DWORD *)(v4 + 20) + 1900,
            *(_DWORD *)(v4 + 16) + 1,
            *(_DWORD *)(v4 + 12));
        system(&arg);
        sys_log(0, "SYSTEM_CMD: %s", &arg);
        *(_DWORD *)(a1 + 12) = v13;
    }
    result = (FILE *)v12;
    if(v12 != *(_DWORD *)(a1 + 8))
    {
        snprintf(&path, 0x80u, "%s/%04d%02d%02d", log_dir, v15 + 1900, v14 + 1, v13);
        if(stat(&path, (struct stat *)&buf) || (unsigned __int16)(v9 & 0xF000) == 16384)
            mkdir(&path, 0x1C0u);
        sys_log(0, "SYSTEM: LOG ROTATE (%04d-%02d-%02d %d)", v15 + 1900, v14 + 1, v13, *(_DWORD *)(a1 + 8));
        fclose(*(FILE **)(a1 + 4));
        snprintf(&arg, 0x80u, "mv %s %s/%s.%02d", *(_DWORD *)a1, &path, *(_DWORD *)a1, *(_DWORD *)(a1 + 8));
        system(&arg);
        *(_DWORD *)(a1 + 8) = v12;
        result = fopen(*(const char **)a1, "a+");
        *(_DWORD *)(a1 + 4) = result;
    }
    return result;
}

FILE *log_rotate()
{
    log_file_check(log_file_sys);
    log_file_check(log_file_err);
    log_file_check(log_file_pt);
    return log_file_rotate(log_file_sys);
}

void log_file_delete_old(const char *dir)
{
    int v1; // edx@1
    int v2; // ecx@1
    int v3; // eax@3
    int v4; // esi@3
    int v5; // ebx@3
    int v6; // [sp+24h] [bp-1E4h]@3
    int v7; // [sp+28h] [bp-1E0h]@11
    char dest; // [sp+38h] [bp-1D0h]@5
    char v9; // [sp+137h] [bp-D1h]@5
    char buf; // [sp+138h] [bp-D0h]@1
    __int16 v11; // [sp+140h] [bp-C8h]@2
    char command; // [sp+198h] [bp-70h]@12
    char nptr; // [sp+1D8h] [bp-30h]@3
    struct dirent **ptr; // [sp+1F8h] [bp-10h]@3

    if(stat(dir, (struct stat *)&buf) == -1)
    {
        perror("log_file_delete_old: stat");
    }
    else
    {
        if((unsigned __int16)(v11 & 0xF000) == 16384)
        {
            v3 = tm_calc(v2, v1, 0, -log_keep_days);
            sprintf(&nptr, "%04d%02d%02d", *(_DWORD *)(v3 + 20) + 1900, *(_DWORD *)(v3 + 16) + 1, *(_DWORD *)(v3 + 12));
            v6 = atoi(&nptr);
            v4 = scandir(dir, &ptr, 0, alphasort);
            v5 = 4 * v4 - 4;
            if(v4 >= 0)
            {
                while(v4)
                {
                    --v4;
                    strncpy(&dest, (const char *)&(*(struct dirent **)((char *)ptr + v5))->d_reclen, 0xFFu);
                    v9 = 0;
                    free(*(struct dirent **)((char *)ptr + v5));
                    if(dest != 46 && !(dest & 0xFFFFFF80) && DefaultRuneLocale__FBSD_1_0[4 * dest + 53] & 4)
                    {
                        v7 = atoi(&dest);
                        if(v6 >= v7)
                        {
                            sprintf(&command, "rm -rf %s/%s", dir, &dest);
                            system(&command);
                            sys_log(0, "%s: SYSTEM_CMD: %s", "log_file_delete_old", &command);
                            fprintf(
                                _stderrp__FBSD_1_0,
                                "%s: SYSTEM_CMD: %s %s:%d %s:%d\n",
                                "log_file_delete_old",
                                &command,
                                &nptr,
                                v6,
                                &dest,
                                v7);
                        }
                    }
                    v5 -= 4;
                }
            }
            else
            {
                perror("scandir");
            }
            free(ptr);
        }
    }
}*/

void log_file_set_dir(const char *src)
{
    strcpy(log_dir, src);
    //log_file_delete_old(log_dir); <- NOW NOT NECESSARY
}

bool log_init(void)
{
#ifdef _MSC_VER
	CHAR szFullPath[MAX_PATH];
#endif

#ifdef _MSC_VER
    log_file_set_dir("./");
#else
	log_file_set_dir("./log");
#endif

#ifdef _MSC_VER
	GetModuleDirectory(MAX_PATH, szFullPath);
	strncat(szFullPath, "\\syslog.txt", MAX_PATH);

    log_file_sys = log_file_init(szFullPath, "a+");
#else
	log_file_sys = log_file_init("syslog", "a+");
#endif

#ifdef _MSC_VER
	GetModuleDirectory(MAX_PATH, szFullPath);
	strncat(szFullPath, "\\syserr.txt", MAX_PATH);

    if(log_file_sys && (log_file_err = log_file_init(szFullPath, "a+")) != 0)
#else
	if(log_file_sys && (log_file_err = log_file_init("syserr", "a+")) != 0)
#endif
    {
#ifdef __LOG_PT
        log_file_pt = log_file_init("PTS", "w");
        return log_file_pt != 0;
#else
		return true;
#endif
    }

    return false;
}

void sys_err(char *a1, int a2, const char *format, ...)
{
    char *result; // eax@1
    int v5; // eax@2
    char s[1026]; // [sp+1Ah] [bp-40Eh]@2
    time_t timer; // [sp+41Ch] [bp-Ch]@1
    va_list va; // [sp+43Ch] [bp+14h]@1

    va_start(va, format);
    timer = time(0);
    result = asctime(localtime(&timer));

    if(log_file_err)
    {
        result[strlen(result) - 1] = 0;

#ifdef _MSC_VER
		v5 = _snprintf(s, 0x400u, "SYSERR: %-15.15s :: %s: ", result + 4, a1);
#else
		v5 = snprintf(s, 0x400u, "SYSERR: %-15.15s :: %s: ", result + 4, a1);
#endif
        
        s[1025] = 0;

        if(v5 <= 1023)
            vsnprintf(&s[v5], 1024 - v5, format, va);

        *(short *)&s[strlen(s)] = 10;

        fputs(s, log_file_err->Unk4);
        fflush(log_file_err->Unk4);

        fputs(s, log_file_sys->Unk4);
        fflush(log_file_sys->Unk4);
    }
}
