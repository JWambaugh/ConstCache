//============================================================================
// Name        : mmCache.cpp
// Author      : Jordan Wambaugh
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================


#include "mmCache.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Entry.h"

#include <cstring>
mmCache::mmCache(std::string fileName, int buffSize){
	//std::cout<< " begin constructor\n";
	bufferSize=buffSize;
	this->fileName=fileName;
	//std::cout<<"receivedFileName:"<<fileName<<"\n";

	//check if file exists

	filePointer= open(fileName.c_str(),O_RDONLY );
	//std::cout<<filePointer;
	if(filePointer==-1){
	//std::cout<<"seting up file\n";
		setupFile();
	}else {
	    map = (char*)mmap(0, bufferSize, PROT_READ, MAP_SHARED, filePointer, 0);
		if (map == MAP_FAILED) {
			close(filePointer);

			throw 0;
		}
		//cache is NOT writable if already existing.
		writable=false;
		readable=true;
		//std::cout << getEntryCount();

	}
	//std::cout<<"end constructor\n";
	setupPointers();
	//std::cout<<"end constructor\n";

}

mmCache::~mmCache(){
	 /* Don't forget to free the mmapped memory
	 */
	if (munmap(map, bufferSize) == -1) {
	perror("Error un-mmapping the file");
	/* Decide here whether to close(fd) and exit() or not. Depends... */
	}

	/* Un-mmaping doesn't close the file, so we still need to do that.
	 */
	close(filePointer);

}

void mmCache::setupPointers(){
	this->statusPointer=map;
	this->entryCountOffsetPointer=map+sizeof(char);
	this->indexOffsetPointer=entryCountOffsetPointer+sizeof(unsigned int);
	this->entriesOffsetPointer=indexOffsetPointer+sizeof(unsigned int);
	this->endOfDataOffsetPointer=entriesOffsetPointer+sizeof(unsigned int);
}

int mmCache::set(std::string key, std::string value){
	if(!writable)throw 2;
	mmCacheEntry *e = new mmCacheEntry(key,value);
	//std::cout<<"adding "<<key<<"->"<<value;
	entries.push_back(e);
	return 1;
}

void mmCache::prepareForWrite(){
	if(!writable) throw 2;
	//std::cout<<"begin prepare";
	writable=true;
	readable=false;
	entries.clear();
}
/**
 * saves the entries and makes cache readable
 */
void mmCache::complete(){
	if(entries.size()<1)throw 3;
	if(!writable)throw 2;
	writable=false;
	readable=true;




	sortEntries();

	//debug entries indexes
	for(unsigned int x=0;x<this->entries.size();x++){
		//std::cout<<"\n"<<entries.at(x)->getIndex() << " hash: " << entries.at(x)->getKeyHash()<<" key: "<<entries.at(x)->getKey();

	}

	//write to memory here
	unsigned int entryCount=0;
	char* entryPos=0;
	unsigned int indexCount=0;
	char *indexPos=0;

	//figure out approx. index size
	unsigned int indexByteSize;
	//index starts 24 bytes after start. This gives us about 12 extra bytes free reserved space in the header.
	indexLoc = map + 24;

	indexByteSize=sizeof(int) + (sizeof(int) * this->entries.size());

	entriesLoc =indexLoc + indexByteSize+8; //entries start 8 bytes after index

	unsigned int currentIndex=0xFFFFFFFF;

	currentIndexPos = indexLoc;
	currentEntriesPos = entriesLoc;
	//std::cout<<"starting loop\n";
	for(unsigned int x=0; x<entries.size();x++){
		mmCacheEntry *e = entries.at(x);
		if(currentIndex != e->getIndex()){
			currentIndex = e->getIndex();
			currentIndexPos=indexLoc+(e->getIndex()*sizeof(unsigned int));
			if(currentIndexPos >= entriesLoc){
				throw "Index overflow!";
			}
			this->intToChar(currentEntriesPos-map,currentIndexPos);
		}

		//std::cout<<"calling writeEntry\n";
		writeEntry(*e);
	}

	//std::cout<<"done writing\n";

	//write our headers
	*statusPointer=1;
	intToChar(entries.size(),entryCountOffsetPointer);
	intToChar(indexLoc - map,indexOffsetPointer);
	intToChar(entriesLoc - map,entriesOffsetPointer);
	intToChar(currentEntriesPos-map,endOfDataOffsetPointer);




	//flush entries to save memory
	entries.clear();
}

void mmCache::sortEntries(){
	//set the key hash on all entries
	for(unsigned int x=0;x<this->entries.size();x++){
		entries.at(x)->setIndex(entries.size());

	}

	this->sortEntryVector(this->entries,0,this->entries.size()-1);
}

