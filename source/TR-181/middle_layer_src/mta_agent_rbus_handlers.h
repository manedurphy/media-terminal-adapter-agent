#ifndef _MTA_AGENT_RBUS_HANDLERS_H
#define _MTA_AGENT_RBUS_HANDLERS_H

#include <rbus/rbus.h>

#include "ansc_platform.h"
#include "ccsp_dml_lib.h"
#include "cosa_x_cisco_com_mta_dml.h"

rbusGetHandler_t mta_agent_get_handler(void);
rbusSetHandler_t mta_agent_set_handler(void);

#endif /* _MTA_AGENT_RBUS_HANDLERS_H */
