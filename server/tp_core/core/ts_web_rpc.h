#ifndef __TS_WEB_RPC_H__
#define __TS_WEB_RPC_H__

#include "ts_session.h"

#include <json/json.h>

// ����log��־״̬
bool ts_web_rpc_register_core();

// ��������ID��ȡԶ��������Ϣ������������IP���˿ڣ��û����������˽Կ��Э����RDP��SSH�ȵȣ�
int ts_web_rpc_get_conn_info(int conn_id, TS_CONNECT_INFO& info);

// ��¼�Ự�Ŀ�ʼ
bool ts_web_rpc_session_begin(TS_CONNECT_INFO& info, int& record_id);
// update session state
bool ts_web_rpc_session_update(int id, int protocol_sub_type, int state);
//session ����
bool ts_web_rpc_session_end(const char* sid, int id, int ret_code);


#endif // __TS_WEB_RPC_H__
