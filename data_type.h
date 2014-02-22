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

#ifndef DATA_TYPE_H_GUARD_6B00A869_B1F8_46FE_AE4C_9C824A9C347F
#define DATA_TYPE_H_GUARD_6B00A869_B1F8_46FE_AE4C_9C824A9C347F

#include <vector>

class DataType {
	private:
		// �z��̃T�C�Y�B0�Ȃ�|�C���^�ł��邱�Ƃ�����
		std::vector<unsigned int> accessInfo;
		// �|�C���^��z��̌��ƂȂ�^(����)�̃T�C�Y
		unsigned int dataSize;
		// ���̌^�̃T�C�Y
		unsigned int typeSize;
		// �����t�����ǂ���
		bool isSignedFlag;

		static const int ptrSize=2; // �|�C���^�̃T�C�Y
		// ���ƂȂ�^�̃T�C�Y�ƕ��������ǂ������w�肵�č쐬
		DataType(int size,bool signedFlag):
			dataSize(size),typeSize(size),isSignedFlag(signedFlag){}
		// �����̌^�N���X�̔z��܂��̓|�C���^���쐬
		DataType(const DataType& dt,unsigned int acInfo);

	public:
		// �Ƃ肠�����쐬
		DataType():dataSize(0),typeSize(0),isSignedFlag(true){}
		// �R�s�[�R���X�g���N�^
		DataType(const DataType& dt):
			accessInfo(dt.accessInfo),dataSize(dt.dataSize),
			typeSize(dt.typeSize),isSignedFlag(dt.isSignedFlag){}

		// �����^���쐬
		static DataType createInteger(unsigned int size,bool isSigned);
		// �����̃|�C���^���쐬
		static DataType createPointer(unsigned int size,bool isSigned);
		// dt�̃|�C���^���쐬
		static DataType createPointer(const DataType& dt);
		// dt�̔z����쐬
		static DataType createArray(const DataType& dt,unsigned int num);
		// ���������̃|�C���^�ɂ���
		DataType& overWriteToPointer();
		// ���������̔z��ɂ���
		DataType& overWriteToArray(unsigned int num);
		// ����̌^������
		bool operator==(const DataType& dt) const;
		bool operator!=(const DataType& dt) const;
		// ���
		DataType& operator=(const DataType& dt);
		// �^�̏��𓾂�
		unsigned int getTypeSize() const; // �^���̂̃T�C�Y
		unsigned int getDataSize() const; // ���ƂȂ�^�̃T�C�Y
		bool isPointer() const; // �|�C���^���ǂ���
		bool canConsiderAsPointer() const; // �|�C���^�ƌ��Ȃ��邩�ǂ���(�|�C���^or�z�񂩂ǂ���)
		bool isSigned() const; // �����t�����ǂ���
		DataType getParentType() const; // 1�i�K�O�̌^
		unsigned int getParentSize() const; // 1�i�K�O�̌^�̃T�C�Y
};

#endif
