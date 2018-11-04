#ifndef __TELNET_SESSION_H__
#define __TELNET_SESSION_H__

#include <ex.h>

#include "telnet_recorder.h"
#include "telnet_conn.h"

enum sess_state {
	s_noop,	// ʲô�����������ض�״̬����

	s_client_connect,	// �ͻ�������
	s_negotiation_with_client,	// ��ͻ��˽������֣�ֱ���õ��ͻ��˷����ĵ�¼�û�������ʵ��SessionID��

	s_server_connected,	// �ɹ������Ϸ�������

	s_relay,	// ����ת������

	s_close,	// �رջỰ
	s_closing,			// ���ڹر�˫������
	s_all_conn_closed,	// ���˾��ѹر�

	s_dead	// �������ٴ˻Ự��
};

class TelnetProxy;

class TelnetSession
{
public:
	TelnetSession(TelnetProxy* proxy);
	virtual ~TelnetSession();

	TelnetProxy* get_proxy() { return m_proxy; }

	TelnetConn* client() { return m_conn_client; }
	TelnetConn* server() { return m_conn_server; }
	uv_loop_t* get_loop();

	void set_state(int state) { m_state = state; }
	void close(int err_code);
	void on_conn_close();

	void do_next(TelnetConn* conn);
	void do_next(TelnetConn* conn, sess_state status);

	// save record cache into file. be called per 5 seconds.
	void save_record();
	void record(ex_u8 type, const ex_u8* data, size_t size) {
		m_rec.record(type, data, size);
	}
	// 
	void check_noop_timeout(ex_u32 t_now, ex_u32 timeout);
	const ex_astr& sid() { return m_sid; }

	void client_addr(const char* addr) { m_client_addr = addr; }
	const char* client_addr() const { return m_client_addr.c_str(); }

	bool is_relay() { return m_is_relay; }
	bool is_closed() { return m_is_closed; }

protected:
	// �̳��� TppSessionBase
	bool _on_session_begin();
	bool _on_session_end();
	void _session_error(int err_code);

private:
	sess_state _do_client_connect(TelnetConn* conn);
	sess_state _do_negotiation_with_client(TelnetConn* conn);
	sess_state _do_connect_server();
	sess_state _do_server_connected();
	sess_state _do_relay(TelnetConn* conn);
	sess_state _do_close(int err_code);
	sess_state _do_check_closing();

	bool _parse_find_and_send(TelnetConn* conn_recv, TelnetConn* conn_remote, const char* find, const char* send);
	bool _putty_replace_username(TelnetConn* conn_recv, TelnetConn* conn_remote);
	bool _parse_win_size(TelnetConn* conn);

private:
	int m_state;

	TPP_CONNECT_INFO* m_conn_info;
	bool m_startup_win_size_recorded;
	int m_db_id;

	bool m_first_client_pkg;
	bool m_is_relay;	// �Ƿ����relayģʽ�ˣ�ֻ�н���relayģʽ�Ż���¼����ڣ�
	bool m_is_closed;
	ex_u32 m_last_access_timestamp;

	TppTelnetRec m_rec;
	int m_win_width;
	int m_win_height;

	TelnetProxy* m_proxy;
	TelnetConn* m_conn_client; // �������ͻ���ͨѶ�����ӣ�������Ϊ����ˣ�
	TelnetConn* m_conn_server; // �����������ͨѶ�����ӣ�������Ϊ�ͻ��ˣ�

	ExThreadLock m_lock;

	ex_astr m_sid;
	ex_astr m_conn_ip;
	ex_u16 m_conn_port;
	ex_astr m_acc_name;
	ex_astr m_acc_secret;
	ex_astr m_username_prompt;
	ex_astr m_password_prompt;

	sess_state m_status;
	ex_astr m_client_addr;

	bool m_is_putty_mode;
	//bool m_is_putty_eat_username;

	bool m_username_sent;
	bool m_password_sent;
};

#endif // __TELNET_SESSION_H__
