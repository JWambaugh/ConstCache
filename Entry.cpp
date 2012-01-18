/*
 * Entry.cpp
 *
 *  Created on: Aug 25, 2011
 *      Author: jordan.wambaugh
 */

#include "Entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
mmCacheEntry::mmCacheEntry(std::string key, std::string value) {
	this->setKey(key);
	this->value=value;
	// TODO Auto-generated constructor stub

}

mmCacheEntry::~mmCacheEntry() {
	// TODO Auto-generated destructor stub
}


void mmCacheEntry::setIndex(int maxIndex){
		//compress hash
		this->index = (this->getKeyHash())%maxIndex;
		if(index<0)index*=-1;
		//std::cout << "\n setting index:" << index;
	}
