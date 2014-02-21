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
#include "read_one_line.h"

std::string readOneLine(FILE* fp) {
	std::string line="";
	int c;
	for(;;) {
		c=fgetc(fp);
		if(c=='\n' || c==EOF)break;
		line+=(std::string::value_type)c;
	}
	return line;
}

std::string stripSpace(const std::string& str) {
	std::string::const_iterator start,end,now;
	bool startedFlag=false;
	start=str.begin();
	end=str.begin();
	for(now=str.begin();now!=str.end();now++) {
		if(!isspace(*now)) {
			if(!startedFlag) {
				start=now;
				startedFlag=true;
			}
			end=now+1;
		}
	}
	std::string res("");
	res.append(start,end);
	return res;
}

stringPair divideKeywordAndValue(const std::string& str) {
	std::string::const_iterator now;
	std::string fst(""),snd("");
	for(now=str.begin();now!=str.end();now++) {
		if(isspace(*now)) {
			fst.append(str.begin(),now);
			snd.append(now,str.end());
			return stringPair(stripSpace(fst),stripSpace(snd));
		}
	}
	return stringPair(stripSpace(str),std::string(""));
}