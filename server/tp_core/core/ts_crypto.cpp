#include "ts_crypto.h"

#include <mbedtls/aes.h>
#include <mbedtls/base64.h>

/* ���ܷ�����
�㷨��AES-CBC
���̣�����������ǰ�˲���16�ֽ��������Ȼ���������ݲ��뵽16�ֽ���������Ȼ�����AES-CBC���ܡ�
*/

static ex_u8 g_db_field_aes_key[32] = {
	0xd6, 0xb6, 0x6e, 0x3b, 0x41, 0xc4, 0x33, 0x13, 0xaa, 0x61, 0xc9, 0x47, 0x82, 0xfc, 0x84, 0x50,
	0x85, 0x53, 0x3a, 0x01, 0x97, 0x2d, 0xca, 0xba, 0x87, 0xbc, 0x27, 0x20, 0x29, 0xde, 0x87, 0x67,
};

bool ts_db_field_encrypt(const ex_astr& str_dec, ex_astr& str_enc)
{
	ex_bin bin_dec;
	bin_dec.resize(str_dec.length());
	memset(&bin_dec[0], 0, bin_dec.size());
	memcpy(&bin_dec[0], str_dec.c_str(), bin_dec.size());

	return ts_db_field_encrypt(bin_dec, str_enc);
}

bool ts_db_field_decrypt(const ex_astr& str_enc, ex_astr& str_dec)
{
	ex_bin bin_dec;
	if (!ts_db_field_decrypt(str_enc, bin_dec))
		return false;
	if (bin_dec[bin_dec.size() - 1] != 0)
	{
		bin_dec.resize(bin_dec.size() + 1);
		bin_dec[bin_dec.size() - 1] = 0;
	}

	str_dec = (char*)&bin_dec[0];
	return true;
}


bool ts_db_field_encrypt(const ex_bin& bin_dec, ex_astr& str_enc)
{
	int i = 0;
	int offset = 0;

	// ��������ӷ�������ע����̵߳����⣩
	ex_u64 _tick = ex_get_tick_count();
	ex_u64 _seed_tmp = ex_get_thread_id() + _tick;
	ex_u32 _seed = ((ex_u32*)&_seed_tmp)[0] + ((ex_u32*)&_seed_tmp)[1];
	srand(_seed);

	// �������Ĵ�С
	int pad = 16 - bin_dec.size() % 16;
	int enc_size = bin_dec.size() + pad + 16;	// ׷��16�ֽ���Ϊ�˶������������

	// ׼�����������ݣ�16�ֽ������+����+������䣩
	ex_bin bin_be_enc;
	bin_be_enc.resize(enc_size);
	memset(&bin_be_enc[0], 0, bin_be_enc.size());
	offset = 0;
	for (i = 0; i < 16; ++i)
	{
		bin_be_enc[offset] = (unsigned char)(rand() % 0xFF);
		offset++;
	}
	memcpy(&bin_be_enc[offset], &bin_dec[0], bin_dec.size());
	offset += bin_dec.size();
	for (i = 0; i < pad; ++i)
	{
		bin_be_enc[offset] = (unsigned char)pad;
		offset++;
	}

	// ׼�����Ļ�����
	ex_bin bin_enc;
	bin_enc.resize(enc_size);
	memset(&bin_enc[0], 0, bin_enc.size());

	// ׼�������㷨
	mbedtls_aes_context ctx;
	mbedtls_aes_init(&ctx);
	if (0 != mbedtls_aes_setkey_enc(&ctx, g_db_field_aes_key, 256))
	{
		EXLOGE("[core] invalid AES key.\n");
		return false;
	}

	// ����
	unsigned char iv[16] = { 0 };
	memset(iv, 0, 16);
	if (0 != mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, enc_size, iv, &bin_be_enc[0], &bin_enc[0]))
	{
		EXLOGE("[core] AES-CBC encrypt failed.\n");
		mbedtls_aes_free(&ctx);
		return false;
	}
	mbedtls_aes_free(&ctx);

	// �����ܽ������base64����
	ex_bin enc_b64;
	enc_b64.resize(enc_size * 2);
	memset(&enc_b64[0], 0, enc_size * 2);
	size_t olen = 0;
	if (0 != mbedtls_base64_encode(&enc_b64[0], enc_size * 2, &olen, &bin_enc[0], enc_size))
	{
		EXLOGE("[core] BASE64 encode failed.\n");
		return false;
	}
	enc_b64[olen] = 0;
	str_enc = (char*)&enc_b64[0];

	return true;
}

bool ts_db_field_decrypt(const ex_astr& str_enc, ex_bin& bin_dec)
{
	ex_bin bin_enc;
	bin_enc.resize(str_enc.length());
	memset(&bin_enc[0], 0, bin_enc.size());

	// base64����
	size_t enc_size = 0;
	if (0 != mbedtls_base64_decode(&bin_enc[0], bin_enc.size(), &enc_size, (const unsigned char*)str_enc.c_str(), str_enc.length()))
	{
		EXLOGE("[core] BASE64 decode failed.\n");
		return false;
	}
	bin_enc.resize(enc_size);
	if (bin_enc.size() % 16 != 0)
	{
		EXLOGE("[core] invalid cipher-data.\n");
		return false;
	}

	// ׼�����Ļ�����
	ex_bin bin_tmp;
	bin_tmp.resize(enc_size);
	memset(&bin_tmp[0], 0, bin_tmp.size());

	// ׼�������㷨
	mbedtls_aes_context ctx;
	mbedtls_aes_init(&ctx);
	if (0 != mbedtls_aes_setkey_dec(&ctx, g_db_field_aes_key, 256))
	{
		EXLOGE("[core] invalid AES key.\n");
		return false;
	}

	// ����
	unsigned char iv[16] = { 0 };
	memset(iv, 0, 16);
	if (0 != mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, enc_size, iv, &bin_enc[0], &bin_tmp[0]))
	{
		EXLOGE("[core] AES-CBC decrypt failed.\n");
		mbedtls_aes_free(&ctx);
		return false;
	}
	mbedtls_aes_free(&ctx);

	// ȥ��padding
	unsigned char pad = bin_tmp[bin_tmp.size() - 1];
	if (pad == 0 || pad > 16)
	{
		EXLOGE("[core] invalid padding.\n");
		return false;
	}
	bin_tmp.resize(bin_tmp.size() - pad);
	if (bin_tmp.size() < 16)
	{
		EXLOGE("[core] invalid decrypted data.\n");
		return false;
	}

	// �����ս�����Ƶ����ػ���������Ҫ����ǰ���16�ֽ��������
	bin_dec.resize(bin_tmp.size() - 16);
	memcpy(&bin_dec[0], &bin_tmp[16], bin_dec.size());

	return true;
}
