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
		// 配列のサイズ。0ならポインタであることを示す
		std::vector<unsigned int> accessInfo;
		// ポインタや配列の元となる型(整数)のサイズ
		unsigned int dataSize;
		// この型のサイズ
		unsigned int typeSize;
		// 符号付きかどうか
		bool isSignedFlag;

		static const int ptrSize=2; // ポインタのサイズ
		// 元となる型のサイズと符号つきかどうかを指定して作成
		DataType(int size,bool signedFlag):
			dataSize(size),typeSize(size),isSignedFlag(signedFlag){}
		// 既存の型クラスの配列またはポインタを作成
		DataType(const DataType& dt,unsigned int acInfo);

	public:
		// とりあえず作成
		DataType():dataSize(0),typeSize(0),isSignedFlag(true){}
		// コピーコンストラクタ
		DataType(const DataType& dt):
			accessInfo(dt.accessInfo),dataSize(dt.dataSize),
			typeSize(dt.typeSize),isSignedFlag(dt.isSignedFlag){}

		// 整数型を作成
		static DataType createInteger(unsigned int size,bool isSigned);
		// 整数のポインタを作成
		static DataType createPointer(unsigned int size,bool isSigned);
		// dtのポインタを作成
		static DataType createPointer(const DataType& dt);
		// dtの配列を作成
		static DataType createArray(const DataType& dt,unsigned int num);
		// 自分をそのポインタにする
		DataType& overWriteToPointer();
		// 自分をその配列にする
		DataType& overWriteToArray(unsigned int num);
		// 同一の型か判定
		bool operator==(const DataType& dt) const;
		bool operator!=(const DataType& dt) const;
		// 代入
		DataType& operator=(const DataType& dt);
		// 型の情報を得る
		unsigned int getTypeSize() const; // 型自体のサイズ
		unsigned int getDataSize() const; // 元となる型のサイズ
		bool isPointer() const; // ポインタかどうか
		bool canConsiderAsPointer() const; // ポインタと見なせるかどうか(ポインタor配列かどうか)
		bool isSigned() const; // 符号付きかどうか
		DataType getParentType() const; // 1段階前の型
		unsigned int getParentSize() const; // 1段階前の型のサイズ
};

#endif
