/* vim:set et sts=4: */
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <ibus.h>

#define IBUS_TYPE_MYANSAN_ENGINE	\
	(ibus_myansan_engine_get_type ())

GType   ibus_myansan_engine_get_type    (void);

#endif
