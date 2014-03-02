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
#include <stack>
#include "expr2tree.h"

namespace Expr2tree {

ExprNode::~ExprNode() {
	ExprList::iterator it;
	for(it=childNodes.begin();it!=childNodes.end();it++) {
		if(*it!=NULL)delete *it;
	}
}

struct OperatorInfo {
	int priority;
	int termNum;
	bool leftToRight;
	bool makesNumber;

	OperatorInfo():
		priority(0),termNum(0),leftToRight(false),makesNumber(false) {}
	OperatorInfo(int pri,int tn,bool l2r,bool mkNum):
		priority(pri),termNum(tn),leftToRight(l2r),makesNumber(mkNum) {}

	// ââéZéqÇÃóDêÊìxÇî‰ärÇ∑ÇÈ
	// opÇêÊÇ…èàóùÇ∑ÇÈÇ◊Ç´Ç©
	bool operator<(const OperatorInfo& op) const {
		return priority<op.priority ||
			(priority==op.priority && !leftToRight);
	}
	// Ç±ÇÃââéZéqÇêÊÇ…èàóùÇ∑ÇÈÇ◊Ç´Ç©
	bool operator>(const OperatorInfo& op) const {
		return priority>op.priority ||
			(priority==op.priority && leftToRight);
	}
};

struct OperatorWithContext {
	std::string str;
	bool prevShouldBeNumber;

	OperatorWithContext():str(""),prevShouldBeNumber(false) {}
	OperatorWithContext(const std::string& s,bool psbn):
		str(s),prevShouldBeNumber(psbn) {}

