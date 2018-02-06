#include "commonError.h"
#include <stdarg.h>
#include <stdio.h>
#ifndef _WIN32
#include <syslog.h>
#endif
#include "IInstance.h"
#include "ILogger.h"
#include "st_FILE.h"
