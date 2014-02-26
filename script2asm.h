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

#ifndef SCRIPT2ASM_H_GUARD_CE14B8E6_114B_4AE3_9636_1B35B0E190BE
#define SCRIPT2ASM_H_GUARD_CE14B8E6_114B_4AE3_9636_1B35B0E190BE

#include <cstdio>
#include <string>
#include <stack>
#include <map>
#include "identifier_info.h"
#include "data_type.h"

class Script2asmError {
	private:
		std::string message;
		int lineno;
	public:
		Script2asmError(): message(""),lineno(0) {}
		Script2asmError(const std::string& mes,int line): message(mes),lineno(line) {}
		const std::string& getMessage() const {return message;}
		int getLineNumber() const {return lineno;}
};

enum ControlType {
	TYPE_IF,
	TYPE_WHILE,
	TYPE_DOWHILE,
	TYPE_REPEAT,
	TYPE_FUNCTION_PARAMETERS,
	TYPE_FUNCTION_VARIABLES,
	TYPE_FUNCTION_PROCEDURE
};

struct ControlInfo {
	ControlType type;
	int count;
};

enum ScriptStatus {
	STATUS_TOP,
	STATUS_GLOBAL_VARIABLE,
	STATUS_FUNCTION_TOP,
	STATUS_FUNCTION_PARAMETERS,
	STATUS_FUNCTION_VARIABLES,
	STATUS_FUNCTION_PROCEDURE,
	STATUS_FUNCTION_ASSEMBLY
};

class Script2asm {
	private:
		// 入出力のファイルポインタ
		FILE* inputFile;
		FILE* outputFile;
		// 今、何行目か
		int lineCounter;
		// 複数行コメントのネストレベル 
		int commentCounter;
		// 次に使用するラベルの番号
		int labelCounter;
		// スクリプトのどこにいるか(トップ、グローバル変数の宣言、関数など)
		ScriptStatus status;
		// グローバル変数と関数のリスト 
		std::map<std::string,IdentifierInfo> globalFunctionAndVariableList;
		// 今変換している関数の情報
		bool needCommitToList; // この関数の情報をリストに追加するべきか
		int parameterOffset; // 次の仮引数の%bpからのオフセット
		int localVariableOffset; // 次のローカル変数の%bpからのオフセット
		std::string nowFunctionName; // 関数名
		DataType nowFunctionReturnType; // 関数の戻り値の型
		std::vector<DataType> nowFunctionParameterTypes; // 関数の引数の型リスト
		std::map<std::string,IdentifierInfo> nowFunctionLocalVariableList; // ローカル変数のリスト
		// フロー制御の階層
		std::stack<ControlInfo> controlStack;

		void throwError(const std::string& message);
	public:
		Script2asm(): inputFile(stdin),outputFile(stdout) {}
		Script2asm(FILE* in,FILE* out): inputFile(in),outputFile(out) {}
		void initialize();
		void workWithOneLine(const std::string& rawLine);
		void finish();
};

#endif
