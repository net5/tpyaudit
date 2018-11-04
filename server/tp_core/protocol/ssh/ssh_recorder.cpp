#include "ssh_recorder.h"
//#include <teleport_const.h>

static ex_u8 TPP_RECORD_MAGIC[4] = {'T', 'P', 'P', 'R'};

TppSshRec::TppSshRec() {
    m_cmd_cache.reserve(MAX_SIZE_PER_FILE);

    memset(&m_head, 0, sizeof(TS_RECORD_HEADER));
    memcpy((ex_u8 *) (&m_head.info.magic), TPP_RECORD_MAGIC, sizeof(ex_u32));
    m_head.info.ver = 0x03;
    m_header_changed = false;
    m_save_full_header = false;

    m_file_info = NULL;
    m_file_data = NULL;
    m_file_cmd = NULL;

}

TppSshRec::~TppSshRec() {
    end();
}

bool TppSshRec::_on_begin(const TPP_CONNECT_INFO *info) {
    if (NULL == info)
        return false;
    m_head.basic.timestamp = (ex_u64) time(NULL);
    m_head.basic.protocol_type = (ex_u16) info->protocol_type;
    m_head.basic.protocol_sub_type = (ex_u16) info->protocol_sub_type;
    m_head.basic.conn_port = (ex_u16) info->conn_port;

    memcpy(m_head.basic.acc_username, info->acc_username, strlen(info->acc_username) >= 63 ? 63 : strlen(info->acc_username));
    memcpy(m_head.basic.user_username, info->user_username, strlen(info->user_username) >= 63 ? 63 : strlen(info->user_username));
    memcpy(m_head.basic.host_ip, info->host_ip, strlen(info->host_ip) >= 39 ? 39 : strlen(info->host_ip));
    memcpy(m_head.basic.conn_ip, info->conn_ip, strlen(info->conn_ip) >= 39 ? 39 : strlen(info->conn_ip));
    memcpy(m_head.basic.client_ip, info->client_ip, strlen(info->client_ip) >= 39 ? 39 : strlen(info->client_ip));

    return true;
}

bool TppSshRec::_on_end() {
    // �������ʣ��δд������ݣ�д���ļ��С�
    save_record();

    if (m_file_info != NULL)
        fclose(m_file_info);
    if (m_file_data != NULL)
        fclose(m_file_data);
    if (m_file_cmd != NULL)
        fclose(m_file_cmd);

    return true;
}

void TppSshRec::save_record() {
    _save_to_data_file();
    _save_to_cmd_file();
}

void TppSshRec::record(ex_u8 type, const ex_u8 *data, size_t size) {
    if (data == NULL || 0 == size)
        return;

    if (sizeof(TS_RECORD_PKG) + size + m_cache.size() > MAX_SIZE_PER_FILE)
        _save_to_data_file();

    TS_RECORD_PKG pkg = {0};
    //memset(&pkg, 0, sizeof(TS_RECORD_PKG));
    pkg.type = type;
    pkg.size = (ex_u32) size;

    if (m_start_time > 0) {
        pkg.time_ms = (ex_u32) (ex_get_tick_count() - m_start_time);
        m_head.info.time_ms = pkg.time_ms;
    }

    m_cache.append((ex_u8 *) &pkg, sizeof(TS_RECORD_PKG));
    m_cache.append(data, size);

    m_head.info.packages++;
    m_header_changed = true;
}

void TppSshRec::record_win_size_startup(int width, int height) {
    m_head.basic.width = (ex_u16) width;
    m_head.basic.height = (ex_u16) height;
    m_save_full_header = true;
}

void TppSshRec::record_win_size_change(int width, int height) {
    TS_RECORD_WIN_SIZE pkg = {0};
    pkg.width = (ex_u16) width;
    pkg.height = (ex_u16) height;
    record(TS_RECORD_TYPE_SSH_TERM_SIZE, (ex_u8 *) &pkg, sizeof(TS_RECORD_WIN_SIZE));
}

