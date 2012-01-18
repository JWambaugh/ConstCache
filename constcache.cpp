#include "constcache.h"
#include "mmCache.h"
extern "C" {
#include <ext/standard/php_var.h>
#include "ext/standard/php_smart_str.h"
}
zend_class_entry *constcache_ce;
zend_object_handlers cache_object_handlers;
struct cache_object {
    zend_object std;
    mmCache *cache;
};



void constcache_free_storage(void *object TSRMLS_DC)
{
    cache_object *obj = (cache_object *)object;
    delete obj->cache;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);

    efree(obj);
}

zend_object_value constcache_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    cache_object *obj = (cache_object *)emalloc(sizeof(cache_object));
    memset(obj, 0, sizeof(cache_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(obj->std.properties, &type->default_properties,
        (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));

    retval.handle = zend_objects_store_put(obj, NULL,
        constcache_free_storage, NULL TSRMLS_CC);
    retval.handlers = &cache_object_handlers;

    return retval;
}











PHP_METHOD(ConstCache, __construct){
    char *cacheName;
    int cacheNameLen;
    long buffSize;
	mmCache *cache = NULL;
	zval *object = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &cacheName, &cacheNameLen,&buffSize) == FAILURE) {
		RETURN_NULL();
	}
	//php_printf("\ncacheName: %s\n",cacheName,cacheNameLen);
	std::string cname(cacheName,(size_t)cacheNameLen);
	//php_printf("\ncacheName: %s -----%s\n",cacheName,cacheNameLen,cname.c_str(),cname.length());
	mmCache *c = new mmCache(cname,buffSize);
	cache_object *obj = (cache_object *)zend_object_store_get_object(object TSRMLS_CC);
	obj->cache = c;
}



PHP_METHOD(ConstCache, prepareForWrite){
	zval *me = getThis();
	cache_object *obj = (cache_object *)zend_object_store_get_object(me TSRMLS_CC);
	obj->cache->prepareForWrite();
}

PHP_METHOD(ConstCache, set){
	zval *me = getThis();
	cache_object *obj = (cache_object *)zend_object_store_get_object(me TSRMLS_CC);
	mmCache *c=obj->cache;

	smart_str buff={0};
	php_serialize_data_t var_hash;
	char *key;
	zval *value;
	int keyLen;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &key, &keyLen, &value) == FAILURE) {
			RETURN_NULL();
		}
	PHP_VAR_SERIALIZE_INIT(var_hash);
	php_var_serialize(&buff, &value, &var_hash TSRMLS_CC);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);
	c->set(std::string(key,(size_t)keyLen),std::string(buff.c,(size_t)buff.len));
	smart_str_free(&buff);
}


PHP_METHOD(ConstCache, finalize){
	zval *me = getThis();
	cache_object *obj = (cache_object *)zend_object_store_get_object(me TSRMLS_CC);
	mmCache *c=obj->cache;
	c->complete();
}


PHP_METHOD(ConstCache, get){

	char *key;
	int keyLen;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &keyLen) == FAILURE) {
		RETURN_NULL();
	}
	zval *me = getThis();
	cache_object *obj = (cache_object *)zend_object_store_get_object(me TSRMLS_CC);
	mmCache *c=obj->cache;

	char *ret;
	int retLen;
	c->get_c(std::string(key,(size_t)keyLen),&ret,&retLen);
	//php_printf("%s\n",ret,retLen);
	//zval *val;

	php_unserialize_data_t data;
	PHP_VAR_UNSERIALIZE_INIT(data);
	const char *endPtr = ret + retLen;
	//ALLOC_INIT_ZVAL(val);
	php_var_unserialize(&return_value
			,(const unsigned char **) &ret
			,(const unsigned char *) endPtr
			,&data	TSRMLS_CC
			);
	PHP_VAR_UNSERIALIZE_DESTROY(data);



}


PHP_METHOD(ConstCache, isWritable){
	zval *me = getThis();
	cache_object *obj = (cache_object *)zend_object_store_get_object(me TSRMLS_CC);
	mmCache *c=obj->cache;
	RETURN_BOOL(c->isWritable());
}

PHP_METHOD(ConstCache, isReadable){
	zval *me = getThis();
	cache_object *obj = (cache_object *)zend_object_store_get_object(me TSRMLS_CC);
	mmCache *c=obj->cache;
	RETURN_BOOL(c->isReadable());
}


function_entry constcache_methods[] = {
    PHP_ME(ConstCache,  __construct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(ConstCache,  prepareForWrite,     NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ConstCache,  set,     NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ConstCache,  finalize,     NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ConstCache,  get,     NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ConstCache,  isWritable,     NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ConstCache,  isReadable,     NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(constcache)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "ConstCache", constcache_methods);
    constcache_ce = zend_register_internal_class(&ce TSRMLS_CC);

    constcache_ce->create_object = constcache_create_handler;
    memcpy(&cache_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    cache_object_handlers.clone_obj = NULL;
    return SUCCESS;
}

zend_module_entry constcache_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
		STANDARD_MODULE_HEADER,
#endif
	PHP_CONSTCACHE_EXTNAME,
	NULL, /* Functions */
	PHP_MINIT(constcache), /* MINIT */
	NULL, /* MSHUTDOWN */
	NULL, /* RINIT */
	NULL, /* RSHUTDOWN */
	NULL, /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
	PHP_CONSTCACHE_EXTVER,
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CONSTCACHE
extern "C" {
	ZEND_GET_MODULE(constcache)
}
#endif
