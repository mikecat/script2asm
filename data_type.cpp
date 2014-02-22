/*
Copyright 2014 MikeCAT.

This software is provided "as is", without any express or implied warranties,
including but not limited to the implied warranties of merchantability and
fitness for a particular purpose.  In no event will the authors or contributors
be held liable for any direct, indirect, incidental, special, exemplary, or
consequential damages however caused and on any theory of liability, whether in
contract, strict liability, or tort (including negligence or otherwise),
arising in any way out of the use of this software, even if advised of the
possibility of such damage.

Permission is granted to anyone to use this software for any purpose, including
commercial applications, and to alter and distribute it freely in any form,
provided that the following conditions are met:

1. The origin of this software must not be misrepresented; you must not claim
   that you wrote the original software. If you use this software in a product,
   an acknowledgment in the product documentation would be appreciated but is
   not required.

2. Altered source versions may not be misrepresented as being the original
   software, and neither the name of MikeCAT nor the names of
   authors or contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

3. This notice must be included, unaltered, with any source distribution.
*/

#include <climits>
#include <string>
#include "data_type.h"

DataType::DataType(const DataType& dt,unsigned int acInfo):
accessInfo(dt.accessInfo),dataSize(dt.dataSize),isSignedFlag(dt.isSignedFlag) {
	accessInfo.push_back(acInfo);
	if(acInfo==0) {
		// ポインタ
		typeSize=ptrSize;
	} else {
		// オーバーフロー対策
		if(UINT_MAX/dt.typeSize<acInfo) {
			throw std::string("Array size is too big!");
		}
		typeSize=dt.typeSize*acInfo;
	}
}

DataType DataType::createInteger(unsigned int size,bool isSigned) {
	return DataType(size,isSigned);
}

DataType DataType::createPointer(const DataType& dt) {
	return DataType(dt,0);
}

DataType DataType::createArray(const DataType& dt,unsigned int num) {
	return DataType(dt,num);
}

DataType& DataType::overWriteToPointer() {
	accessInfo.push_back(0);
	typeSize=ptrSize;
	return *this;
}

DataType& DataType::overWriteToArray(unsigned int num) {
	// オーバーフロー対策
	if(UINT_MAX/typeSize<num) {
		throw std::string("Array size is too big!");
	}
	accessInfo.push_back(num);
	typeSize*=num;
	return *this;
}

bool DataType::operator==(const DataType& dt) const {
	return accessInfo==dt.accessInfo && isSignedFlag==dt.isSignedFlag &&
		dataSize==dt.dataSize;
}

bool DataType::operator!=(const DataType& dt) const {
	return !(*this==dt);
}

DataType& DataType::operator=(const DataType& dt) {
	accessInfo=dt.accessInfo;
	dataSize=dt.dataSize;
	typeSize=dt.typeSize;
	isSignedFlag=dt.isSignedFlag;
	return *this;
}

unsigned int DataType::getTypeSize() const {
	return typeSize;
}

unsigned int DataType::getDataSize() const {
	return dataSize;
}

bool DataType::isPointer() const {
	return !accessInfo.empty() && accessInfo.at(accessInfo.size()-1)==0;
}

bool DataType::canConsiderAsPointer() const {
	return !accessInfo.empty();
}

bool DataType::isSigned() const {
	return isSignedFlag;
}

DataType DataType::getParentType() const {
	if(accessInfo.empty()) {
		throw std::string("This is neither pointer nor array!");
	}
	DataType ret(*this);
	ret.accessInfo.pop_back();
	ret.typeSize=ret.dataSize;
	for(std::vector<unsigned int>::iterator it=ret.accessInfo.begin();
	it!=ret.accessInfo.end();it++) {
		if(*it==0) {
			ret.typeSize=ptrSize;
		} else {
			ret.typeSize*=(*it);
		}
	}
	return ret;
}

unsigned int DataType::getParentSize() const {
	if(accessInfo.empty()) {
		throw std::string("This is neither pointer nor array!");
	}
	unsigned int ret=dataSize;
	for(std::vector<unsigned int>::const_iterator it=accessInfo.begin();
	(it+1)!=accessInfo.end();it++) {
		if(*it==0) {
			ret=ptrSize;
		} else {
			ret*=(*it);
		}
	}
	return ret;
}
