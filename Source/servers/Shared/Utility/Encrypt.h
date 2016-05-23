#ifndef SWA_MD5_H_
#define SWA_MD5_H_
#include "BaseDefine.h"
#include "CommonDefine.h"
#include "NetConfig.h"

#include <string>
#include <fstream>

/* Type define */
using std::string;
using std::ifstream;


/* MD5 declaration. */
class Encrypt {
public:
	Encrypt();
	Encrypt(const void *input, size_t length);
	Encrypt(const string &str);
	Encrypt(ifstream &in);
	void MD5(const void *input, size_t length);
	void MD5(const string &str);
	void MD5(ifstream &in);
	const char* Digest();
	string toString();
	void Reset();

	static void  RandKey(char o_arrKey[MAX_ENCRYPT_LENTH]);
	static int32 XorCode(uint32 nRandNum, const char arrKey[MAX_ENCRYPT_LENTH], char *pSrc, uint32 nSrcLen);

private:
	void MD5(const char *input, size_t length);
	void Final();
	void Transform(const char block[64]);
	void Encode(const uint32 *input, char *output, size_t length);
	void Decode(const char *input, uint32 *output, size_t length);
	string CharsToHexString(const char *input, size_t length);

	/* class uncopyable */
	Encrypt(const Encrypt&);
	Encrypt& operator=(const Encrypt&);

private:
	uint32 _state[4];	/* state (ABCD) */
	uint32 _count[2];	/* number of bits, modulo 2^64 (low-order word first) */
	char _buffer[64];	/* input buffer */
	char _digest[16];	/* message digest */
	bool _finished;		/* calculate finished ? */

	static const char PADDING[64];	/* padding for calculate */
	static const char HEX[16];
	static const size_t BUFFER_SIZE = 1024;
};




#endif/*MD5_H*/