// Ϊ��¼��طź�������ʷ�ܹ���Ӧ������������ֱ������¼��Ķ�Ӧʱ�㣩������¼�����ݰ��ķ�ʽ��¼���ʱ��ƫ�ƣ������Ǿ���ʱ�䡣
void TppSshRec::record_command(int flag, const ex_astr &cmd) {
    char szTime[100] = {0};
#ifdef EX_OS_WIN32
    // 	SYSTEMTIME st;
    // 	GetLocalTime(&st);
    // 	sprintf_s(szTime, 100, "[%04d-%02d-%02d %02d:%02d:%02d %d] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, flag);
#else
// 	time_t timep;
// 	struct tm *p;
// 	time(&timep);
// 	p = localtime(&timep);
// 	if (p == NULL)
// 		return;
// 	sprintf(szTime, "[%04d-%02d-%02d %02d:%02d:%02d %d] ", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, flag);
#endif

    ex_strformat(szTime, 99, "%d,%d,", (ex_u32) (ex_get_tick_count() - m_start_time), flag);
    size_t lenTime = strlen(szTime);

    if (m_cmd_cache.size() + cmd.length() + lenTime > MAX_SIZE_PER_FILE)
        _save_to_cmd_file();

    m_cmd_cache.append((ex_u8 *) szTime, lenTime);
    m_cmd_cache.append((ex_u8 *) cmd.c_str(), cmd.length());
    m_cmd_cache.append((ex_u8 *) "\r\n", 2);
}

bool TppSshRec::_save_to_info_file() {
    if (!m_header_changed)
        return true;

    if (m_file_info == NULL) {
        ex_wstr fname = m_base_path;
        if(!ex_path_join(fname, false, m_base_fname.c_str(), NULL))
            return false;

        fname += L".tpr";
        m_file_info = ex_fopen(fname, L"wb");
        if (NULL == m_file_info) {
            EXLOGE("[ssh] can not open record info-file for write.\n");
            return false;
        }

        // first time to save header, write whole header.
        m_save_full_header = true;
    }

    fseek(m_file_info, 0L, SEEK_SET);
    if (m_save_full_header) {
        fwrite(&m_head, ts_record_header_size, 1, m_file_info);
        fflush(m_file_info);
        m_save_full_header = false;
    } else {
        fwrite(&m_head.info, ts_record_header_info_size, 1, m_file_info);
        fflush(m_file_info);
    }

    return true;
}

bool TppSshRec::_save_to_data_file() {
    if (m_cache.size() == 0)
        return true;

    if (m_file_data == NULL) {
        ex_wstr fname = m_base_path;
        if(!ex_path_join(fname, false, m_base_fname.c_str(), NULL))
            return false;

        fname += L".dat";
        m_file_data = ex_fopen(fname, L"wb");
        if (NULL == m_file_data) {
            EXLOGE("[ssh] can not open record data-file for write.\n");
            return false;
        }

        m_header_changed = true;
    }

    fwrite(m_cache.data(), m_cache.size(), 1, m_file_data);
    fflush(m_file_data);
    m_cache.empty();

    return _save_to_info_file();
}

bool TppSshRec::_save_to_cmd_file() {
    if (m_cmd_cache.size() == 0)
        return true;

    if (NULL == m_file_cmd) {
        ex_wstr fname = m_base_path;
        if (!ex_path_join(fname, false, m_base_fname.c_str(), NULL))
            return false;

        fname += L"-cmd.txt";
        m_file_cmd = ex_fopen(fname, L"wb");
        if (NULL == m_file_cmd) {
            EXLOGE("[ssh] can not open record cmd-file for write.\n");
            return false;
        }

        m_header_changed = true;
    }

    fwrite(m_cmd_cache.data(), m_cmd_cache.size(), 1, m_file_cmd);
    fflush(m_file_cmd);
    m_cmd_cache.empty();

    return _save_to_info_file();
}