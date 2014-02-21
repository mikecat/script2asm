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

#ifndef READ_ONE_LINE_H_GUARD_9D98F54F_993F_45AC_BEE6_08D49DE8A04B
#define READ_ONE_LINE_H_GUARD_9D98F54F_993F_45AC_BEE6_08D49DE8A04B

#include <cstdio>
#include <string>
#include <utility>

typedef std::pair<std::string,std::string> stringPair;

std::string readOneLine(FILE* fp);
std::string stripSpace(const std::string& str);
stringPair divideKeywordAndValue(const std::string& str);

#endif
