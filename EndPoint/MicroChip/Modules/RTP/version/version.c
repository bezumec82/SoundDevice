#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include "stdio.h"
#endif
#include "version.h"

static int initialized = 0;
static char versionString[32] = "(no version)";

const char * version_string( void )
{
    if( !initialized )
    {
	#if REVISION+0 > 0
	snprintf( versionString, sizeof(versionString), "version R%04d", REVISION );
	#else
	snprintf( versionString, sizeof(versionString), "snapshot %s", BUILD_DATE );
	#endif
	initialized = 1;
    }
    return versionString;
}

const char * version_copyright( void )
{
    return COPYRIGHT;
}
