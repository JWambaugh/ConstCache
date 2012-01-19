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
------------
ConstCache is a PHP extension. You will need a working PHP development environment to build the extension.

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

