
#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include "c_logger_config.h"

void logger_log(const char *format, ...);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LOGGER_H
