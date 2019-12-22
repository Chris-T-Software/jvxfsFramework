#include <switch.h>
#include "broadcast.h"

void jvxfs_console_print_detailed(const char* function, const char* file, uint32_t line, jvxfs_log_level_t level, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    jvxfs_console_vprint_detailed(function, file, line, level, fmt, args);
    va_end(args);
}

void jvxfs_console_vprint_detailed(const char* function, const char* file, uint32_t line, jvxfs_log_level_t level, const char* fmt, va_list args)
{
    switch_log_vprintf(SWITCH_CHANNEL_ID_LOG, file, function, (int)line, NULL, level, fmt, args);
}