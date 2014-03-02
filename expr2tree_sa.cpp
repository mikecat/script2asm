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
#include "expr2tree.h"

using namespace Expr2tree;

static std::map<OperatorType,std::string> op2str;
static std::map<IdentifierType,std::string> id2str;

void initPrint(void) {
	op2str[OP_INVALID]="invalid";
	op2str[OP_IDENTIFIER]="identifier";
	op2str[OP_CALL_FUNCTION]="call_function";
	op2str[OP_ARRAY]="[]";
	op2str[OP_CONDITIONAL]="{cond,T,F}";
	op2str[OP_INC_AFTER]="++(after)";
	op2str[OP_INC_BEFORE]="++(before)";
	op2str[OP_DEC_AFTER]="++(after)";
	op2str[OP_DEC_BEFORE]="++(before)";
	op2str[OP_READADDR]="* (addr)";
	op2str[OP_GETADDR]="& (addr)";
	op2str[OP_PLUS]="+ (sign)";
	op2str[OP_MINUS]="- (sign)";
	op2str[OP_BIT_NOT]="~";
	op2str[OP_LOGIC_NOT]="!";
	op2str[OP_MUL]="*";
	op2str[OP_DIV]="/";
	op2str[OP_MOD]="%";
	op2str[OP_ADD]="+";
	op2str[OP_SUB]="-";
	op2str[OP_LSHIFT]="<<";
	op2str[OP_RSHIFT]=">>";
	op2str[OP_SMALLER]="<";
	op2str[OP_BIGGER]=">";
	op2str[OP_SMALLER_EQUAL]="<=";
	op2str[OP_BIGGER_EQUAL]=">=";
	op2str[OP_EQUAL]="=";
	op2str[OP_NOT_EQUAL]="!=";
	op2str[OP_BIT_AND]="&";
	op2str[OP_BIT_XOR]="^";
	op2str[OP_BIT_OR]="|";
	op2str[OP_LOGIC_AND]="&&";
	op2str[OP_LOGIC_OR]="||";
	op2str[OP_ASSIGN]="=";
	op2str[OP_MUL_ASSIGN]="*=";
	op2str[OP_DIV_ASSIGN]="/=";
	op2str[OP_MOD_ASSIGN]="%=";
	op2str[OP_ADD_ASSIGN]="+=";
	op2str[OP_SUB_ASSIGN]="-=";
	op2str[OP_LSHIFT_ASSIGN]="<<=";
	op2str[OP_RSHIFT_ASSIGN]=">>=";
	op2str[OP_AND_ASSIGN]="&=";
	op2str[OP_XOR_ASSIGN]="^=";
	op2str[OP_OR_ASSIGN]="|=";
	op2str[OP_CONNECT]=",";

	id2str[IDENTIFIER_FUNCTION]="関数";
	id2str[IDENTIFIER_GLOBAL_VARIABLE]="グローバル変数";
	id2str[IDENTIFIER_LOCAL_VARIABLE]="ローカル変数";
	id2str[IDENTIFIER_INTEGER_LITERAL]="整数リテラル";
	id2str[IDENTIFIER_STRING_LITERAL]="文字列リテラル";
}

void printExprList(const ExprList& exl,int level=0) {
	for(ExprList::const_iterator it=exl.begin();it!=exl.end();it++) {
		const ExprNode& en=**it;
		for(int i=0;i<level;i++)fputs("  ",stdout);
		printf("operator %d (%s)",(int)en.opType,op2str[en.opType].c_str());
		if(en.opType==OP_IDENTIFIER) {
			printf(" %s",id2str[en.idInfo.getIdentifierType()].c_str());
			if(en.idInfo.getIdentifierType()==IDENTIFIER_FUNCTION ||
			en.idInfo.getIdentifierType()==IDENTIFIER_GLOBAL_VARIABLE) {
				printf(" %s",en.idInfo.getName().c_str());
			} else if(en.idInfo.getIdentifierType()==IDENTIFIER_STRING_LITERAL) {
				printf(" \"%s\"",en.idInfo.getName().c_str());
			} else {
				printf(" %d",en.idInfo.getValue());
			}
		}
		putchar('\n');
		printExprList(en.childNodes,level+1);
	}
}

int main(void) {
	char buffer[10000];
	IdentifierMap imap;
	initPrint();
	DataType intType=DataType::createInteger(2,true);
	// グローバル変数
	imap["hoge"]=IdentifierInfo::makeGlobalVariable("hoge",intType);
	imap["fuga"]=IdentifierInfo::makeGlobalVariable("fuga",intType);
	// ローカル変数
	imap["x"]=IdentifierInfo::makeLocalVariable(-2,intType);
	imap["y"]=IdentifierInfo::makeLocalVariable(-4,intType);
	imap["z"]=IdentifierInfo::makeLocalVariable(-6,intType);
	imap["i"]=IdentifierInfo::makeLocalVariable(-8,intType);
	imap["ii"]=IdentifierInfo::makeLocalVariable(-10,intType);
	// 引数
	imap["foo"]=IdentifierInfo::makeLocalVariable(4,intType);
	imap["bar"]=IdentifierInfo::makeLocalVariable(6,intType);
	// 関数
	std::vector<DataType> dataTypeList;
	imap["rand"]=IdentifierInfo::makeFunction("rand",intType,dataTypeList);
	dataTypeList.push_back(intType);
	imap["abs"]=IdentifierInfo::makeFunction("abs",intType,dataTypeList);
	dataTypeList.push_back(intType);
	imap["max"]=IdentifierInfo::makeFunction("max",intType,dataTypeList);
	imap["min"]=IdentifierInfo::makeFunction("min",intType,dataTypeList);

	while(fgets(buffer,sizeof(buffer),stdin)) {
		ExprList exl;
		ErrorType err;
		exl=expr2tree(err,std::string(buffer),imap);
		if(err==SUCCESS) {
			printExprList(exl);
		} else {
			printf("Error %d\n",(int)err);
		}
	}
	return 0;
}
