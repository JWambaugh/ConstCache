<?php
dl('constcache.so');
//var_dump(get_loaded_extensions());
$cache=new ConstCache("phpTest.cache",1200);
if($cache->isWritable()){
	$cache->prepareForWrite();
	$cache->set("myKey","My value!!!!");
	$cache->finalize();
}
$key= $cache->get("myKey");
var_dump($key);
?>
