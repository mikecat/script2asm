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

#include "tree2asm.h"

static void printAxTypeConversionCode(FILE* out,const DataType& from,const DataType& to) {
	enum ConvertType {
		CT_INVALID,
		CT_NONE,
		CT_2_TO_1,
		CT_1S_TO_2,
		CT_1U_TO_2
	} convert=CT_INVALID;
	// 変換の種類を取得する
	if(from.canConsiderAsPointer() && to.canConsiderAsPointer()) {
		// ポインタ同士の変換なので、何もしない
		convert=CT_NONE;
	} else if(!from.canConsiderAsPointer() && to.canConsiderAsPointer()) {
		// 整数→ポインタ
		if(from.getTypeSize()==DataType::ptrSize) {
			convert=CT_NONE;
		} else if(from.getTypeSize()==1) {
			if(from.isSigned()) {
				convert=CT_1S_TO_2;
			} else {
				convert=CT_1U_TO_2;
			}
		}
	} else if(from.canConsiderAsPointer() && !to.canConsiderAsPointer()) {
		// ポインタ→整数
		if(to.getTypeSize()==DataType::ptrSize) {
			convert=CT_NONE;
		} else if(to.getTypeSize()==1) {
			convert=CT_2_TO_1;
		}
	} else {
		// 整数→整数
		if(from.getTypeSize()==to.getTypeSize()) {
			convert=CT_NONE;
		} else if(from.getTypeSize()==1 && to.getTypeSize()==2) {
			if(from.isSigned()) {
				convert=CT_1S_TO_2;
			} else {
				convert=CT_1U_TO_2;
			}
		} else if(from.getTypeSize()==2 && to.getTypeSize()==1) {
			convert=CT_2_TO_1;
		}
	}
	// 変換コードを出力する
	switch(convert) {
		case CT_INVALID:
			throw std::string("invalid type conversion");
		case CT_NONE:
			// no code
			break;
		case CT_2_TO_1:
			// no code
			break;
		case CT_1S_TO_2:
			fputs("\tcbw\n",out);
			break;
		case CT_1U_TO_2:
			fputs("\txor %ah,%ah\n",out);
			break;
	}
}

DataType tree2asm(const Expr2tree::ExprNode* expr,
FILE* out,bool expectVariable,bool pleasePush) {
	DataType returnType;
	switch(expr->opType) {
		case Expr2tree::OP_INVALID:
			throw std::string("invalid operator on tree");
		case Expr2tree::OP_IDENTIFIER:
			break;
		case Expr2tree::OP_CALL_FUNCTION:
			break;
		case Expr2tree::OP_ARRAY:
			break;
		case Expr2tree::OP_CONDITIONAL:
			break;
		case Expr2tree::OP_INC_AFTER:
			break;
		case Expr2tree::OP_INC_BEFORE:
			break;
		case Expr2tree::OP_DEC_AFTER:
			break;
		case Expr2tree::OP_DEC_BEFORE:
			break;
		case Expr2tree::OP_READADDR:
			break;
		case Expr2tree::OP_GETADDR:
			break;
		case Expr2tree::OP_PLUS:
			break;
		case Expr2tree::OP_MINUS:
			break;
		case Expr2tree::OP_BIT_NOT:
			break;
		case Expr2tree::OP_LOGIC_NOT:
			break;
		case Expr2tree::OP_MUL:
			break;
		case Expr2tree::OP_DIV:
			break;
		case Expr2tree::OP_MOD:
			break;
		case Expr2tree::OP_ADD:
			break;
		case Expr2tree::OP_SUB:
			break;
		case Expr2tree::OP_LSHIFT:
			break;
		case Expr2tree::OP_RSHIFT:
			break;
		case Expr2tree::OP_SMALLER:
			break;
		case Expr2tree::OP_BIGGER:
			break;
		case Expr2tree::OP_SMALLER_EQUAL:
			break;
		case Expr2tree::OP_BIGGER_EQUAL:
			break;
		case Expr2tree::OP_EQUAL:
			break;
		case Expr2tree::OP_NOT_EQUAL:
			break;
		case Expr2tree::OP_BIT_AND:
			break;
		case Expr2tree::OP_BIT_XOR:
			break;
		case Expr2tree::OP_BIT_OR:
			break;
		case Expr2tree::OP_LOGIC_AND:
			break;
		case Expr2tree::OP_LOGIC_OR:
			break;
		case Expr2tree::OP_ASSIGN:
			break;
		case Expr2tree::OP_MUL_ASSIGN:
			break;
		case Expr2tree::OP_DIV_ASSIGN:
			break;
		case Expr2tree::OP_MOD_ASSIGN:
			break;
		case Expr2tree::OP_ADD_ASSIGN:
			break;
		case Expr2tree::OP_SUB_ASSIGN:
			break;
		case Expr2tree::OP_LSHIFT_ASSIGN:
			break;
		case Expr2tree::OP_RSHIFT_ASSIGN:
			break;
		case Expr2tree::OP_AND_ASSIGN:
			break;
		case Expr2tree::OP_XOR_ASSIGN:
			break;
		case Expr2tree::OP_OR_ASSIGN:
			break;
		case Expr2tree::OP_CONNECT:
			break;
	}
	if(pleasePush) {
		fputs("\tpush %ax\n",out);
	}
	return returnType;
}
