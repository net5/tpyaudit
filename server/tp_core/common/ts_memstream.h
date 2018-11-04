#ifndef __TS_MEMSTREAM_H__
#define __TS_MEMSTREAM_H__

#include "ts_membuf.h"

class MemStream
{
public:
	MemStream(MemBuffer& mbuf);
	~MemStream();

	void reset(void); // ��ջ��������ݣ������ͷ��ڴ棩��ָ���ƶ���ͷ��

	bool seek(size_t offset);	// �ƶ�ָ�뵽ָ��ƫ�ƣ����Խ�磬�򷵻ش���
	bool rewind(size_t n = 0);	// ����n�ֽڣ����Խ�磬���ش������nΪ0������˵��ʼ��
	bool skip(size_t n);	// ����n�ֽڣ����Խ�磬�򷵻ش���

	ex_u8* ptr(void) { return m_mbuf.data() + m_offset; } // ���ص�ǰ����ָ��
	size_t offset(void) { return m_offset; } // ���ص�ǰָ�����������ʼ��ƫ��

	size_t left(void) { return m_mbuf.size() - m_offset; }	// ����ʣ�����ݵĴ�С���ӵ�ǰ����ָ�뵽��������β��

	ex_u8 get_u8(void);
	ex_u16 get_u16_le(void);
	ex_u16 get_u16_be(void);
	ex_u32 get_u32_le(void);
	ex_u32 get_u32_be(void);
	ex_u8* get_bin(size_t n); // ���ص�ǰָ������ݵ�ָ�룬�ڲ�ƫ�ƻ�����ƶ�n�ֽ�

	void put_zero(size_t n);  // ���n�ֽڵ�0
	void put_u8(ex_u8 v);
	void put_u16_le(ex_u16 v);
	void put_u16_be(ex_u16 v);
	void put_u32_le(ex_u32 v);
	void put_u32_be(ex_u32 v);
	void put_bin(const ex_u8* p, size_t n); // ���pָ���n�ֽ�����

	size_t size(void) { return m_mbuf.size(); }

private:
	MemBuffer& m_mbuf;
	size_t m_offset;
};

#endif // __TS_MEMSTREAM_H__
