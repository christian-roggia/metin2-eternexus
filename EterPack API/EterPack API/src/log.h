#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>

#ifdef _MSC_VER
#include <Windows.h>
#include "Utils.h"

#pragma warning(disable : 4996)

#ifndef __cplusplus
typedef int bool;
#define false 0
#define true 1
#endif

#else
#include <stdbool.h>
#endif

typedef struct
{
    char *Unk0;
    FILE *Unk4;
    int Unk8, Unk12;
} SUnkStruct1;

extern char log_dir[256], sys_log_header_string[32]; // SIZE TO CHECK
extern SUnkStruct1 *log_file_sys, *log_file_err, *log_file_pt;
extern unsigned int log_level_bits;
extern int log_keep_days;

#ifdef __cplusplus
extern "C" {
#endif

void log_set_level(int a1);
void log_unset_level(int a1);
void log_set_expiration_days(int a1);
int log_get_expiration_days(void);
void sys_log_header(const char *src);
void log_file_destroy(SUnkStruct1 *ptr);
void log_destroy(void);

/*FILE *log_file_check(int a1);*/

SUnkStruct1 *log_file_init(const char *s, const char *modes);
void pt_log(const char *format, ...);
void sys_log(int a1, const char *format, ...);

/*FILE *log_file_rotate(int a1);
FILE *log_rotate();
void log_file_delete_old(const char *dir);*/

void log_file_set_dir(const char *src);
bool log_init(void);
void sys_err(char *a1, int a2, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // LOG_H