	bool operator<(const OperatorWithContext& opwc) const {
		return str<opwc.str ||
			(str==opwc.str && !prevShouldBeNumber && opwc.prevShouldBeNumber);
	}
	bool operator<=(const OperatorWithContext& opwc) const {
		return str<opwc.str ||
			(str==opwc.str && !prevShouldBeNumber);
	}
	bool operator>(const OperatorWithContext& opwc) const {
		return !(*this<=opwc);
	}
	bool operator>=(const OperatorWithContext& opwc) const {
		return !(*this<opwc);
	}
	bool operator==(const OperatorWithContext& opwc) const {
		return str==opwc.str && prevShouldBeNumber==opwc.prevShouldBeNumber;
	}
	bool operator!=(const OperatorWithContext& opwc) const {
		return !(*this==opwc);
	}
};

static bool inited=false;
static std::map<OperatorWithContext,OperatorType> opList;
static std::map<OperatorType,OperatorInfo> opInfoList;

static void registerOperator(
const std::string& op,bool prevShouldBeNumber,OperatorType opType) {
	opList[OperatorWithContext(op,prevShouldBeNumber)]=opType;
}

static void registerOperatorInfo(
OperatorType op,int priority,int termNum,bool leftToRight,bool makesNumber) {
	opInfoList[op]=OperatorInfo(priority,termNum,leftToRight,makesNumber);
}

static void initOperatorInfo() {
	if(inited)return;
	inited=true;
	// ââéZéqàÍóóÇstd::mapÇ…ìoò^Ç∑ÇÈ
	opList.clear();
	registerOperator("++",true,OP_INC_AFTER);
	registerOperator("++",false,OP_INC_BEFORE);
	registerOperator("--",true,OP_DEC_AFTER);
	registerOperator("--",false,OP_DEC_BEFORE);
	registerOperator("*",false,OP_READADDR);
	registerOperator("&",false,OP_GETADDR);
	registerOperator("+",false,OP_PLUS);
	registerOperator("-",false,OP_MINUS);
	registerOperator("~",false,OP_BIT_NOT);
	registerOperator("!",false,OP_LOGIC_NOT);
	registerOperator("*",true,OP_MUL);
	registerOperator("/",true,OP_DIV);
	registerOperator("%",true,OP_MOD);
	registerOperator("+",true,OP_ADD);
	registerOperator("-",true,OP_SUB);
	registerOperator("<<",true,OP_LSHIFT);
	registerOperator(">>",true,OP_RSHIFT);
	registerOperator("<",true,OP_SMALLER);
	registerOperator(">",true,OP_BIGGER);
	registerOperator("<=",true,OP_SMALLER_EQUAL);
	registerOperator(">=",true,OP_BIGGER_EQUAL);
	registerOperator("==",true,OP_EQUAL);
	registerOperator("!=",true,OP_NOT_EQUAL);
	registerOperator("&",true,OP_BIT_AND);
	registerOperator("^",true,OP_BIT_XOR);
	registerOperator("|",true,OP_BIT_OR);
	registerOperator("&&",true,OP_LOGIC_AND);
	registerOperator("||",true,OP_LOGIC_OR);
	registerOperator("=",true,OP_ASSIGN);
	registerOperator("*=",true,OP_MUL_ASSIGN);
	registerOperator("/=",true,OP_DIV_ASSIGN);
	registerOperator("%=",true,OP_MOD_ASSIGN);
	registerOperator("+=",true,OP_ADD_ASSIGN);
	registerOperator("-=",true,OP_SUB_ASSIGN);
	registerOperator("<<=",true,OP_LSHIFT_ASSIGN);
	registerOperator(">>=",true,OP_RSHIFT_ASSIGN);
	registerOperator("&=",true,OP_AND_ASSIGN);
	registerOperator("^=",true,OP_XOR_ASSIGN);
	registerOperator("|=",true,OP_OR_ASSIGN);
	registerOperator(",",true,OP_CONNECT);
	// ââéZéqÇÃèÓïÒÇstd::mapÇ…ìoò^Ç∑ÇÈ
	opInfoList.clear();
//	registerOperatorInfo(OP_ARRAY,			12,	2,true,true);
	registerOperatorInfo(OP_INC_AFTER,		11,	1,true,true);
	registerOperatorInfo(OP_INC_BEFORE,		11,	1,false,false);
	registerOperatorInfo(OP_DEC_AFTER,		11,	1,true,true);
	registerOperatorInfo(OP_DEC_BEFORE,		11,	1,false,false);
	registerOperatorInfo(OP_READADDR,		11,	1,false,false);
	registerOperatorInfo(OP_GETADDR,		11,	1,false,false);
	registerOperatorInfo(OP_PLUS,			11,	1,false,false);
	registerOperatorInfo(OP_MINUS,			11,	1,false,false);
	registerOperatorInfo(OP_BIT_NOT,		11,	1,false,false);
	registerOperatorInfo(OP_LOGIC_NOT,		11,	1,false,false);
	registerOperatorInfo(OP_MUL,			10,	2,true,false);
	registerOperatorInfo(OP_DIV,			10,	2,true,false);
	registerOperatorInfo(OP_MOD,			10,	2,true,false);
	registerOperatorInfo(OP_ADD,			9,	2,true,false);
	registerOperatorInfo(OP_SUB,			9,	2,true,false);
	registerOperatorInfo(OP_LSHIFT,			8,	2,true,false);
	registerOperatorInfo(OP_RSHIFT,			8,	2,true,false);
	registerOperatorInfo(OP_SMALLER,		7,	2,true,false);
	registerOperatorInfo(OP_BIGGER,			7,	2,true,false);
	registerOperatorInfo(OP_SMALLER_EQUAL,	7,	2,true,false);
	registerOperatorInfo(OP_BIGGER_EQUAL,	7,	2,true,false);
	registerOperatorInfo(OP_EQUAL,			6,	2,true,false);
	registerOperatorInfo(OP_NOT_EQUAL,		6,	2,true,false);
	registerOperatorInfo(OP_BIT_AND,		5,	2,true,false);
	registerOperatorInfo(OP_BIT_XOR,		4,	2,true,false);
	registerOperatorInfo(OP_BIT_OR,			3,	2,true,false);
	registerOperatorInfo(OP_LOGIC_AND,		2,	2,true,false);
	registerOperatorInfo(OP_LOGIC_OR,		1,	2,true,false);
	registerOperatorInfo(OP_ASSIGN,			0,	2,false,false);
	registerOperatorInfo(OP_MUL_ASSIGN,		0,	2,false,false);
	registerOperatorInfo(OP_DIV_ASSIGN,		0,	2,false,false);
	registerOperatorInfo(OP_MOD_ASSIGN,		0,	2,false,false);
	registerOperatorInfo(OP_ADD_ASSIGN,		0,	2,false,false);
	registerOperatorInfo(OP_SUB_ASSIGN,		0,	2,false,false);
	registerOperatorInfo(OP_LSHIFT_ASSIGN,	0,	2,false,false);
	registerOperatorInfo(OP_RSHIFT_ASSIGN,	0,	2,false,false);
	registerOperatorInfo(OP_AND_ASSIGN,		0,	2,false,false);
	registerOperatorInfo(OP_XOR_ASSIGN,		0,	2,false,false);
	registerOperatorInfo(OP_OR_ASSIGN,		0,	2,false,false);
	registerOperatorInfo(OP_CONNECT,		-1,	2,true,false);
	registerOperatorInfo(OP_INVALID,		-9,	0,true,true);
}

static bool isValidCharForBase(int base,int c) {
	switch(base) {
		case 16: return isxdigit(c);
		case 10: return isdigit(c);
		case 8: return (isdigit(c) && c!='9' && c!='8');
		case 2: return c=='0' || c=='1';
		default: return false;
	}
}

static int hex2int(int onechar) {
	if(isdigit(onechar))return onechar-'0';
	else if(isxdigit(onechar)) {
		if(islower(onechar))return onechar-'a'+10;
		else if(isupper(onechar))return onechar-'A'+10;
	}
	return 0;
}

static std::string unescapeString(const std::string& str) {
	std::string ret("");
	std::string::size_type len=str.length();
	for(std::string::size_type i=0;i<len;i++) {
		if(str.at(i)=='\\') {
			i++;
			if(i<len) {
				switch(str.at(i)) {
					case 'a': ret+='\x07';break;
					case 'b': ret+='\x08';break;
					case 't': ret+='\x09';break;
					case 'n': ret+='\x0A';break;
					case 'f': ret+='\x0C';break;
					case 'r': ret+='\x0D';break;
					case '\\': ret+='\x5C';break;
					case '\'': ret+='\x2C';break;
					case '\"': ret+='\x22';break;
					case '\?': ret+='\x3F';break;
					case 'x':
						if(i+2<len && isxdigit(str.at(i+1)) && isxdigit(str.at(i+2))) {
							ret+=(std::string::value_type)
								(hex2int(str.at(i+1))*16+hex2int(str.at(i+2)));
							i+=2;
						} else {
							ret+='x';
						}
						break;
					default: ret+=str.at(i);break;
				}
			}
		} else {
			ret+=str.at(i);
		}
	}
	return ret;
}

ExprList expr2tree(
ErrorType& error,const std::string& expr,const IdentifierMap& identifiers,
bool doConnect) {
	ExprList exprStack;
	std::stack<OperatorType> operatorStack;
	initOperatorInfo();
	try {
		bool prevIsNumber=false;
		int length=expr.size();
		for(int i=0;i<=length;) {
			int nowExpr=(i<length?expr[i]:-1);
			if(isdigit(nowExpr)) {
				// êîéö
				if(prevIsNumber)throw ERROR_UNEXPECTED_NUMBER;
				int now=0;
				int base=10;
				bool signedFlag=true;
				if(nowExpr=='0' && i<length-1) {
					i++;
					if(expr[i]=='x') {
						base=16;i++;
					} else if(expr[i]=='b') {
						base=2;i++;
					} else {
						base=8;
					}
				}
				for(;i<length && isValidCharForBase(base,expr[i]);i++) {
					if(isdigit(expr[i])) {
						now=now*base+expr[i]-'0';
					} else if(isupper(expr[i])) {
						now=now*base+expr[i]-'A'+10;
					} else if(islower(expr[i])) {
						now=now*base+expr[i]-'a'+10;
					} else {
						break;
					}
				}
				if(i<length && (expr[i]=='u' || expr[i]=='U')) {
					signedFlag=false;
					i++;
				}
				exprStack.push_back(new ExprNode(OP_IDENTIFIER,
					IdentifierInfo::makeIntegerLiteral(
						now,DataType::createInteger(2,signedFlag)
					)
				));
				prevIsNumber=true;
			} else if(isalpha(nowExpr) || nowExpr=='_') {
				// éØï éq(ïœêîÇ‹ÇΩÇÕä÷êî)
				if(prevIsNumber)throw ERROR_UNEXPECTED_NUMBER;
				std::string nowIdentifier;
				int ii=i;
				for(;i<length && (isalnum(expr[i]) || expr[i]=='_');i++);
				nowIdentifier=expr.substr(ii,i-ii);
				if(identifiers.find(nowIdentifier)!=identifiers.end()) {
					exprStack.push_back(
						new ExprNode(OP_IDENTIFIER,identifiers.at(nowIdentifier))
					);
					prevIsNumber=true;
				} else {
					throw ERROR_UNKNOWN_IDENTIFIER;
				}
			} else if(nowExpr=='\'') {
				// ï∂éö
				int ii=i;
				for(i++;i<length && expr[i]!='\'';i++) {
					if(expr[i]=='\\')i++;
				}
				i++;
				if(i>length)throw ERROR_UNTERMINATED_STRING;
				exprStack.push_back(new ExprNode(OP_IDENTIFIER,
					IdentifierInfo::makeIntegerLiteral(
						unescapeString(expr.substr(ii+1,i-ii-2)).at(0),
						DataType::createInteger(2,true)
					)
				));
				prevIsNumber=true;
			} else if(nowExpr=='\"') {
				// ï∂éöóÒ
				int ii=i;
				for(i++;i<length && expr[i]!='\"';i++) {
					if(expr[i]=='\\')i++;
				}
				i++;
				if(i>length)throw ERROR_UNTERMINATED_STRING;
				exprStack.push_back(new ExprNode(OP_IDENTIFIER,
					IdentifierInfo::makeStringLiteral(
						unescapeString(expr.substr(ii+1,i-ii-2)),
						DataType::createPointer(1,false)
					)
				));
				prevIsNumber=true;
			} else if(!isspace(nowExpr)) {
				// ââéZéq
				if(nowExpr=='(' || nowExpr=='[' || nowExpr=='{') {
					// ÉJÉbÉRóﬁÇÇ‹Ç∆ÇﬂÇƒèàóùÇ∑ÇÈ
					int ii=i;
					if(nowExpr=='[' && !prevIsNumber) {
						throw ERROR_UNEXPECTED_OPERATOR;
					} else if(nowExpr=='{' && prevIsNumber) {
						throw ERROR_UNEXPECTED_NUMBER;
					}
					std::stack<char> bracketStack;
					bracketStack.push(nowExpr);
					for(i++;i<length && !bracketStack.empty();i++) {
						if(expr[i]=='(' || expr[i]=='[' || expr[i]=='{') {
							bracketStack.push(expr[i]);
						} else if(expr[i]==')') {
							if(bracketStack.top()!='(') {
								throw ERROR_BRACKET_MISMATCH;
							}
							bracketStack.pop();
						} else if(expr[i]==']') {
							if(bracketStack.top()!='[') {
								throw ERROR_BRACKET_MISMATCH;
							}
							bracketStack.pop();
						} else if(expr[i]=='}') {
							if(bracketStack.top()!='{') {
								throw ERROR_BRACKET_MISMATCH;
							}
							bracketStack.pop();
						}
					}
					if(!bracketStack.empty())throw ERROR_BRACKET_MISMATCH;
					ExprList innerExpr;
					ErrorType nowError;
					innerExpr=expr2tree(nowError,expr.substr(ii+1,i-ii-2),
						identifiers,nowExpr!='{' && !(nowExpr=='(' && prevIsNumber));
					if(nowError!=SUCCESS)throw nowError;
					if(nowExpr=='(') {
						if(prevIsNumber) {
							// ä÷êîåƒÇ—èoÇµ
							if(exprStack.empty()) {
								throw ERROR_MISSING_NUMBER;
							}
							ExprNode* func=exprStack[exprStack.size()-1];
							exprStack.pop_back();
							ExprNode* funcNode=new ExprNode(OP_CALL_FUNCTION);
							funcNode->childNodes.push_back(func);
							funcNode->childNodes.insert(
								funcNode->childNodes.end(),
								innerExpr.begin(),innerExpr.end());
							exprStack.push_back(funcNode);
						} else {
							// ïÅí ÇÃÉJÉbÉR
							if(innerExpr.empty()) {
								throw ERROR_MISSING_NUMBER;
							}
							exprStack.push_back(innerExpr[0]);
						}
					} else if(nowExpr=='[') {
						// îzóÒÉAÉNÉZÉX
						if(innerExpr.empty() || exprStack.empty()) {
							throw ERROR_MISSING_NUMBER;
						}
						ExprNode* newNode=new ExprNode(OP_ARRAY);
						newNode->childNodes.push_back(exprStack[exprStack.size()-1]);
						newNode->childNodes.push_back(innerExpr[0]);
						exprStack.pop_back();
						exprStack.push_back(newNode);
					} else {
						// èåèââéZéq
						if(innerExpr.size()!=3) {
							throw ERROR_INVALID_CONDITIONAL;
						}
						ExprNode* newNode=new ExprNode(OP_CONDITIONAL);
						newNode->childNodes.insert(
							newNode->childNodes.end(),
							innerExpr.begin(),innerExpr.end());
						exprStack.push_back(newNode);
					}
					prevIsNumber=true;
				} else if(nowExpr==')' || nowExpr==']') {
					throw ERROR_BRACKET_MISMATCH;
				} else {
					// ÇªÇÃëºÇÃââéZéq
					OperatorType nowOperator=OP_INVALID;
					if(nowExpr!=-1) {
						for(int j=3;j>0;j--) {
							if((unsigned int)(i+j)>expr.size())continue;
							std::string nowCandidate=expr.substr(i,j);
							OperatorWithContext target(nowCandidate,prevIsNumber);
							if(opList.find(target)!=opList.end()) {
								nowOperator=opList.at(target);
								i+=j;
								break;
							}
						}
						if(nowOperator==OP_INVALID) {
							throw ERROR_INVALID_OPERATOR;
						}
					} else {
						i++;
					}
					OperatorInfo nowOperatorInfo=opInfoList.at(nowOperator);
					while(!operatorStack.empty()) {
						OperatorType opOnStack=operatorStack.top();
						OperatorInfo opOnStackInfo=opInfoList.at(opOnStack);
						if(opOnStackInfo>nowOperatorInfo) {
							int exprSize=exprStack.size();
							if(exprSize<opOnStackInfo.termNum) {
								throw ERROR_UNEXPECTED_OPERATOR;
							}
							if(doConnect || opOnStack!=OP_CONNECT) {
								ExprNode* newNode=new ExprNode(opOnStack);
								for(int j=exprSize-opOnStackInfo.termNum;j<exprSize;j++) {
									newNode->childNodes.push_back(exprStack[j]);
								}
								for(int j=0;j<opOnStackInfo.termNum;j++) {
									exprStack.pop_back();
								}
								exprStack.push_back(newNode);
							}
							operatorStack.pop();
						} else {
							break;
						}
					}
					if(nowOperator!=OP_INVALID)operatorStack.push(nowOperator);
					prevIsNumber=nowOperatorInfo.makesNumber;
				}
			} else {
				i++;
			}
		}
	} catch(ErrorType e) {
		ExprList::iterator it;
		for(it=exprStack.begin();it!=exprStack.end();it++) {
			delete *it;
		}
		error=e;
		return ExprList();
	}
	error=SUCCESS;
	return exprStack;
}

}
