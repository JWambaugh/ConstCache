#ifndef PHP_CONSTCACHE_H
/* Prevent double inclusion */
#define PHP_CONSTCACHE_H
/* Define Extension Properties */
#define PHP_CONSTCACHE_EXTNAME    "constcache"
#define PHP_CONSTCACHE_EXTVER    "1.0"
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
extern zend_module_entry constcache_module_entry;
#define phpext_constcache_ptr &constcache_module_entry
#endif /* PHP_CONSTCACHE_H */



