#ifndef __TP_PROTOCOL_INTERFACE_H__
#define __TP_PROTOCOL_INTERFACE_H__

#include "ts_const.h"
#include <ex.h>

#ifdef EX_OS_WIN32
#	ifdef TPP_EXPORTS
#		define TPP_API __declspec(dllexport)
#	else
#		define TPP_API __declspec(dllimport)
#	endif
#else
#	define TPP_API
#endif

#define TPP_CMD_INIT			0x00000000
#define TPP_CMD_SET_RUNTIME_CFG	0x00000005
#define TPP_CMD_KILL_SESSIONS	0x00000006

typedef struct TPP_CONNECT_INFO
{
	char* sid;

	// ���������Ϣ��ص�����Ҫ�ص�ID
	int user_id;
	int host_id;
	int acc_id;

	char* user_username;		// ���뱾�����ӵ��û���

	char* host_ip;	// ������Զ������IP�������ֱ������ģʽ������remote_host_ip��ͬ��
	char* conn_ip;	// Ҫ���ӵ�Զ��������IP������Ƕ˿�ӳ��ģʽ����Ϊ·��������IP��
	int conn_port;	// Ҫ���ӵ�Զ�������Ķ˿ڣ�����Ƕ˿�ӳ��ģʽ����Ϊ·�������Ķ˿ڣ�
	char* client_ip;

	char* acc_username;		// Զ���������˺�
	char* acc_secret;	// Զ�������˺ŵ����루����˽Կ��
	char* username_prompt;	// for telnet
	char* password_prompt;	// for telnet

	int protocol_type;
	int protocol_sub_type;
	int protocol_flag;
	int record_flag;
	int auth_type;
}TPP_CONNECT_INFO;

typedef TPP_CONNECT_INFO* (*TPP_GET_CONNNECT_INFO_FUNC)(const char* sid);
typedef void(*TPP_FREE_CONNECT_INFO_FUNC)(TPP_CONNECT_INFO* info);
typedef bool(*TPP_SESSION_BEGIN_FUNC)(const TPP_CONNECT_INFO* info, int* db_id);
typedef bool(*TPP_SESSION_UPDATE_FUNC)(int db_id, int protocol_sub_type, int state);
typedef bool(*TPP_SESSION_END_FUNC)(const char* sid, int db_id, int ret);


typedef struct TPP_INIT_ARGS
{
	ExLogger* logger;
	ex_wstr exec_path;
	ex_wstr etc_path;
	ex_wstr replay_path;
	ExIniFile* cfg;

	TPP_GET_CONNNECT_INFO_FUNC func_get_connect_info;
	TPP_FREE_CONNECT_INFO_FUNC func_free_connect_info;
	TPP_SESSION_BEGIN_FUNC func_session_begin;
	TPP_SESSION_UPDATE_FUNC func_session_update;
	TPP_SESSION_END_FUNC func_session_end;
}TPP_INIT_ARGS;

// typedef struct TPP_SET_CFG_ARGS {
// 	ex_u32 noop_timeout; // as second.
// }TPP_SET_CFG_ARGS;

#ifdef __cplusplus
extern "C"
{
#endif

	TPP_API ex_rv tpp_init(TPP_INIT_ARGS* init_args);
	TPP_API ex_rv tpp_start(void);
	TPP_API ex_rv tpp_stop(void);
	TPP_API void tpp_timer(void);
// 	TPP_API void tpp_set_cfg(TPP_SET_CFG_ARGS* cfg_args);

	TPP_API ex_rv tpp_command(ex_u32 cmd, const char* param);

#ifdef __cplusplus
}
#endif

typedef ex_rv (*TPP_INIT_FUNC)(TPP_INIT_ARGS* init_args);
typedef ex_rv (*TPP_START_FUNC)(void);
typedef ex_rv(*TPP_STOP_FUNC)(void);
typedef void(*TPP_TIMER_FUNC)(void);
// typedef void(*TPP_SET_CFG_FUNC)(TPP_SET_CFG_ARGS* cfg_args);

typedef ex_rv(*TPP_COMMAND_FUNC)(ex_u32 cmd, const char* param); // param is a JSON formatted string.

#endif // __TP_PROTOCOL_INTERFACE_H__
