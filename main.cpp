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
#include "script2asm.h"

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

int main(int argc,char* argv[]) {
	FILE* in=stdin;
	FILE* out=stdout;
	// TODO: コマンドライン引数の解析
	Script2asm s2a(out,stderr);
	try {
		s2a.initialize();
		while(!feof(in)) {
			std::string nowLine=readOneLine(in);
			s2a.workWithOneLine(nowLine);
		}
		s2a.finish();
	} catch(Script2asmError err) {
		fprintf(stderr,"Error at line %d: %s\n",
			err.getLineNumber(),err.getMessage().c_str());
		return 1;
	}
	return 0;
}
