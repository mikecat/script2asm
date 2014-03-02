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

#ifndef EXPR2TREE_H_GUARD_97DDB99F_F65C_47FF_A631_604B7B6B6577
#define EXPR2TREE_H_GUARD_97DDB99F_F65C_47FF_A631_604B7B6B6577

#include <vector>
#include <map>
#include <string>
#include "identifier_info.h"

namespace Expr2tree {

enum OperatorType {
	OP_INVALID,
	OP_IDENTIFIER,
	OP_CALL_FUNCTION,
	OP_ARRAY,
	OP_CONDITIONAL,
	OP_INC_AFTER,
	OP_INC_BEFORE,
	OP_DEC_AFTER,
	OP_DEC_BEFORE,
	OP_READADDR,
	OP_GETADDR,
	OP_PLUS,
	OP_MINUS,
	OP_BIT_NOT,
	OP_LOGIC_NOT,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_ADD,
	OP_SUB,
	OP_LSHIFT,
	OP_RSHIFT,
	OP_SMALLER,
	OP_BIGGER,
	OP_SMALLER_EQUAL,
	OP_BIGGER_EQUAL,
	OP_EQUAL,
	OP_NOT_EQUAL,
	OP_BIT_AND,
	OP_BIT_XOR,
	OP_BIT_OR,
	OP_LOGIC_AND,
	OP_LOGIC_OR,
	OP_ASSIGN,
	OP_MUL_ASSIGN,
	OP_DIV_ASSIGN,
	OP_MOD_ASSIGN,
	OP_ADD_ASSIGN,
	OP_SUB_ASSIGN,
	OP_LSHIFT_ASSIGN,
	OP_RSHIFT_ASSIGN,
	OP_AND_ASSIGN,
	OP_XOR_ASSIGN,
	OP_OR_ASSIGN,
	OP_CONNECT
};

enum ErrorType {
	SUCCESS,
	ERROR_UNEXPECTED_NUMBER,
	ERROR_UNEXPECTED_OPERATOR,
	ERROR_MISSING_NUMBER,
	ERROR_BRACKET_MISMATCH,
	ERROR_UNKNOWN_IDENTIFIER,
	ERROR_INVALID_OPERATOR,
	ERROR_UNTERMINATED_STRING,
	ERROR_INVALID_CONDITIONAL
};

struct ExprNode;
typedef std::vector<ExprNode*> ExprList;

struct ExprNode {
	OperatorType opType;
	IdentifierInfo idInfo;
	ExprList childNodes;
	ExprNode(): opType(OP_INVALID) {}
	ExprNode(OperatorType t): opType(t) {}
	ExprNode(OperatorType t,const IdentifierInfo& it): opType(t),idInfo(it) {}
	~ExprNode();
};

typedef std::map<std::string,IdentifierInfo> IdentifierMap;

ExprList expr2tree(
	ErrorType& error,const std::string& expr,const IdentifierMap& identifiers,
	bool doConnect=true);
std::string getErrorMessage(const ErrorType& err);

}

#endif
