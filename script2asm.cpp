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
	// ���o�͂̃t�@�C���|�C���^
	FILE* inputFile=stdin;
	FILE* outputFile=stdout;
	// ���A���s�ڂ�
	int lineCounter=0;
	// �����s�R�����g�̃l�X�g���x�� 
	int commentCounter=0;
	// ���Ɏg�p���郉�x���̔ԍ�
	int labelCounter=0;
	// �X�N���v�g�̂ǂ��ɂ��邩(�g�b�v�A�O���[�o���ϐ��̐錾�A�֐��Ȃ�)
	ScriptStatus status=STATUS_TOP;
	// �O���[�o���ϐ��Ɗ֐��̃��X�g 
	std::map<std::string,IdentifierInfo> globalFunctionAndVariableList;
	// ���ϊ����Ă���֐��̏��
	bool needCommitToList=false; // ���̊֐��̏������X�g�ɒǉ�����ׂ���
	int parameterOffset=4; // ���̉�������%bp����̃I�t�Z�b�g
	int localVariableOffset=0; // ���̃��[�J���ϐ���%bp����̃I�t�Z�b�g
	std::string nowFunctionName; // �֐���
	DataType nowFunctionReturnType; // �֐��̖߂�l�̌^
	std::vector<DataType> nowFunctionParameterTypes; // �֐��̈����̌^���X�g
	std::map<std::string,IdentifierInfo> nowFunctionLocalVariableList; // ���[�J���ϐ��̃��X�g
	// �t���[����̊K�w
	std::stack<ControlInfo> controlStack;
	try {
		while(!feof(stdin)) {
			lineCounter++;
			std::string rawLine=readOneLine(inputFile); // ���͂��̂܂܂̍s
			std::string now=stripSpace(stripComment(rawLine)); // �O��̋󔒂ƃR�����g������
			if(now=="")continue; // ��s�͓ǂݔ�΂�
			// �s���̃L�[���[�h�܂��͕ϐ����Ƃ��̌�̎��ɕ�����
			stringPair keywordAndValue=divideKeywordAndValue(now);
			const std::string& keyword=keywordAndValue.first;
			const std::string& value=keywordAndValue.second;
			// �܂��A�����s�R�����g�̏������s��
			if(keyword=="comment") {
				commentCounter++;
			} else if(keyword=="endcomment") {
				if(commentCounter==0) {
					throw std::string("stray \"endcomment\"");
				}
				commentCounter--;
			} else if(commentCounter<=0) {
				// �����s�R�����g�̒��ł͂Ȃ����̂݁A�������s��
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
					// �X�e�[�^�X���m�F
					if(status!=STATUS_TOP) {
						throw std::string("stray \"function\"");
					}
					// �֐����Ɩ߂�l�̌^�ɕ�����
					stringPair functionNameAndType=divideKeywordAndValue(value);
					const std::string& functionName=functionNameAndType.first;
					const std::string& functionType=functionNameAndType.second;
					if(globalFunctionAndVariableList.count(functionName)!=0) {
						// ���ɂ��̊֐������݂���: �����錾�̎����Ƃ݂Ȃ�
						if(globalFunctionAndVariableList.at(functionName).
						getIdentifierType()!=IDENTIFIER_FUNCTION) {
							throw functionName+std::string(" is already defined and not a function");
						} else if(functionType!="") {
							fprintf(stderr,
								"Warning at line %d : return type written here is ignored\n",lineCounter);
						}
						needCommitToList=false;
					} else {
						// �V�K�֐�
						if(functionType=="") {
							throw std::string("return type is required");
						}
						needCommitToList=true;
					}
					// �֐��̃p�����[�^�̏�����
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
							// �s��
							throw std::string("Invalid expression");
							break;
						case STATUS_GLOBAL_VARIABLE:
							// �O���[�o���ϐ��̐錾
							if(globalFunctionAndVariableList.count(keyword)!=0) {
								throw keyword+std::string(" is already defined");
							}
							globalFunctionAndVariableList[keyword]=
								IdentifierInfo::makeGlobalVariable(
									keyword,parseType(value)
								);
							break;
						case STATUS_FUNCTION_PARAMETERS: // �������̐錾
						case STATUS_FUNCTION_VARIABLES: // ���[�J���ϐ��̐錾
							{
								// �ϐ���������Ă��Ȃ����`�F�b�N����
								if(nowFunctionLocalVariableList.count(keyword)!=0) {
									throw keyword+std::string(" is already defined");
								}
								int nowOffset=0;
								DataType nowType=parseType(value);
								// �I�t�Z�b�g���v�Z����
								if(status==STATUS_FUNCTION_PARAMETERS) {
									nowOffset=parameterOffset;
									parameterOffset+=nowType.getTypeSize();
									// ���łɉ��������X�g�ɉ�����
									nowFunctionParameterTypes.push_back(nowType);
								} else {
									localVariableOffset-=nowType.getTypeSize();
									nowOffset=localVariableOffset;
								}
								// ���[�J���ϐ��̃��X�g�ɉ�����
								nowFunctionLocalVariableList[keyword]=
									IdentifierInfo::makeLocalVariable(
										nowOffset,nowType
									);
							}
							break;
						case STATUS_FUNCTION_PROCEDURE:
							// ���ۂ̌v�Z����
							// not impremented yet
							break;
						case STATUS_FUNCTION_ASSEMBLY:
							// ���̃A�Z���u���̏o��
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
