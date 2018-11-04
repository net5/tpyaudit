#ifndef __TELNET_RECORDER_H__
#define __TELNET_RECORDER_H__

#include "../../common/base_record.h"

#define TS_RECORD_TYPE_TELNET_TERM_SIZE		0x01		// �ն˴�С��������������
#define TS_RECORD_TYPE_TELNET_DATA			0x02		// ����չʾ����������

#pragma pack(push,1)

// ��¼���ڴ�С�ı�����ݰ�
typedef struct TS_RECORD_WIN_SIZE
{
	ex_u16 width;
	ex_u16 height;
}TS_RECORD_WIN_SIZE;

#pragma pack(pop)

class TppTelnetRec : public TppRecBase
{
public:
	TppTelnetRec();
	virtual ~TppTelnetRec();

	void record(ex_u8 type, const ex_u8* data, size_t size);
// 	void record_win_size(int width, int height);
	void record_win_size_startup(int width, int height);
	void record_win_size_change(int width, int height);

	void save_record();

protected:
	bool _on_begin(const TPP_CONNECT_INFO* info);
	bool _on_end();

	bool _save_to_info_file();
	bool _save_to_data_file();
	bool _save_to_cmd_file();

protected:
	TS_RECORD_HEADER m_head;
	bool m_header_changed;

	MemBuffer m_cmd_cache;

	bool m_save_full_header;

	FILE* m_file_info;
	FILE* m_file_data;
	FILE* m_file_cmd;
};

#endif // __TELNET_RECORDER_H__
