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
	// �ϊ��̎�ނ��擾����
	if(from.canConsiderAsPointer() && to.canConsiderAsPointer()) {
		// �|�C���^���m�̕ϊ��Ȃ̂ŁA�������Ȃ�
		convert=CT_NONE;
	} else if(!from.canConsiderAsPointer() && to.canConsiderAsPointer()) {
		// �������|�C���^
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
		// �|�C���^������
		if(to.getTypeSize()==DataType::ptrSize) {
			convert=CT_NONE;
		} else if(to.getTypeSize()==1) {
			convert=CT_2_TO_1;
		}
	} else {
		// ����������
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
	// �ϊ��R�[�h���o�͂���
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
	// �Ƃ肠����������
	fputs("\txor %ax,%ax\n",out);
	if(pleasePush) {
		fputs("\tpush %ax\n",out);
	}
	return DataType::createInteger(2,true);
}
