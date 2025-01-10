
#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
# define LOGGER_PATH_SEPARATOR '\\'
#elif defined(__APPLE__) || defined(__linux__)
# define LOGGER_PATH_SEPARATOR '/'
#endif  // OS

#define IS_VALID_LOGGING_LEVEL(loggingLevel) ((LOGGING_LEVEL_DEBUG <= (loggingLevel)) && ((loggingLevel) < LOGGING_LEVEL_COUNT))

typedef struct Logger_internalState_t_ {
    Logger_StreamConfig_t *streamsArr;
    size_t                 streamsArrLen;
} Logger_internalState_t;

static Logger_internalState_t *g_Logger_internalState;

static const char *g_Logger_logLevelToStrMap[] = {
    [LOGGING_LEVEL_DEBUG]    = "DEBUG",
    [LOGGING_LEVEL_INFO]     = "INFO",
    [LOGGING_LEVEL_WARNING]  = "WARNING",
    [LOGGING_LEVEL_ERROR]    = "ERROR",
    [LOGGING_LEVEL_CRITICAL] = "CRITICAL",
};

#define ARRAY_LEN(array) (sizeof(array) / sizeof(*(array)))
_Static_assert(ARRAY_LEN(g_Logger_logLevelToStrMap) == LOGGING_LEVEL_COUNT,
               "Error: seems like a description string for an error is missing");

status_t Logger_init(void) {
#ifdef C_LOGGER_SAFE_MODE
    if (g_Logger_internalState != NULL) {
        return ERR_LIBRARY_ALREADY_INITIALIZED;
    }
#endif  // C_LOGGER_SAFE_MODE

    g_Logger_internalState = malloc(sizeof(*g_Logger_internalState));
    if (g_Logger_internalState == NULL) {
        return ERR_MEM_ALLOC;
    }

    *g_Logger_internalState = (Logger_internalState_t){
        .streamsArr    = NULL,
        .streamsArrLen = 0,
    };

    return SUCCESS;
}

status_t Logger_deinit(void) {
#ifdef C_LOGGER_SAFE_MODE
    if (g_Logger_internalState == NULL) {
        return ERR_LIBRARY_NOT_INITIALIZED;
    }
#endif  // C_LOGGER_SAFE_MODE

    free(g_Logger_internalState->streamsArr);
    free(g_Logger_internalState);
    g_Logger_internalState = NULL;

    return SUCCESS;
}

status_t Logger_addOutputStream(Logger_StreamConfig_t streamConfig) {
    Logger_StreamConfig_t *streamsArrTemp;
#ifdef C_LOGGER_SAFE_MODE
    if (g_Logger_internalState == NULL) {
        return ERR_LIBRARY_NOT_INITIALIZED;
    }
    if (streamConfig.stream == NULL) {
        return ERR_BAD_ARGUMENT;
    }
    if (!IS_VALID_LOGGING_LEVEL(streamConfig.loggingLevel)) {
        return ERR_BAD_ARGUMENT;
    }
#endif  // C_LOGGER_SAFE_MODE

    streamsArrTemp                     = g_Logger_internalState->streamsArr;
    g_Logger_internalState->streamsArr = realloc(g_Logger_internalState->streamsArr, (g_Logger_internalState->streamsArrLen + 1) * sizeof(*(g_Logger_internalState->streamsArr)));
    if (g_Logger_internalState->streamsArr == NULL) {
        g_Logger_internalState->streamsArr = streamsArrTemp;
        return ERR_MEM_ALLOC;
    }
    (g_Logger_internalState->streamsArr)[g_Logger_internalState->streamsArrLen] = streamConfig;
    ++(g_Logger_internalState->streamsArrLen);

    return SUCCESS;
}

void Logger_log(Logger_LoggingLevel_t loggingLevel, const char *file, int line, const char *func, const char *format, ...) {
    (void)loggingLevel;
    (void)g_Logger_logLevelToStrMap;

    fprintf(stdout, "%20s::%4d::%-20s:", file, line, func);
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}
