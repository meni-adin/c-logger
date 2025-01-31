
#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if (defined __APPLE__) || (defined __linux__)
# include <sys/time.h>
#elif defined _WIN32
# include <windows.h>
#endif  // OS

#include "mock_wrapper.h"

#ifdef C_LOGGER_SAFE_MODE
# define IS_VALID_LOGGING_LEVEL(loggingLevel)   ((0 <= (loggingLevel)) && ((loggingLevel) < LOGGING_LEVEL_COUNT))
# define IS_VALID_LOGGING_FORMAT(loggingFormat) ((0 <= (loggingFormat)) && ((loggingFormat) < LOGGING_FORMAT_COUNT))
#endif  // C_LOGGER_SAFE_MODE

typedef struct Logger_InternalState_t_ {
    Logger_StreamConfig_t *streamsArr;
    size_t                 streamsArrLen;
} Logger_InternalState_t;

static Logger_InternalState_t *g_Logger_internalState;

typedef struct Logger_LogToStreamArguments_t_ {
    size_t                streamIndex;
    Logger_LoggingLevel_t loggingLevel;
    const char           *file;
    int                   line;
    const char           *func;
    const char           *format;
    va_list               args;
} Logger_LogToStreamArguments_t;

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

#define ANSI_ESC                     "\033"
#define ANSI_PARAM_BEGIN             "["
#define ANSI_PARAM_END               "m"
#define COLOR_PREFIX                 ANSI_ESC ANSI_PARAM_BEGIN
#define COLOR_SUFFIX                 ANSI_PARAM_END
#define COLOR(color)                 COLOR_PREFIX color COLOR_SUFFIX
#define LOGGER_TERMINAL_COLOR_RESET  COLOR("0")
#define LOGGER_TERMINAL_COLOR_RED    COLOR("31")
#define LOGGER_TERMINAL_COLOR_YELLOW COLOR("33")
#define LOGGER_TERMINAL_COLOR_GREEN  COLOR("32")
#define LOGGER_TERMINAL_COLOR_GRAY   COLOR("90")

typedef enum Logger_LoggingColor_t_ {
    LOGGING_COLOR_RESET,
    LOGGING_COLOR_GRAY,
    LOGGING_COLOR_RED,
    LOGGING_COLOR_YELLOW,
    LOGGING_COLOR_GREEN,
    LOGGING_COLOR_COUNT,
} Logger_LoggingColor_t;

static Logger_LoggingColor_t g_Logger_loggingLevelToColorMap[] = {
    [LOGGING_LEVEL_DEBUG]    = LOGGING_COLOR_GRAY,
    [LOGGING_LEVEL_INFO]     = LOGGING_COLOR_RESET,
    [LOGGING_LEVEL_WARNING]  = LOGGING_COLOR_YELLOW,
    [LOGGING_LEVEL_ERROR]    = LOGGING_COLOR_RED,
    [LOGGING_LEVEL_CRITICAL] = LOGGING_COLOR_RED,
};

const char *g_Logger_colorToTerminalColorMap[] = {
    [LOGGING_COLOR_RESET]  = LOGGER_TERMINAL_COLOR_RESET,
    [LOGGING_COLOR_GRAY]   = LOGGER_TERMINAL_COLOR_GRAY,
    [LOGGING_COLOR_RED]    = LOGGER_TERMINAL_COLOR_RED,
    [LOGGING_COLOR_YELLOW] = LOGGER_TERMINAL_COLOR_YELLOW,
    [LOGGING_COLOR_GREEN]  = LOGGER_TERMINAL_COLOR_GREEN,
};

