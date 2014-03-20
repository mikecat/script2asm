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
#include "expr2tree.h"
#include "script2asm.h"
#include "tree2asm.h"

std::string Script2asm::readOneLine(FILE* fp) {
	std::string line="";
	int c;
	for(;;) {
		c=fgetc(fp);
		if(c=='\n' || c==EOF)break;
		line+=(std::string::value_type)c;
	}
	return line;
}

void Script2asm::throwError(const std::string& message) {
	throw Script2asmError(message,nowFileName,lineCounter);
}

void Script2asm::printWarning(const std::string& message) {
	fprintf(errorFile,"Warning at file \"%s\" line %d:\n%s\n",
		nowFileName.c_str(),lineCounter,message.c_str());
}

void Script2asm::initialize() {
	// 変数の初期化
	includeLevel=0;
	nowFileName="";
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

void Script2asm::workWithOneFile(const std::string& fileName,int level) {
	FILE* in;
	if(level>includeLevelMax) {
		throwError("too deep include");
	}
	if(fileName=="-") {
		in=stdin;
	} else {
		in=fopen(fileName.c_str(),"r");
		if(in==NULL) {
			throwError("include file open failed");
		}
	}
	lineCounter=0;
	includeLevel=level;
	nowFileName=fileName;
	std::string nowLine;
	while(!feof(in)) {
		nowLine=readOneLine(in);
		workWithOneLine(nowLine);
	}
	if(in!=stdin)fclose(in);
}

void Script2asm::workWithOneLine(const std::string& rawLine) {
	lineCounter++;
	std::string now=stripSpace(stripComment(rawLine)); // 前後の空白とコメントを消す
	if(now=="")return; // 空行は読み飛ばす
	// 行頭のキーワードまたは変数名とその後の式に分ける
	stringPair keywordAndValue=divideKeywordAndValue(now);
	const std::string& keyword=keywordAndValue.first;
	const std::string& value=keywordAndValue.second;
	if(status==STATUS_FUNCTION_ASSEMBLY) {
		// 生のアセンブリの出力(endfunction以外の一切のキーワードを無視する)
		if(keyword=="endfunction") {
			status=STATUS_TOP;
		} else {
			fputs((rawLine+"\n").c_str(),outputFile);
		}
	} else {
		// まず、複数行コメントの処理を行う
		if(keyword=="comment") {
			processComment(value);
		} else if(keyword=="endcomment") {
			processEndcomment(value);
		} else if(commentCounter<=0) {
			// 複数行コメントの中ではない時のみ、処理を行う
			if(keyword=="include") {
				processInclude(value);
			} else if(keyword=="global") {
				processGlobal(value);
			} else if(keyword=="endglobal") {
				processEndglobal(value);
			} else if(keyword=="function") {
				processFunction(value);
			} else if(keyword=="parameters") {
				processParameters(value);
			} else if(keyword=="variables") {
				processVariables(value);
			} else if(keyword=="procedure") {
				processProcedure(value);
			} else if(keyword=="assembly") {
				processAssembly(value);
			} else if(keyword=="endfunction") {
				processEndfunction(value);
			} else if(keyword=="if") {
				processIf(value);
			} else if(keyword=="elseif") {
				processElseif(value);
			} else if(keyword=="else") {
				processElse(value);
			} else if(keyword=="endif") {
				processEndif(value);
			} else if(keyword=="while") {
				processWhile(value);
			} else if(keyword=="wend") {
				processWend(value);
			} else if(keyword=="do") {
				processDo(value);
			} else if(keyword=="dowhile") {
				processDowhile(value);
			} else if(keyword=="repeat") {
				processRepeat(value);
			} else if(keyword=="loop") {
				processLoop(value);
			} else if(keyword=="continue") {
				processContinue(value);
			} else if(keyword=="break") {
				processBreak(value);
			} else if(keyword=="return") {
				processReturn(value);
			} else {
				processPlainExpression(now,keyword,value);
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

void Script2asm::processComment(const std::string& value) {
	if(value!="") {
		printWarning(std::string("stray \"")+value+"\" ignored");
	}
	commentCounter++;
}

void Script2asm::processEndcomment(const std::string& value) {
	if(commentCounter==0) {
		throwError("stray \"endcomment\"");
	}
	if(value!="") {
		printWarning(std::string("stray \"")+value+"\" ignored");
	}
	commentCounter--;
}

void Script2asm::processInclude(const std::string& value) {
	// ファイルの位置を表す変数を退避
	int includeLevelBackup=includeLevel;
	int lineCounterBackup=lineCounter;
	std::string nowFileNameBackup=nowFileName;
	// includeされるファイルを処理する
	workWithOneFile(value,includeLevel+1);
	// 退避した変数を復帰する
	includeLevel=includeLevelBackup;
	lineCounter=lineCounterBackup;
	nowFileName=nowFileNameBackup;
}

void Script2asm::processGlobal(const std::string& value) {
	if(status!=STATUS_TOP) {
		throwError("stray \"global\"");
	}
	if(value!="") {
		printWarning(std::string("stray \"")+value+"\" ignored");
	}
	status=STATUS_GLOBAL_VARIABLE;
}

void Script2asm::processEndglobal(const std::string& value) {
	if(status!=STATUS_GLOBAL_VARIABLE) {
		throwError("stray \"endglobal\"");
	}
	if(value!="") {
		printWarning(std::string("stray \"")+value+"\" ignored");
	}
	status=STATUS_TOP;
}

void Script2asm::processFunction(const std::string& value) {
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
	try {
		nowFunctionReturnType=parseType(functionType);
	} catch(std::string e) {
		throwError(e);
	}
	nowFunctionParameterTypes.clear();
	nowFunctionLocalVariableList.clear();
	parameterOffset=4;
	localVariableOffset=0;
	status=STATUS_FUNCTION_TOP;
}

void Script2asm::processParameters(const std::string& value) {
}

void Script2asm::processVariables(const std::string& value) {
}

void Script2asm::processProcedure(const std::string& value) {
}

void Script2asm::processAssembly(const std::string& value) {
}

void Script2asm::processEndfunction(const std::string& value) {
}

void Script2asm::processIf(const std::string& value) {
}

void Script2asm::processElseif(const std::string& value) {
}

void Script2asm::processElse(const std::string& value) {
}

void Script2asm::processEndif(const std::string& value) {
}

void Script2asm::processWhile(const std::string& value) {
}

void Script2asm::processWend(const std::string& value) {
}

void Script2asm::processDo(const std::string& value) {
}

void Script2asm::processDowhile(const std::string& value) {
}

void Script2asm::processRepeat(const std::string& value) {
}

void Script2asm::processLoop(const std::string& value) {
}

void Script2asm::processContinue(const std::string& value) {
}

void Script2asm::processBreak(const std::string& value) {
}

void Script2asm::processReturn(const std::string& value) {
}

void Script2asm::processPlainExpression
(const std::string& now,const std::string& keyword,const std::string& value) {
	switch(status) {
		case STATUS_TOP:
		case STATUS_FUNCTION_TOP:
		case STATUS_FUNCTION_ASSEMBLY: // avoid warning
			// 不正
			throwError("Invalid expression");
			break;
		case STATUS_GLOBAL_VARIABLE:
			// グローバル変数の宣言
			{
				if(globalFunctionAndVariableList.count(keyword)!=0) {
					throwError(keyword+" is already defined");
				}
				DataType nowType;
				try {
					nowType=parseType(value);
				} catch(std::string e) {
					throwError(e);
				}
				globalFunctionAndVariableList[keyword]=
					IdentifierInfo::makeGlobalVariable(keyword,nowType);
			}
			break;
		case STATUS_FUNCTION_PARAMETERS: // 仮引数の宣言
		case STATUS_FUNCTION_VARIABLES: // ローカル変数の宣言
			{
				// 変数名が被っていないかチェックする
				if(nowFunctionLocalVariableList.count(keyword)!=0) {
					throwError(keyword+" is already defined");
				}
				int nowOffset=0;
				DataType nowType;
				try {
					nowType=parseType(value);
				} catch(std::string e) {
					throwError(e);
				}
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
			try {
				Expr2tree::ErrorType error;
				Expr2tree::ExprList expr=Expr2tree::expr2tree(error,now,
					globalFunctionAndVariableList,nowFunctionLocalVariableList);
				if(error!=Expr2tree::SUCCESS) {
					throwError(Expr2tree::getErrorMessage(error));
				}
				tree2asm(expr.at(0),outputFile,false,false);
			} catch(std::string e) {
				throwError(e);
			}
			break;
	}
}
