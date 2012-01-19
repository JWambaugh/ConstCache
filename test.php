<?php
//dl('constcache.so');
//var_dump(get_loaded_extensions());
$cache=new ConstCache("phpTest.cache",1200);
if($cache->isWritable()){
	$cache->prepareForWrite();
	$cache->set("myKey","My value!!!!");
	$cache->set("myotherKey",array("This is a complex type","Because it is an array."));
	$cache->finalize();
}
$cache2=new ConstCache("phpTest.cache",1200);


$val= $cache->get("myKey");
var_dump($val);
echo "here";

$val= $cache2->get("myKey");
var_dump($val);

echo "not here";
$val ="BLAH";

$val= $cache2->get("myKey");
var_dump($val);




?>
