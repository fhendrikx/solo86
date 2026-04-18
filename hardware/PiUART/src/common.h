#ifndef COMMON_H
#define COMMON_H

#include <circle/multicore.h>
#include <circle/koptions.h>
#include <circle/logger.h>

#define VERSION "PiUART v1.0"

#define NULL 0

#define TELNET_PORT 23
#define RAW_PORT 24
#define ALT_TELNET_PORT 2323
#define ALT_RAW_PORT 2424

enum TTCPMode { telnet, raw };

// the system logger writes ALL messages to an internal log regardless of the loglevel
// this is slow, too slow for timing critcal regions so we skip logging altogether
// based on the loglevel
#ifdef NDEBUG
#define klog(level, ...) ((void) 0)
#else
#define klog(level, ...) if (level <= CKernelOptions::Get()->GetLogLevel()) \
        CLogger::Get()->Write(From, level, __VA_ARGS__)
#endif

#endif
