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

#include <cstdio>
#include <string>
#include <stack>
#include <map>
#include "read_one_line.h"
#include "script2asm.h"

int main(void) {
	int ifCounter=0;
	int whileCounter=0;
	int doWhileCounter=0;
	int repeatCounter=0;
	ScriptStatus status=STATUS_TOP;
	std::stack<ControlInfo> controlStack;
	std::map<std::string,int> globalFunctionAndVariableList;
	std::map<std::string,int> localVariableList;
	while(!feof(stdin)) {
		std::string now=stripSpace(stripComment(readOneLine(stdin)));
		if(now=="")continue;
		stringPair keywordAndValue=divideKeywordAndValue(now);
		const std::string& keyword=keywordAndValue.first;
		const std::string& value=keywordAndValue.second;
		if(keyword=="function") {
		} else if(keyword=="parameters") {
		} else if(keyword=="procedure") {
		} else if(keyword=="endfunction") {
		} else {
		}
	}
	return 0;
}
