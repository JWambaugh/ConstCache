/*
 * Entry.h
 *
 *  Created on: Aug 25, 2011
 *      Author: jordan.wambaugh
 */

#ifndef ENTRY_H
#define ENTRY_H

#include <string>

class mmCacheEntry {
private:
	std::string key;
	int keyHash;
	std::string value;
	int index;
	int hashCode(const char *p, int len){	//hashacharacterarray
			unsigned int h=0;
			for(int i=0; i< len; i++){
				h=(h<<5) | (h>>27);	//5-bitcyclicshift
				h+=(unsigned int)p[i];	//add innextcharacter
			}
			return int(h);
		}

public:

	mmCacheEntry(std::string key, std::string value);
	virtual ~mmCacheEntry();
	void setKey(std::string key){
		this->key=key;
		this->keyHash=hashCode(key.c_str(),key.length());
	}
	std::string getKey(){return key;}

	void setValue(std::string v){
			this->value=v;
	}
	std::string getValue(){return value;}
	int getKeyHash(){return keyHash;}
	int getIndex(){
		return index;
	}

	void setIndex(int maxIndex);



};

#endif /* ENTRY_H */
