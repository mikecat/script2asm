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
#include "identifier_info.h"
#include "parse_type.h"
#include "script2asm.h"

int main(void) {
	// 入出力のファイルポインタ
	FILE* inputFile=stdin;
	FILE* outputFile=stdout;
	// 今、何行目か
	int lineCounter=0;
	// 複数行コメントのネストレベル 
	int commentCounter=0;
	// 次に使用するラベルの番号
	int labelCounter=0;
	// スクリプトのどこにいるか(トップ、グローバル変数の宣言、関数など)
	ScriptStatus status=STATUS_TOP;
	// グローバル変数と関数のリスト 
	std::map<std::string,IdentifierInfo> globalFunctionAndVariableList;
	// 今変換している関数の情報
	bool needCommitToList=false; // この関数の情報をリストに追加するべきか
	int parameterOffset=4; // 次の仮引数の%bpからのオフセット
	int localVariableOffset=0; // 次のローカル変数の%bpからのオフセット
	std::string nowFunctionName; // 関数名
	DataType nowFunctionReturnType; // 関数の戻り値の型
	std::vector<DataType> nowFunctionParameterTypes; // 関数の引数の型リスト
	std::map<std::string,IdentifierInfo> nowFunctionLocalVariableList; // ローカル変数のリスト
	// フロー制御の階層
	std::stack<ControlInfo> controlStack;
	try {
		while(!feof(stdin)) {
			lineCounter++;
			std::string rawLine=readOneLine(inputFile); // 入力そのままの行
			std::string now=stripSpace(stripComment(rawLine)); // 前後の空白とコメントを消す
			if(now=="")continue; // 空行は読み飛ばす
			// 行頭のキーワードまたは変数名とその後の式に分ける
			stringPair keywordAndValue=divideKeywordAndValue(now);
			const std::string& keyword=keywordAndValue.first;
			const std::string& value=keywordAndValue.second;
			// まず、複数行コメントの処理を行う
			if(keyword=="comment") {
				commentCounter++;
			} else if(keyword=="endcomment") {
				if(commentCounter==0) {
					throw std::string("stray \"endcomment\"");
				}
				commentCounter--;
			} else if(commentCounter<=0) {
				// 複数行コメントの中ではない時のみ、処理を行う
				if(keyword=="global") {
					if(status!=STATUS_TOP) {
						throw std::string("stray \"global\"");
					}
					status=STATUS_GLOBAL_VARIABLE;
				} else if(keyword=="endglobal") {
					if(status!=STATUS_GLOBAL_VARIABLE) {
						throw std::string("stray \"endglobal\"");
					}
					status=STATUS_TOP;
				} else if(keyword=="function") {
					// ステータスを確認
					if(status!=STATUS_TOP) {
						throw std::string("stray \"function\"");
					}
					// 関数名と戻り値の型に分ける
					stringPair functionNameAndType=divideKeywordAndValue(value);
					const std::string& functionName=functionNameAndType.first;
					const std::string& functionType=functionNameAndType.second;
					if(globalFunctionAndVariableList.count(functionName)!=0) {
						// 既にその関数が存在する: 分割宣言の実装とみなす
						if(globalFunctionAndVariableList.at(functionName).
						getIdentifierType()!=IDENTIFIER_FUNCTION) {
							throw functionName+std::string(" is already defined and not a function");
						} else if(functionType!="") {
							fprintf(stderr,
								"Warning at line %d : return type written here is ignored\n",lineCounter);
						}
						needCommitToList=false;
					} else {
						// 新規関数
						if(functionType=="") {
							throw std::string("return type is required");
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
							throw std::string("Invalid expression");
							break;
						case STATUS_GLOBAL_VARIABLE:
							// グローバル変数の宣言
							if(globalFunctionAndVariableList.count(keyword)!=0) {
								throw keyword+std::string(" is already defined");
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
									throw keyword+std::string(" is already defined");
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
		if(commentCounter>0) {
			throw std::string("comment is unterminated at end of file");
		}
		if(status!=STATUS_TOP) {
			if(status==STATUS_GLOBAL_VARIABLE) {
				throw std::string("global is unterminated at end of file");
			} else {
				throw std::string("function is unterminated at end of file");
			}
		}
	} catch(std::string err) {
		fprintf(stderr,"Error at line %d : %s\n",lineCounter,err.c_str());
		return 1;
	}
	return 0;
}
