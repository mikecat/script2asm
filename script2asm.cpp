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

#include "parse_type.h"
#include "process_string.h"
#include "script2asm.h"

void Script2asm::throwError(const std::string& message) {
	throw Script2asmError(message,lineCounter);
}

void Script2asm::printWarning(const std::string& message) {
	fprintf(stderr,"Warning at line %d: %s\n",lineCounter,message.c_str());
}

void Script2asm::initialize() {
	// 変数の初期化
	lineCounter=0;
	commentCounter=0;
	labelCounter=0;
	status=STATUS_TOP;
	globalFunctionAndVariableList.clear();
	needCommitToList=false;
	parameterOffset=4;
	localVariableOffset=0;
	nowFunctionName="";
	nowFunctionReturnType=DataType();
	nowFunctionParameterTypes.clear();
	nowFunctionLocalVariableList.clear();
	controlStack=std::stack<ControlInfo>();
	// 標準ライブラリの出力
}

void Script2asm::workWithOneLine(const std::string& rawLine) {
	lineCounter++;
	std::string now=stripSpace(stripComment(rawLine)); // 前後の空白とコメントを消す
	if(now=="")return; // 空行は読み飛ばす
	// 行頭のキーワードまたは変数名とその後の式に分ける
	stringPair keywordAndValue=divideKeywordAndValue(now);
	const std::string& keyword=keywordAndValue.first;
	const std::string& value=keywordAndValue.second;
	// まず、複数行コメントの処理を行う
	if(keyword=="comment") {
		if(value!="") {
			printWarning(std::string("stray \"")+value+"\" ignored.");
		}
		commentCounter++;
	} else if(keyword=="endcomment") {
		if(commentCounter==0) {
			throwError("stray \"endcomment\"");
		}
		if(value!="") {
			printWarning(std::string("stray \"")+value+"\" ignored.");
		}
		commentCounter--;
	} else if(commentCounter<=0) {
		// 複数行コメントの中ではない時のみ、処理を行う
		if(keyword=="global") {
			if(status!=STATUS_TOP) {
				throwError("stray \"global\"");
			}
			if(value!="") {
				printWarning(std::string("stray \"")+value+"\" ignored.");
			}
			status=STATUS_GLOBAL_VARIABLE;
		} else if(keyword=="endglobal") {
			if(status!=STATUS_GLOBAL_VARIABLE) {
				throwError("stray \"endglobal\"");
			}
			if(value!="") {
				printWarning(std::string("stray \"")+value+"\" ignored.");
			}
			status=STATUS_TOP;
		} else if(keyword=="function") {
			// ステータスを確認
			if(status!=STATUS_TOP) {
				throwError("stray \"function\"");
			}
			// 関数名と戻り値の型に分ける
			stringPair functionNameAndType=divideKeywordAndValue(value);
			const std::string& functionName=functionNameAndType.first;
			const std::string& functionType=functionNameAndType.second;
			if(globalFunctionAndVariableList.count(functionName)!=0) {
				// 既にその関数が存在する: 分割宣言の実装とみなす
				if(globalFunctionAndVariableList.at(functionName).
				getIdentifierType()!=IDENTIFIER_FUNCTION) {
					throwError(functionName+" is already defined and not a function");
				} else if(functionType!="") {
					printWarning("return type written here is ignored");
				}
				needCommitToList=false;
			} else {
				// 新規関数
				if(functionType=="") {
					throwError("return type is required");
				}
				needCommitToList=true;
			}
			// 関数のパラメータの初期化
			nowFunctionName=functionName;
			nowFunctionReturnType=parseType(functionType);
			nowFunctionParameterTypes.clear();
			nowFunctionLocalVariableList.clear();
			parameterOffset=4;
			localVariableOffset=0;
			status=STATUS_FUNCTION_TOP;
		} else if(keyword=="parameters") {
		} else if(keyword=="variables") {
		} else if(keyword=="procedure") {
		} else if(keyword=="assembly") {
		} else if(keyword=="endfunction") {
		} else if(keyword=="if") {
		} else if(keyword=="elseif") {
		} else if(keyword=="else") {
		} else if(keyword=="endif") {
		} else if(keyword=="while") {
		} else if(keyword=="wend") {
		} else if(keyword=="do") {
		} else if(keyword=="dowhile") {
		} else if(keyword=="repeat") {
		} else if(keyword=="loop") {
		} else if(keyword=="continue") {
		} else if(keyword=="break") {
		} else if(keyword=="return") {
		} else {
			switch(status) {
				case STATUS_TOP:
				case STATUS_FUNCTION_TOP:
					// 不正
					throwError("Invalid expression");
					break;
				case STATUS_GLOBAL_VARIABLE:
					// グローバル変数の宣言
					if(globalFunctionAndVariableList.count(keyword)!=0) {
						throwError(keyword+" is already defined");
					}
					globalFunctionAndVariableList[keyword]=
						IdentifierInfo::makeGlobalVariable(
							keyword,parseType(value)
						);
					break;
				case STATUS_FUNCTION_PARAMETERS: // 仮引数の宣言
				case STATUS_FUNCTION_VARIABLES: // ローカル変数の宣言
					{
						// 変数名が被っていないかチェックする
						if(nowFunctionLocalVariableList.count(keyword)!=0) {
							throwError(keyword+" is already defined");
						}
						int nowOffset=0;
						DataType nowType=parseType(value);
						// オフセットを計算する
						if(status==STATUS_FUNCTION_PARAMETERS) {
							nowOffset=parameterOffset;
							parameterOffset+=nowType.getTypeSize();
							// ついでに仮引数リストに加える
							nowFunctionParameterTypes.push_back(nowType);
						} else {
							localVariableOffset-=nowType.getTypeSize();
							nowOffset=localVariableOffset;
						}
						// ローカル変数のリストに加える
						nowFunctionLocalVariableList[keyword]=
							IdentifierInfo::makeLocalVariable(
								nowOffset,nowType
							);
					}
					break;
				case STATUS_FUNCTION_PROCEDURE:
					// 実際の計算処理
					// not impremented yet
					break;
				case STATUS_FUNCTION_ASSEMBLY:
					// 生のアセンブリの出力
					fputs((rawLine+"\n").c_str(),outputFile);
					break;
			}
		}
	}
}

void Script2asm::finish() {
	if(commentCounter>0) {
		throwError("comment is unterminated at end of file");
	}
	if(status!=STATUS_TOP) {
		if(status==STATUS_GLOBAL_VARIABLE) {
			throwError("global is unterminated at end of file");
		} else {
			throwError("function is unterminated at end of file");
		}
	}
}