//Perform a quicksort on the entries vector
void mmCache::sortEntryVector(std::vector<mmCacheEntry *> &array, int left, int right){
    int pivot,leftIdx=left,rightIdx=right;
    if(right-left>0){
        pivot = left + (right - left)/2; //start in the center
        while(leftIdx<=pivot && rightIdx>=pivot){
            //skip any to the left that are already less than pivot
            while(array.at(leftIdx)->getIndex() < array.at(pivot)->getIndex() && leftIdx <=pivot){
                leftIdx++;
            }
            //skip any to the right that are greater than pivot
            while(array.at(rightIdx)->getIndex() > array.at(pivot)->getIndex() && rightIdx >=pivot){
                rightIdx--;
            }

            //swap left with right
            mmCacheEntry * left = array[leftIdx];
            array[leftIdx] = array[rightIdx];
            array[rightIdx]=left;

            leftIdx++;
            rightIdx--;
            if(leftIdx-1 == pivot){
                pivot=++rightIdx;
            } else if(rightIdx+1==pivot){
                    pivot=--leftIdx;
            }
        }
        this->sortEntryVector(array,left,pivot-1);
        this->sortEntryVector(array,pivot+1,right);
    }
}


void mmCache::setupFile(){
	int result;
	//std::cout<<"optening "<<this->fileName;
	this->filePointer = open(this->fileName.c_str(),O_RDWR | O_CREAT | O_TRUNC );
	if(filePointer == -1){
		throw 1;
	}

	result = lseek(filePointer, bufferSize-1, SEEK_SET);
	if (result == -1) {
		close(filePointer);
		throw 1;
	}
	//puts("here");

	/* Something needs to be written at the end of the file to
	 * have the file actually have the new size.
	 * Just writing an empty string at the current file position will do.
	 *
	 * Note:
	 *  - The current position in the file is at the end of the stretched
	 *    file due to the call to lseek().
	 *  - An empty string is actually a single '\0' character, so a zero-byte
	 *    will be written at the last byte of the file.
	 */
	result = write(filePointer, "", 1);
	if (result != 1) {
		close(filePointer);
		throw 1;
	}

	//puts("here");
	//map the file
	 map =(char*) mmap(0, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, filePointer, 0);
	    if (map == MAP_FAILED) {
		close(filePointer);
		throw 1;
	    }

	//puts("here");




	/*writeEntry("test","myData");
	writeEntry("this is a longer key!!!","Heere's some data");
	writeEntry("Another key with more data...","a lot more data.....sadf saffasdf ff ");
	writeEntry("this is a longer key","myData");
	*/
	writable=true;

}

char* mmCache::findEntry(std::string key){
	int hash = hashCode(key.c_str(), key.length());
	int index = hashIndex(hash,getEntryCount());
	int indexOffset = charToInt(this->indexOffsetPointer);
	int entryOffset = charToInt(map+indexOffset+(index*sizeof(unsigned int)));

	//start at beginning and look for our key

	//std::cout<<"\nEC: "<<getEntryCount()<< " EO:"<<entryOffset<<" io:"<<indexOffset<<" index:"<<index<<" h:"<<hash<<"\n";

	char *x = map+entryOffset;

	char *end = map+charToInt(endOfDataOffsetPointer);
	//std::cout<<end-map;
	//std::cout<<"x-end "<<end-x;
	while(x<end){

		int k =charToInt(x);
		//std::cout <<"Comparing index" <<index << " to " <<k<<"\n";
		if(k == index){
			//if we have a hash index match. Now check hash
			k = charToInt(x+sizeof(int));
			if(k==hash){
				std::string foundKey=getEntryKey(x);
				//std::cout<<"\n found this key:"<<foundKey;
				if(key==foundKey){
					//make sure entry is active
					if(getEntryStatus(x) && 1){
						//entry is active

						//std::cout<<"found active entry!!:";
						return x;
					}
				}
			}
		}else {
			//we left that index bucket, and the key wasn't found.
			//std::cout<<"No key Found. :(";
			return 0;
		}
		x+=getSizeOfEntry(x)+(sizeof(int)*2);
	}
	return 0;
}




int mmCache::getIndexSize(){

	int indexOffset = charToInt(this->indexOffsetPointer);
	int entriesOffset = charToInt(this->entriesOffsetPointer);
	return (entriesOffset-indexOffset-8)/sizeof(unsigned int); //there's a padding of 8 bytes between end of index and beginning of entries
}


int mmCache::getIndexEntryCount(){
	int entriesOffset = charToInt(this->entriesOffsetPointer);
	int indexOffset = charToInt(this->indexOffsetPointer);
	for(int x=indexOffset;x<entriesOffset;x+=sizeof(unsigned int)){

	}
}


std::string mmCache::get(std::string key){
	char * x = findEntry(key);
	if(x!=0){
		return getEntryData(x);
	}
	return "not found\n";
}


int mmCache::get_c(std::string key, char **string, int *length){
	char * x = findEntry(key);
	if(x!=0){
		return getEntryData_c(x,string,length);
	}
	return 0;
}

