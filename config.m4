PHP_ARG_ENABLE(staticache,
[Whether to enable the staticache extension],
[  -enable-staticache        Enable "staticache" extension support])

if test $PHP_STATICACHE != "no"; then
    PHP_REQUIRE_CXX()
    PHP_ADD_LIBRARY(stdc++, 1, STATICACHE_SHARED_LIBADD)
    PHP_SUBST(STATICACHE_SHARED_LIBADD)
    PHP_NEW_EXTENSION(staticache, staticache.cpp mmCache.cpp Entry.cpp, $ext_shared)
fi
