<?php
dl('staticache.so');
//var_dump(get_loaded_extensions());
$cache=new StatiCache("phpTest.cache",1200);
//$cache->prepareForWrite();
//$cache->set("myKey","My value!!!!");
//$cache->finalize();
$key= $cache->get("myKey");
var_dump($key);
?>
