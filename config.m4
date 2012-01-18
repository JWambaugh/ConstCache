PHP_ARG_ENABLE(constcache,
[Whether to enable the constcache extension],
[  -enable-constcache        Enable "constcache" extension support])

if test $PHP_CONSTCACHE != "no"; then
    PHP_REQUIRE_CXX()
    PHP_ADD_LIBRARY(stdc++, 1, CONSTCACHE_SHARED_LIBADD)
    PHP_SUBST(CONSTCACHE_SHARED_LIBADD)
    PHP_NEW_EXTENSION(constcache, constcache.cpp mmCache.cpp Entry.cpp, $ext_shared)
fi
