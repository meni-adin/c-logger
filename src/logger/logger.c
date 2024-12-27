
#include "logger.h"

#include <stdio.h>
#include <stdarg.h>

void logger_log(const char *format, ...) {
    fprintf(stdout, "%20s::%4d: ", __FILE__, __LINE__);
    va_list args;;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);


}
