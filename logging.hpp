#ifndef LOGGING_INCLUDED
#define LOGGING_INCLUDED

#include <syslog.h>

#define LOGF(priority, msg, ...) syslog(priority, "%s:%d %s "\
    msg, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define LOGF_PANIC(msg, ...)\
    LOGF(LOG_EMERG, msg, ##__VA_ARGS__)

#define LOGF_ALERT(msg, ...)\
    LOGF(LOG_ALERT, msg, ##__VA_ARGS__)

#define LOGF_CRITICAL(msg, ...)\
    LOGF(LOG_CRIT, msg, ##__VA_ARGS__)

#define LOGF_ERROR(msg, ...)\
    LOGF(LOG_ERR, msg, ##__VA_ARGS__)

#define LOGF_WARNING(msg, ...)\
    LOGF(LOG_WARNING, msg, ##__VA_ARGS__)

#define LOGF_NOTICE(msg, ...)\
    LOGF(LOG_NOTICE, msg, ##__VA_ARGS__)

#define LOGF_INFO(msg, ...)\
    LOGF(LOG_INFO, msg, ##__VA_ARGS__)

#define LOGF_DEBUG(msg, ...)\
    LOGF(LOG_DEBUG, msg, ##__VA_ARGS__)
    
#define SET_PANIC_LOG_MASK    setlogmask(LOG_EMERG)
#define SET_ALERT_LOG_MASK    setlogmask(LOG_ALERT)
#define SET_CRITICAL_LOG_MASK setlogmask(LOG_CRIT)
#define SET_ERROR_LOG_MASK    setlogmask(LOG_ERR)
#define SET_WARNING_LOG_MASK  setlogmask(LOG_WARNING)
#define SET_NOTICE_LOG_MASK   setlogmask(LOG_NOTICE)
#define SET_INFO_LOG_MASK     setlogmask(LOG_INFO)
#define SET_DEBUG_LOG_MASK    setlogmask(LOG_DEBUG)

#endif // LOGGING_INCLUDED
