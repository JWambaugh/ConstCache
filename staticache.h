#ifndef PHP_STATICACHE_H
/* Prevent double inclusion */
#define PHP_STATICACHE_H
/* Define Extension Properties */
#define PHP_STATICACHE_EXTNAME    "staticache"
#define PHP_STATICACHE_EXTVER    "1.0"
/* Import configure options
when building outside of
the PHP source tree */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
/* Include PHP Standard Header */
#include "php.h"
}

/* Define the entry point symbol
* Zend will use when loading this module
*/
extern zend_module_entry staticache_module_entry;
#define phpext_staticache_ptr &staticache_module_entry
#endif /* PHP_STATICACHE_H */



