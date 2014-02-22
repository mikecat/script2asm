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

#ifndef IDENTIFIER_INFO_H_GUARD_171D8ADC_0319_40E2_A464_E7661BBF3664
#define IDENTIFIER_INFO_H_GUARD_171D8ADC_0319_40E2_A464_E7661BBF3664

#include <string>
#include <vector>
#include "data_type.h"

enum IdentifierType {
	IDENTIFIER_FUNCTION, // 関数
	IDENTIFIER_GLOBAL_VARIABLE, // グローバル変数
	IDENTIFIER_LOCAL_VARIABLE, // ローカル変数・引数
	IDENTIFIER_INTEGER_LITERAL, // 整数リテラル
	IDENTIFIER_STRING_LITERAL // 文字列リテラル
};

class IdentifierInfo {
	private:
		IdentifierType idType;
		// ローカル変数・引数では%bpからのオフセット
		// 整数リテラルではその値
		int intValue; 
		// グローバル変数・関数では名前
		// 文字列リテラルではその文字列
		std::string strValue;
		// 型
		DataType valueType;
		// 関数の引数の型リスト
		std::vector<DataType> parameterTypes;

		IdentifierInfo(IdentifierType t,int iv): idType(t),intValue(iv),strValue("") {}
		IdentifierInfo(IdentifierType t,const std::string& sv):
			idType(t),intValue(0),strValue(sv) {}
	public:
		IdentifierInfo(): idType(IDENTIFIER_LOCAL_VARIABLE),intValue(0),strValue("") {}

		static IdentifierInfo makeLocalVariable(int offset,const DataType& type);
		static IdentifierInfo makeGlobalVariable(const std::string& name,const DataType& type);
		static IdentifierInfo makeFunction(
			const std::string& name,const DataType& returnType,
			const std::vector<DataType>& parameterTypeList);
		static IdentifierInfo makeIntegerLiteral(int value,const DataType& type);
		static IdentifierInfo makeStringLiteral(const std::string& str,const DataType& type);

		IdentifierType getIdentifierType() const {return idType;}
		int getOffset() const {return intValue;}
		int getValue() const {return intValue;}
		const std::string& getName() const {return strValue;}
		const std::string& getString() const {return strValue;}
		const DataType& getDataType() const {return valueType;}
		const std::vector<DataType>& getParameterTypeList() const {return parameterTypes;}
};

#endif
