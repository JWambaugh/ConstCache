<?php
dl('staticache.so');
//var_dump(get_loaded_extensions());
$cache=new StatiCache("phpTest.cache",1200);
//$cache->prepareForWrite();
//$cache->set("myKey","My value!!!!");
//$cache->finalize();
print $cache->get("myKey");
?>
