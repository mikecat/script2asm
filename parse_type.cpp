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

#include <cctype>
#include <map>
#include "parse_type.h"

static bool inited=false;
static std::map<std::string,DataType> typeList;

static void initTypeList() {
	if(!inited) {
		typeList["char"]=DataType::createInteger(1,false);
		typeList["byte"]=DataType::createInteger(1,false);
		typeList["schar"]=DataType::createInteger(1,true);
		typeList["short"]=DataType::createInteger(2,true);
		typeList["word"]=DataType::createInteger(2,false);
		inited=true;
	}
}

// 暫定仕様 : 10進数のみ、数字の間に空白を入れても通っちゃう
DataType parseType(const std::string& typeStr) {
	std::string::size_type len=typeStr.size();
	std::vector<unsigned int> accessInfo;
	std::string typeName("");
	bool inArraySize=false;
	unsigned int arrSize=0;
	if(typeStr=="") {
		throw std::string("Type name is empty");
	}
	for(std::string::size_type i=0;i<len;i++) {
		if(typeStr.at(i)=='*') {
			if(inArraySize) {
				throw std::string("Stray '*' foind in type definition!");
			}
			accessInfo.push_back(0);
		} else if(typeStr.at(i)=='[') {
			if(inArraySize) {
				throw std::string("Invalid array definition!");
			}
			inArraySize=true;
			arrSize=0;
		} else if(typeStr.at(i)==']') {
			if(!inArraySize) {
				throw std::string("Invalid array definition!");
			}
			inArraySize=false;
			accessInfo.push_back(arrSize);
			arrSize=0;
		} else if(isdigit(typeStr.at(i))) {
			arrSize=arrSize*10+typeStr.at(i)-'0';
		} else if(!isspace(typeStr.at(i))) {
			std::string::size_type ii=i;
			while(i<len && !isspace(typeStr.at(i)))i++;
			typeName=typeStr.substr(ii,i-ii);
			break;
		}
	}
	initTypeList();
	if(typeList.find(typeName)!=typeList.end()) {
		return typeList.at(typeName);
	} else {
		throw std::string("Invalid type name!");
	}
	return DataType();
}