status_t Logger_init(void) {
#ifdef C_LOGGER_SAFE_MODE
    if (g_Logger_internalState != NULL) {
        return ERR_LIBRARY_ALREADY_INITIALIZED;
    }
#endif  // C_LOGGER_SAFE_MODE

    g_Logger_internalState = MW_malloc(sizeof(*g_Logger_internalState));
    if (g_Logger_internalState == NULL) {
        return ERR_MEM_ALLOC;
    }

    *g_Logger_internalState = (Logger_InternalState_t){
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
    if (!IS_VALID_LOGGING_FORMAT(streamConfig.loggingFormat)) {
        return ERR_BAD_ARGUMENT;
    }
#endif  // C_LOGGER_SAFE_MODE

    streamsArrTemp                     = g_Logger_internalState->streamsArr;
    g_Logger_internalState->streamsArr = MW_realloc(g_Logger_internalState->streamsArr, (g_Logger_internalState->streamsArrLen + 1) * sizeof(*(g_Logger_internalState->streamsArr)));
    if (g_Logger_internalState->streamsArr == NULL) {
        g_Logger_internalState->streamsArr = streamsArrTemp;
        return ERR_MEM_ALLOC;
    }
    (g_Logger_internalState->streamsArr)[g_Logger_internalState->streamsArrLen] = streamConfig;
    ++(g_Logger_internalState->streamsArrLen);

    return SUCCESS;
}

static void Logger_setColor(FILE *stream, Logger_LoggingColor_t color) {
    fprintf(stream, "%s", g_Logger_colorToTerminalColorMap[color]);
}

static void Logger_printTime(FILE *stream) {
#if (defined __APPLE__) || (defined __linux__)
    struct timeval tv;
    struct tm     *tm_info;
    char           buffer[10 + 1 + 8 + 1];  // 10(date: YYYY-MM-DD) + 1(space) + 8(time: HH:MM:SS) + 1(null-terminator)
    int            usecToMsecDenom = 1000;

    (void)gettimeofday(&tv, NULL);
    tm_info = localtime(&tv.tv_sec);

    (void)fprintf(stream, "%s.%03d", buffer, tv.tv_usec / usecToMsecDenom);
#elif defined _WIN32
    SYSTEMTIME st;

    GetLocalTime(&st);

    fprintf(stream, "%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#endif  // OS
}

static void Logger_printDateAndTime(FILE *stream) {
    // Logger_printDate
    Logger_printTime(stream);
}

static void Logger_logToScreen(Logger_LogToStreamArguments_t *logToStreamArguments) {
    FILE *stream = g_Logger_internalState->streamsArr[logToStreamArguments->streamIndex].stream;

    Logger_setColor(stream, g_Logger_loggingLevelToColorMap[logToStreamArguments->loggingLevel]);
    Logger_printDateAndTime(stream);
    (void)fprintf(stream, "%s", g_Logger_logLevelToStrMap[logToStreamArguments->loggingLevel]);
    (void)vfprintf(stream, logToStreamArguments->format, logToStreamArguments->args);  // NOLINT (allow non-literal string as format)
    (void)fprintf(stream, "\n");
    Logger_setColor(stream, LOGGING_COLOR_RESET);
}

static void Logger_logToFile(Logger_LogToStreamArguments_t *logToStreamArguments) {
    (void)logToStreamArguments;
}

typedef void (*Logger_logPrintFunc)(Logger_LogToStreamArguments_t *);
static Logger_logPrintFunc g_Logger_logFormatToFuncMap[] = {
    [LOGGING_FORMAT_SCREEN] = Logger_logToScreen,
    [LOGGING_FORMAT_FILE]   = Logger_logToFile,
};

void Logger_log(Logger_LoggingLevel_t loggingLevel, const char *file, int line, const char *func, const char *format, ...) {
    Logger_LogToStreamArguments_t logToStreamArguments = (Logger_LogToStreamArguments_t){
        .loggingLevel = loggingLevel,
        .file         = file,
        .line         = line,
        .func         = func,
        .format       = format,
    };
    va_list             args;
    Logger_logPrintFunc logPrintFunc;

    for (size_t idx = 0; idx < g_Logger_internalState->streamsArrLen; ++idx) {
        if (loggingLevel < g_Logger_internalState->streamsArr[idx].loggingLevel) {
            continue;
        }
        va_start(args, format);
        logToStreamArguments.streamIndex = idx;
        logToStreamArguments.args        = args;
        logPrintFunc                     = g_Logger_logFormatToFuncMap[g_Logger_internalState->streamsArr[idx].loggingFormat];
        logPrintFunc(&logToStreamArguments);
        va_end(args);
    }
}
