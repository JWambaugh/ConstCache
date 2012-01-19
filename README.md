ConstCache
==========

ConstCache is an extremely fast, persistant caching system for PHP. It is highly specialized, and tuned specifically for reading data. In fact, as its name suggests, ConstCache's caches are read-only!


What do you mean read-only?
---------------------------
By read-only, I mean that once you have finalized a ConstCache cache file, it cannot be written to again.
And you cannot read from a ConstCache file until it has been finalized.


Why are ConstCache's caches read-only?
--------------------------------------
In order to optimize purely for speed of reading, ConstCache's cache files cannot be altered once created.
Creating a cache that allows for random access presents a lot of issue regarding performance and reliability. 
By making the cache read-only, we can skip many of those issues. The cache never has to be resized, locked, or reorganized, and never has fragmentation issues.

Also, by being read only, the format the data is stored in can be further optimized and indexed. Due to this design, lookups occur in constant time.

What use is a Read-Only cache?
------------------------------
A read-only cache can be usefull when you have lots of constant data that doesn't change between software builds. Things like configuration data. Anything you might put in a text file, or constant php array is probably a good candidate.
Some applications deal with lots of data that usually never changes. 
In these situations, ConstCache can really shine compared to storing the data in plain PHP arrays, databases or even text files.


Installation
============
ConstCache is a PHP extension. You will need a working PHP development environment to build the extension. To build it, do the following:

1.	Get ConstCache - 

		git clone git://github.com/martamius/ConstCache.git
2.	Change into the ConstCache directory

		cd ConstCache
3.	Run phpize to setup the environment

		phpize
4.	Configure build system

		./configure
5.	Compile and install

		sudo make install
6.	Then add the following to your php.ini file:

		extension=constcache.so
7.	Then restart your web server if applicable. 



Usage
=====

Once you have the extension built and installed, you can create a new ConstCache object like so:

	<?php
	$cache = new ConstCache('myFile.cache',2000);
	?>
Here's the call signature for the constructor:

	ConstCache(string fileName, int cacheSizeBytes)
The fileName can be any valid path and file. The cacheSizeBytes is the size to which the cache should be initialized in bytes. 
Be sure to make this large enough to fit all of your data. This size cannot be changed, and reserving more than you need is not a problem, so feel free to reserve more than needed. ConstCache will also use some of this space for its index, so be a little generous. Be sure to always specify the same cacheByteSize for all ConstCache objects that point to the same file.

Writing to the cache
--------------------
If a cache file with the specified name does not already exist, ConstCache will initialize a new one. Set it up for writing using the prepareForWrite() method:

	$cache->prepareForWrite();
Then, you can write key-value pairs using the set method:

	$cache->set('myKey','myValue');
Here's the call signature:

	set(string key, mixed value);

Go ahead and set all your data now. You cannot read from the cache until it has been finalized.

Once you are done writing to the cache, finalize it with the finalize() function:

	$cache->finalize();

Cache State
-----------
If you create a ConstCache object pointing to a non-existing file, it will create a new cache file and the object will be in its writable state.
After finalizeing the cache, the cache will enter a readable state. 

If you create a new ConstCache object pointing to an already initialized cache file, the cache object will begin in the readable state.

You can determine the state a cache object is in with the isWritable() and isReadable() methods:

	$cache->isWritable(); //returns true if writable
or

	$cache->isReadable(); //returns true if readable


Reading from Cache
------------------
You can get the value of a key with the get() method:

	$value = $cache->get('myKey');


Here's a full example:

	<?php
	$cache = new ConstCache('myFile.cache',2000);
	
	if($cache->isWritable()){
		$cache->prepareForWrite();
		$cache->set('MyKey',"myValue");
		$cache->finalize();
	}
	
	echo $cache->get('myKey'); // will output 'myValue'
	


