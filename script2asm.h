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
		// ���o�͂̃t�@�C���|�C���^
		FILE* outputFile;
		FILE* errorFile;
		// �C���N���[�h�̃l�X�g���x���̍ő�l
		static const int includeLevelMax=10; 
		// �C���N���[�h�̃l�X�g���x��
		int includeLevel;
		// ���A���s�ڂ�
		int lineCounter;
		// �����s�R�����g�̃l�X�g���x�� 
		int commentCounter;
		// ���Ɏg�p���郉�x���̔ԍ�
		int labelCounter;
		// �X�N���v�g�̂ǂ��ɂ��邩(�g�b�v�A�O���[�o���ϐ��̐錾�A�֐��Ȃ�)
		ScriptStatus status;
		// �O���[�o���ϐ��Ɗ֐��̃��X�g 
		std::map<std::string,IdentifierInfo> globalFunctionAndVariableList;
		// ���ϊ����Ă���֐��̏��
		bool needCommitToList; // ���̊֐��̏������X�g�ɒǉ�����ׂ���
		int parameterOffset; // ���̉�������%bp����̃I�t�Z�b�g
		int localVariableOffset; // ���̃��[�J���ϐ���%bp����̃I�t�Z�b�g
		std::string nowFunctionName; // �֐���
		DataType nowFunctionReturnType; // �֐��̖߂�l�̌^
		std::vector<DataType> nowFunctionParameterTypes; // �֐��̈����̌^���X�g
		std::map<std::string,IdentifierInfo> nowFunctionLocalVariableList; // ���[�J���ϐ��̃��X�g
		// �t���[����̊K�w
		std::stack<ControlInfo> controlStack;

		// �G���[�E�x��
		void throwError(const std::string& message);
		void printWarning(const std::string& message);

		// �e�\���ɑ΂��鏈��
		void processComment(const std::string& value);
		void processEndcomment(const std::string& value);
		void processInclude(const std::string& value);
		void processGlobal(const std::string& value);
		void processEndglobal(const std::string& value);
		void processFunction(const std::string& value);
		void processParameters(const std::string& value);
		void processVariables(const std::string& value);
		void processProcedure(const std::string& value);
		void processAssembly(const std::string& value);
		void processEndfunction(const std::string& value);
		void processIf(const std::string& value);
		void processElseif(const std::string& value);
		void processElse(const std::string& value);
		void processEndif(const std::string& value);
		void processWhile(const std::string& value);
		void processWend(const std::string& value);
		void processDo(const std::string& value);
		void processDowhile(const std::string& value);
		void processRepeat(const std::string& value);
		void processLoop(const std::string& value);
		void processContinue(const std::string& value);
		void processBreak(const std::string& value);
		void processReturn(const std::string& value);
		void processPlainExpression(const std::string& now,
			const std::string& keyword,const std::string& value); // �L�[���[�h�ȊO
	public:
		Script2asm(): outputFile(stdout),errorFile(stderr) {}
		Script2asm(FILE* out): outputFile(out),errorFile(stderr) {}
		Script2asm(FILE* out,FILE* err): outputFile(out),errorFile(err) {}
		void initialize();
		void workWithOneFile(const std::string& fileName,int level=0);
		void workWithOneLine(const std::string& rawLine);
		void finish();

		static std::string readOneLine(FILE* fp);
};

#endif
