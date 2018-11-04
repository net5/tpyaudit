#ifndef __TS_BASE_RECORD_H__
#define __TS_BASE_RECORD_H__

#include "base_env.h"
#include "ts_membuf.h"
#include "protocol_interface.h"

#include <ex.h>

#define MAX_SIZE_PER_FILE 4194304  // 4M = 1024*1024*4

#pragma pack(push,1)

/*
 * ¼��
 *
 * һ��¼���Ϊ�����ļ���һ����Ϣ�ļ���һ�������ļ���
 * �����ڲ��������4M������5�룬�ͽ�����д�������ļ��У���ͬʱ������Ϣ�ļ���
 *
 */


// ¼���ļ�ͷ(����¼������д�룬��ı�Ĳ���)
typedef struct TS_RECORD_HEADER_INFO
{
	ex_u32 magic;		// "TPPR" ��־ TpyAudit Protocol Record
	ex_u16 ver;			// ¼���ļ��汾��ĿǰΪ3
	ex_u32 packages;	// �ܰ���
	ex_u32 time_ms;		// �ܺ�ʱ�����룩
	//ex_u32 file_size;	// �����ļ���С
}TS_RECORD_HEADER_INFO;
#define ts_record_header_info_size sizeof(TS_RECORD_HEADER_INFO)

// ¼���ļ�ͷ(�̶����䲿��)
typedef struct TS_RECORD_HEADER_BASIC
{
	ex_u16 protocol_type;		// Э�飺1=RDP, 2=SSH, 3=Telnet
	ex_u16 protocol_sub_type;	// ��Э�飺100=RDP-DESKTOP, 200=SSH-SHELL, 201=SSH-SFTP, 300=Telnet
	ex_u64 timestamp;	// ����¼�����ʼʱ�䣨UTCʱ�����
	ex_u16 width;		// ��ʼ��Ļ�ߴ磺��
	ex_u16 height;		// ��ʼ��Ļ�ߴ磺��
	char user_username[64];	// teleport�˺�
	char acc_username[64];	// Զ�������û���

	char host_ip[40];	// Զ������IP
	char conn_ip[40];	// Զ������IP
	ex_u16 conn_port;	// Զ�������˿�

	char client_ip[40];		// �ͻ���IP

	// RDPר��
	ex_u8 rdp_security;	// 0 = RDP, 1 = TLS

	ex_u8 _reserve[512 - 2 - 2 - 8 - 2 - 2 - 64 - 64 - 40 - 40 - 2 - 40 - 1 - ts_record_header_info_size];
}TS_RECORD_HEADER_BASIC;
#define ts_record_header_basic_size sizeof(TS_RECORD_HEADER_BASIC)

typedef struct TS_RECORD_HEADER
{
	TS_RECORD_HEADER_INFO info;
	TS_RECORD_HEADER_BASIC basic;
}TS_RECORD_HEADER;

// header���֣�header-info + header-basic�� = 512B
#define ts_record_header_size sizeof(TS_RECORD_HEADER)


// һ�����ݰ���ͷ
typedef struct TS_RECORD_PKG
{
	ex_u8 type;			// ������������
	ex_u32 size;		// ��������ܴ�С��������ͷ��
	ex_u32 time_ms;		// ���������ʼʱ���ʱ�����룬��ζ��һ�����Ӳ��ܳ�������49�죩
	ex_u8 _reserve[3];	// ����
}TS_RECORD_PKG;

#pragma pack(pop)

class TppRecBase
{
public:
	TppRecBase();
	virtual ~TppRecBase();

	bool begin(const wchar_t* base_path, const wchar_t* base_fname, int record_id, const TPP_CONNECT_INFO* info);
	bool end();

protected:
	virtual bool _on_begin(const TPP_CONNECT_INFO* info) = 0;
	virtual bool _on_end() = 0;

protected:
	ex_wstr m_base_path;		// ¼���ļ�����·�������� /usr/local/tpyaudit/data/replay/ssh/123�����ֱ�����ڲ����ӵģ���Ϊ���λỰ¼���ļ���Ŀ¼����
	ex_wstr m_base_fname;		// ¼���ļ����ļ�����������չ�����֣��ڲ����Դ�Ϊ�����ϳ��ļ�ȫ��������¼���ļ������ m_base_path ָ���Ŀ¼��

	ex_u64 m_start_time;

	MemBuffer m_cache;
};

#endif // __TS_BASE_RECORD_H__
