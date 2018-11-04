#ifndef __TS_MEMBUF_H__
#define __TS_MEMBUF_H__

#include <ex.h>

#define MEMBUF_BLOCK_SIZE 128

class MemBuffer
{
public:
	MemBuffer();
	virtual ~MemBuffer();

	// ����size�ֽڵ����ݵ�������ĩβ�����ܻᵼ�»���������
	void append(const ex_u8* data, size_t size);
	// ����������Ϊָ���ֽ��������ܻ����󻺳�������������С����������֤��Ч���ݲ��ᱻ�ı䣩
	void reserve(size_t size);
	// ��m����Ч���ݸ��ӵ��Լ�����Ч����ĩβ�����ܻ����󻺳�����m���ݲ���
	void concat(const MemBuffer& m);
	// �ӻ�����ͷ���Ƴ�size�ֽڣ���������С���ܲ�������������ʣ�µ���Ч����ǰ�ơ�
	void pop(size_t size);
	// ��ջ���������Ч����Ϊ0�ֽڣ����������䣩
	void empty(void) { m_data_size = 0; }
	bool is_empty(void) { return m_data_size == 0; }

	ex_u8* data(void) { return m_buffer; }
	size_t size(void) { return m_data_size; }
	void size(size_t size) { m_data_size = size; }
	size_t buffer_size(void) { return m_buffer_size; }

private:
	ex_u8* m_buffer;
	size_t m_data_size;
	size_t m_buffer_size;
};

#endif // __TS_MEMBUF_H__
