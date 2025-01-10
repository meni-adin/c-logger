
#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include <stdbool.h>
#include <stdio.h>

#include "c_logger_config.h"
#include "errors.h"

typedef enum Logger_LoggingLevel_t {
    LOGGING_LEVEL_DEBUG,     // Detailed information for debugging
    LOGGING_LEVEL_INFO,      // General informational messages
    LOGGING_LEVEL_WARNING,   // Warnings about potential issues
    LOGGING_LEVEL_ERROR,     // Errors that need attention
    LOGGING_LEVEL_CRITICAL,  // Critical issues that cause system failure
    LOGGING_LEVEL_COUNT      // Total number of logging levels (upper bound)
} Logger_LoggingLevel_t;

typedef struct Logger_StreamConfig_t_ {
    FILE                 *stream;
    Logger_LoggingLevel_t loggingLevel;
    bool                  isSupportingColor;
} Logger_StreamConfig_t;

#define LOG_COMMON(logLevel, ...) Logger_log(logLevel, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_DEBUG(...)            LOG_COMMON(LOGGING_LEVEL_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)             LOG_COMMON(LOGGING_LEVEL_INFO, __VA_ARGS__)
#define LOG_WARNING(...)          LOG_COMMON(LOGGING_LEVEL_WARNING, __VA_ARGS__)
#define LOG_ERROR(...)            LOG_COMMON(LOGGING_LEVEL_ERROR, __VA_ARGS__)
#define LOG_CRITICAL(...)         LOG_COMMON(LOGGING_LEVEL_CRITICAL, __VA_ARGS__)

status_t Logger_init(void);

status_t Logger_deinit(void);

status_t Logger_addOutputStream(Logger_StreamConfig_t streamConfig);

void Logger_log(Logger_LoggingLevel_t logLevel, const char *file, int line, const char *func, const char *format, ...);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LOGGER_H