std::string mmCache::getEntryKey(char *entryLoc){
	//key size is located 13 bytes after beginning
	char *x;
	x= entryLoc+13;
	int keyLen = charToInt(x);
	////std::cout <<"\ngetEntryKey - key length:"<<keyLen;
	x+=sizeof(int);
	std::string str(x,keyLen);
	return str;
}

char mmCache::getEntryStatus(char *entryLoc){
	//status byte is located 12 bytes after start
	return entryLoc[sizeof(int)*3];
}

int mmCache::getSizeOfEntry(char *entryLoc){
	//key size is located 8 bytes after beginning
		char *x;
		x= entryLoc+(sizeof(int)*2);
		int size=charToInt(x);
		//std::cout<<"size of entry:"<<size;
		return size;
}

std::string mmCache::getEntryData(char *entryLoc){
	//key size is located 13 bytes after beginning
	char *x;
	x= entryLoc+13;
	int keyLen = charToInt(x);

	//body size is located 17 + keyLength bytes after beginning

	x= entryLoc+17+keyLen;
	int bodyLen = charToInt(x);
	////std::cout <<"\ngetEntryKey - key length:"<<keyLen;
	x+=sizeof(int);
	std::string str(x,bodyLen);
	return str;
}


int mmCache::getEntryData_c(char *entryLoc, char **string, int *length){
	//key size is located 13 bytes after beginning
	char *x;
	x= entryLoc+13;
	int keyLen = charToInt(x);

	//body size is located 17 + keyLength bytes after beginning

	x= entryLoc+17+keyLen;
	int bodyLen = charToInt(x);
	*length = bodyLen;
	////std::cout <<"\ngetEntryKey - key length:"<<keyLen;
	x+=sizeof(int);
	*string=x;
	return 1;
}




unsigned int mmCache::writeEntry(mmCacheEntry &entry ){
/**
 * Current format:
 * int keyIndex
 * int keyHash
 * int length of entire entry (everything AFTER key hash, including this field)
 * char status of the entry (active, garbage collect, etc.)
 * int length of the key
 * char[keyLen] the key
 * int length of the body
 * char[bodyLen] the body data
 *
 *
 */

	//std::cout<<"***begin writeEntry\n";
	std::string key, data;
	key = entry.getKey();
	data=entry.getValue();
	//puts("\n****");

	char *x = currentEntriesPos;


	//std::cout<<"writing index\n";
	//write key index
	char k[sizeof(int)];
	//puts("after hashcode");
	intToChar(entry.getIndex(),k);
	//std::cout<<"writing index2\n";
	copyStr(x,k,sizeof(int));

	//std::cout<<"writing index3\n";
	x+=sizeof(int);


	//std::cout<<"writing hash\n";
	//write key hash
	int hash;
	hash= hashCode(key.c_str(),key.length());
	//std::cout<<"\nKey hash: "<<hash;
	//puts("after hashcode");
	intToChar(hash,k);
	copyStr(x,k,4);

	x+=sizeof(int);

	//write length of entire entry
	int entrySize=data.length() +key.length()+sizeof(int)+sizeof(int)+sizeof(char)+sizeof(int);
	intToChar(entrySize ,k);
	copyStr(x,k,4);
	x+=sizeof(int);

	//write the entry's current status
	x[0] = MM_ENTRY_ACTIVE;
	x++;

	//write key length
	intToChar(key.length(),k);
	copyStr(x,k,4);
	x+=sizeof(int);


	//write key
	copyStr(x,key.c_str(),key.length());
	////std::cout<<"key length: "<<key.length();
	x+=key.length();

	//write length of data
	intToChar(data.length(),k);
	copyStr(x,k,4);
	x+=sizeof(int);




	//write data
	copyStr(x,data.c_str(),data.length());
	x+=data.length();

	currentPosition=x;



	msync(map,bufferSize, MS_SYNC);

	currentEntriesPos = x;
	//std::cout<<"end writeEntry\n******";
}


int mmCache::hashCode(const char *p, int len){	//hashacharacterarray
	unsigned int h=0;
	for(int i=0; i< len; i++){
		h=(h<<5) | (h>>27);	//5-bitcyclicshift
		h+=(unsigned int)p[i];	//add innextcharacter
	}
	return int(h);
}


void mmCache::intToChar(int n, char * bytes){
	char *buff;
	buff = (char*)&n;
	for(unsigned int x=0;x<sizeof(int);x++){
		bytes[x]=buff[x];
	}
}

int mmCache::charToInt(char *x){
	int *i;
	//read key hash
	i =(int*) x;
	return *i;

}

void mmCache::setEntryCount(int count){
	intToChar(count,entryCountOffsetPointer);
}

int mmCache::getEntryCount(){
	return charToInt(entryCountOffsetPointer);
}

int mmCache::hashIndex(int hash, int max){
	int index;
	index = (hash%max);
	if(index<0)index*=-1;
	return index;
}

void mmCache::copyStr(char *dest,const char *src,int length){
	for(int x=0;x<length;x++){
		dest[x]=src[x];
	}

}

