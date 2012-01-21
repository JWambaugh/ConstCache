/*
 * mmCache.h
 *
 *  Created on: Aug 19, 2011
 *      Author: jordan.wambaugh
 */


#ifndef MMCACHE_H_
#define MMCACHE_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string>
#include <vector>

class mmCacheEntry;

#define MM_ENTRY_ACTIVE 1;


typedef char charint[sizeof(int)];
class mmCache{

private:
	int filePointer;
	char * currentPosition;
	char *map;
	unsigned int bufferSize;
	std::vector <mmCacheEntry*> entries;
	bool writable, readable;

 	std::string fileName;
	int hashCode(const char *p, int len);
	void intToChar(int, char*);
	void copyStr(char *dest,const char *src,int length);
	int charToInt(char *x);
	std::string getEntryKey(char *entryLoc);
	std::string getEntryData(char *entryLoc);
	char getEntryStatus(char *entryLoc);
	int getSizeOfEntry(char *entryLoc);
	void setEntryCount(int);
	int getEntryData_c(char *entryLoc, char **string, int *length);
	unsigned int writeEntry(mmCacheEntry &entry );
	char * findEntry(std::string);

	char *indexLoc;			//points to beginning of index in memory map
	char *currentIndexPos;
	char *entriesLoc;		//points to beginning of entries in memory map
	char *currentEntriesPos;

	char *indexOffsetPointer; 		//location in memory map where we store where the index starts
	char *entriesOffsetPointer;		//locatoin in memory map where we store where the entries start
	char *statusPointer;			//points to status byte in memory map
	char *entryCountOffsetPointer;	//points to bytes where we store entry count
	char *endOfDataOffsetPointer;	//points to bytes containing offset to the end of content
	void sortEntries();
	void sortEntryVector(std::vector<mmCacheEntry *> &array, int left, int right);

	void setupPointers();
public:
	mmCache(std::string fileName, int buffSize);
	~mmCache();
	int getEntryCount();
	void setupFile();
	void complete();
	int set(std::string key,std::string value);
	void prepareForWrite();
	std::string get(std::string key);
	int get_c(std::string key, char **string, int *length);
	static int hashIndex(int hash, int max);
	bool isWritable(){return writable;};
	bool isReadable(){return readable;};
	int getIndexSize();
	int getIndexEntryCount();

};




#endif /* MMCACHE_H_ */
