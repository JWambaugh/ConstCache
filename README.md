ConstCache
==========

ConstCache is an extremely fast, reliable caching system for PHP. It is highly specialized, and tuned specifically for reading data. In fact, as its name suggests, ConstCache's caches are read-only!


What do you mean read-only?
---------------------------
By read-only, i mean that once you have finalized a ConstCache cache file, it cannot be written to again. And


Why are ConstCache's caches read-only?
--------------------------------------


What use is a Read-Only cache?
------------------------------
A read-only cache can be usefull when you have lots of constant data that doesn't change between software builds. Things like configuration data. Anything you might put in a text file, or constant php array.
Some applications deal with lots of data that usually never changes. 
In these situations, ConistCache can really shine compared to storing the data in plain PHP arrays, databases or even text files.





