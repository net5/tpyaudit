#include "ts_env.h"
#include "ts_ver.h"

#include <ex.h>
#include <pys.h>

// �����в���˵�������������������Է���ʽ������
// tp_web [-i|-u|--version] [ [-d] start] [...]
//   -d          �����������������Ϣ����������Ϊ�ػ�����/����ģʽ��
//   -i          ��װ����Ȼ���˳�������Winƽ̨��
//   -u          ж�ط���Ȼ���˳�������Winƽ̨��
//   --version  ��ӡ�汾��Ȼ���˳�
//   start       �Է���ʽ����
//   ...         ʣ������в��������ݸ�python�ű�
//
// 
// ִ��ָ����Python�ű���
// tp_web --py [-f FuncName] script_file.py ...
//   --py             ����Ϊ��һ����������ʾ����ִ��Ϊִ��ָ���ű�
//   -f FuncName      ָ����ں�����Ĭ��Ϊmain��
//   script-file.py   ��ִ�еĽű��ļ�
//   ...              ʣ������в��������ݸ�Python�ű�

ExLogger g_ex_logger;
bool g_is_debug = false;
static ex_wstrs g_py_args;

// �����ִ��ָ���ű�
static ex_wstr g_py_script_file;
static ex_wstr g_py_main_func;

#define RUN_UNKNOWN			0
#define RUN_WEB				1
#define RUN_PY_SCRIPT			2
#define RUN_INSTALL_SRV		3
#define RUN_UNINST_SRV		4
static ex_u8 g_run_type = RUN_UNKNOWN;

#define EOM_WEB_SERVICE_NAME		L"Teleport Web Service"

static bool _run_daemon(void);

// ����������Python�ű�ʹ�ã���Ҫ��Ϊ�˼�¼��־��
//   Windowsƽ̨�ϣ�tp_web�������־�ļ�д֮��Python�ű�����д�뷽ʽ�򿪴���־�ļ�ʱ��ʧ�ܡ�
PyObject* init_web_builtin_module(void);

#ifdef EX_OS_WIN32
static int service_install()
{
	ex_wstr exec_file(g_env.m_exec_file);
	exec_file += L" start";

	if (EXRV_OK == ex_winsrv_install(EOM_WEB_SERVICE_NAME, EOM_WEB_SERVICE_NAME, exec_file))
		return 0;
	else
		return 1;
}

static int service_uninstall()
{
	if (EXRV_OK != ex_winsrv_stop(EOM_WEB_SERVICE_NAME))
		return 1;

	if (EXRV_OK != ex_winsrv_uninstall(EOM_WEB_SERVICE_NAME))
		return 2;

	return 0;
}
#endif

static bool _process_cmd_line(int argc, wchar_t** argv)
{
	if (argc <= 1)
	{
		EXLOGE("[tpweb] nothing to do.\n\n");
		return false;
	}

	g_run_type = RUN_UNKNOWN;
	bool is_py_arg = false;
	bool is_py_func = false;

	if (0 == wcscmp(argv[1], L"--version"))
	{
		EXLOGV("\nTeleport Web Server, version %ls.\n\n", TP_SERVER_VER);
		return false;
	}
	else if (0 == wcscmp(argv[1], L"--py"))
	{
		g_run_type = RUN_PY_SCRIPT;

		for (int i = 2; i < argc; ++i)
		{
			if (is_py_arg)
			{
				g_py_args.push_back(argv[i]);
				continue;
			}

			if(is_py_func)
            {
                g_py_main_func = argv[i];
                is_py_func = false;
			    continue;
            }

			if (0 == wcscmp(argv[i], L"-f"))
			{
			    is_py_func = true;
				continue;
			}

			if (g_py_script_file.length() == 0)
			{
				g_py_script_file = argv[i];
				is_py_arg = true;
				continue;
			}
		}
	}
	else if (0 == wcscmp(argv[1], L"-i"))
	{
		g_run_type = RUN_INSTALL_SRV;
	}
	else if (0 == wcscmp(argv[1], L"-u"))
	{
		g_run_type = RUN_UNINST_SRV;
	}
	else
	{
		for (int i = 1; i < argc; ++i)
		{
			if (is_py_arg)
			{
				g_py_args.push_back(argv[i]);
				continue;
			}
			if (0 == wcscmp(argv[i], L"start"))
			{
				g_run_type = RUN_WEB;
				is_py_arg = true;
				continue;
			}

			if (0 == wcscmp(argv[i], L"-d"))
			{
				g_is_debug = true;
				continue;
			}

			EXLOGE(L"[tpweb] Unknown option: %ls\n", argv[i]);
			return false;
		}
	}

	if (g_run_type == RUN_UNKNOWN)
	{
		EXLOGE("[tpweb] nothing to do.\n\n");
		return false;
	}

	return true;
}


static int _main_loop(void)
{
	PYS_HANDLE pysh = pys_create();
	if (NULL == pysh)
	{
		EXLOGE("pys_create() failed.\n");
		return 1;
	}
	PysHandleHolder hh(pysh);

	ex_wstr pysrt_path(g_env.m_exec_path);
	if(!ex_path_join(pysrt_path, false, L"pysrt", NULL))
	{
		EXLOGE("pysrt not exists.\n");
		return 1;
	}

	if (!pys_init_runtime(pysh, g_env.m_exec_file.c_str(), pysrt_path.c_str()))
	{
		EXLOGE("pys_init_runtime() failed.\n");
		return 1;
	}

	// ����web��·��
	ex_wstr sf_path;
	if (g_run_type == RUN_WEB)
	{
		sf_path = g_env.m_www_path;

		if (!ex_path_join(sf_path, false, L"tpyaudit", L"app_bootstrap.py", NULL))
		{
			EXLOGE(L"[tpweb] invalid path [%ls].\n", sf_path.c_str());
			return 1;
		}

		if (ex_is_file_exists(sf_path.c_str()))
		{
			pys_set_startup_file(pysh, sf_path.c_str());
		}
		else
		{
			EXLOGE(L"[tpweb] tpyaudit web app not found at [%ls].\n", sf_path.c_str());
			return 1;
		}
	}
	else if (g_run_type == RUN_PY_SCRIPT)
	{
		sf_path = g_env.m_exec_path;

		if (!ex_is_file_exists(g_py_script_file.c_str()))
		{
			EXLOGE("[tpweb] file not found: [%s].\n", g_py_script_file.c_str());
			return 1;
		}

		if (g_py_main_func.length() == 0)
		{
			pys_set_startup_file(pysh, g_py_script_file.c_str());
		}
		else
		{
//			ex_astr file_name;
			ex_astr func_name;
//			ex_wstr2astr(g_py_script_file, file_name);
			ex_wstr2astr(g_py_main_func, func_name);
//
//			pys_set_bootstrap_module(pysh, file_name.c_str(), func_name.c_str());

			pys_set_bootstrap_module(pysh, NULL, func_name.c_str());
            pys_set_startup_file(pysh, g_py_script_file.c_str());
		}

//        ex_astr add_path("/home/apex/work/tp-py37/server/www/tpyaudit");
//        ex_wstr add_path_w;
//        ex_astr2wstr(add_path, add_path_w);
//
//        pys_add_search_path(pysh, add_path_w.c_str());

	}

	ex_wstrs::const_iterator it = g_py_args.begin();
	for (; it != g_py_args.end(); ++it)
	{
		pys_add_arg(pysh, it->c_str());
	}

	if (!pys_add_builtin_module(pysh, "tpweb", init_web_builtin_module))
	{
		EXLOGE("[tpweb] can not add builtin module for python script.\n");
		return 1;
	}


	return pys_run(pysh);
}

int _app_main(int argc, wchar_t** argv)
{
	EXLOG_USE_LOGGER(&g_ex_logger);

	if (!_process_cmd_line(argc, argv))
		return 1;

	if (g_run_type == RUN_PY_SCRIPT)
	{
		if (!g_env.init(false))
		{
			EXLOGE("[tpweb] env init failed.\n");
			return 1;
		}

		return _main_loop();
	}

#ifdef EX_DEBUG
	EXLOG_LEVEL(EX_LOG_LEVEL_DEBUG);
#endif

#ifdef EX_OS_WIN32
	if (g_run_type == RUN_INSTALL_SRV)
	{
		if (!g_env.init(false))
		{
			EXLOGE("[tpweb] env init failed.\n");
			return 1;
		}
		return service_install();
	}
	else if(g_run_type == RUN_UNINST_SRV)
	{
		if (!g_env.init(false))
		{
			EXLOGE("[tpweb] env init failed.\n");
			return 1;
		}
		return service_uninstall();
	}
#endif

	if (!g_env.init(true))
	{
		EXLOGE("[tpweb] env init failed.\n");
		return 1;
	}

	if (!g_is_debug)
	{
		if (!_run_daemon())
		{
			EXLOGE("[tpweb] can not run in daemon mode.\n");
			return 1;
		}

#ifdef EX_OS_WIN32
		return 0;
#endif
	}

	return _main_loop();
}



#ifdef EX_OS_WIN32

// #ifdef EX_DEBUG
// #include <vld.h>
// #endif

static SERVICE_STATUS g_ServiceStatus = { 0 };
static SERVICE_STATUS_HANDLE g_hServiceStatusHandle = NULL;
HANDLE g_hWorkerThread = NULL;

VOID WINAPI service_main(DWORD argc, wchar_t** argv);
void WINAPI service_handler(DWORD fdwControl);

static DWORD WINAPI service_thread_func(LPVOID lpParam);

int main()
{
	int ret = 0;
	LPWSTR szCmdLine = (LPWSTR)::GetCommandLineW(); //��ȡ�����в�����

	int _argc = 0;
	wchar_t** _argv = ::CommandLineToArgvW(szCmdLine, &_argc); //��������в����ַ�����

	ret = _app_main(_argc, _argv);
	
	LocalFree(_argv);
	_argv = NULL;

	return ret;
}

static bool _run_daemon(void)
{
	SERVICE_TABLE_ENTRY DispatchTable[2];
	DispatchTable[0].lpServiceName = EOM_WEB_SERVICE_NAME;
	DispatchTable[0].lpServiceProc = service_main;
	DispatchTable[1].lpServiceName = NULL;
	DispatchTable[1].lpServiceProc = NULL;

	if (!StartServiceCtrlDispatcher(DispatchTable))
	{
		EXLOGE_WIN("StartServiceCtrlDispatcher()");
		return false;
	}

	return true;
}


static DWORD WINAPI service_thread_func(LPVOID lpParam)
{
	int ret = _main_loop();

	// ���·���״̬��������������У���������Ϊֹͣ״̬��
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	g_ServiceStatus.dwCheckPoint = 0;
	g_ServiceStatus.dwWaitHint = 0;
	if (!SetServiceStatus(g_hServiceStatusHandle, &g_ServiceStatus))
		EXLOGE_WIN("SetServiceStatus()");

	return ret;
}

static void WINAPI service_handler(DWORD fdwControl)
{
	switch (fdwControl)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
	{
		if (g_hWorkerThread)
		{
			TerminateThread(g_hWorkerThread, 1);
			g_hWorkerThread = NULL;
		}

		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwCheckPoint = 0;
		g_ServiceStatus.dwWaitHint = 0;

	}break;

	default:
		return;
	};

	if (!SetServiceStatus(g_hServiceStatusHandle, &g_ServiceStatus))
	{
		EXLOGE_WIN("SetServiceStatus(STOP)");
		return;
	}
}

VOID WINAPI service_main(DWORD argc, wchar_t** argv)
{
	g_ServiceStatus.dwServiceType = SERVICE_WIN32;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;
	g_ServiceStatus.dwWaitHint = 0;
	g_hServiceStatusHandle = RegisterServiceCtrlHandler(EOM_WEB_SERVICE_NAME, service_handler);
	if (g_hServiceStatusHandle == 0)
	{
		EXLOGE_WIN("RegisterServiceCtrlHandler()");
		return;
	}

	DWORD tid = 0;
	g_hWorkerThread = CreateThread(NULL, 0, service_thread_func, NULL, 0, &tid);
	if (NULL == g_hWorkerThread)
	{
		EXLOGE_WIN("CreateThread(python)");

		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwCheckPoint = 0;
		g_ServiceStatus.dwWaitHint = 0;
		if (!SetServiceStatus(g_hServiceStatusHandle, &g_ServiceStatus))
			EXLOGE_WIN("SetServiceStatus()");

		return;
	}

	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwCheckPoint = 0;
	g_ServiceStatus.dwWaitHint = 9000;
	if (!SetServiceStatus(g_hServiceStatusHandle, &g_ServiceStatus))
	{
		EXLOGE_WIN("SetServiceStatus()");
		return;
	}
}

//======================================================
#else // Linux or MacOS

#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>

static void _sig_handler(int signum, siginfo_t* info, void* ptr);
//static int _daemon(int nochdir, int noclose);

int main(int argc, char** argv)
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = _sig_handler;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &act, NULL);

	wchar_t** wargv = ex_make_wargv(argc, argv);
	int ret = _app_main(argc, wargv);

	ex_free_wargv(argc, wargv);

	return ret;
}

void _sig_handler(int signum, siginfo_t* info, void* ptr)
{
	if (signum == SIGINT || signum == SIGTERM)
	{
		EXLOGW("[tpweb] received signal SIGINT, exit now.\n");
		exit(1);
	}
}

static bool _run_daemon(void)
{
	pid_t pid = fork();
	if (pid < 0)
	{
		EXLOGE("[tpweb] can not fork daemon.\n");
		exit(EXIT_FAILURE);
	}
	else if (pid > 0)
	{
		exit(EXIT_SUCCESS);	// parent exit.
	}

	// now I'm first children.
	if (setsid() == -1)
	{
		EXLOGE("[tpweb] setsid() failed.\n");
		assert(0);
		exit(EXIT_FAILURE);
	}

	umask(0);

	pid = fork();
	if (pid < 0)
	{
		EXLOGE("[tpweb] can not fork daemon.\n");
		exit(EXIT_FAILURE);
	}
	else if (pid > 0)
	{
		exit(0);	// first children exit.
	}

	// now I'm second children.
	int ret = chdir("/");
	close(STDIN_FILENO);

	int stdfd = open("/dev/null", O_RDWR);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	dup2(stdfd, STDOUT_FILENO);
	dup2(stdfd, STDERR_FILENO);

	return true;
}

#endif





//===============================================================
// �����ڽ�ģ�鹩�ű�����
//===============================================================
PyObject* _py_log_output(PyObject* self, PyObject* args)
{
// 	UNUSED(self);
// 	UNUSED(args);

	int level = 0;
	const char* msg = NULL;
	if (!pylib_PyArg_ParseTuple(args, "is", &level, &msg))
	{
		EXLOGE("invalid args for _py_log_output().\n");
		PYLIB_RETURN_FALSE;
	}

	ex_wstr tmp;
	ex_astr2wstr(msg, tmp, EX_CODEPAGE_UTF8);
//    EXLOGE(L"(%d) %ls.\n", level, tmp.c_str());

	switch (level)
	{
	case EX_LOG_LEVEL_DEBUG:
		ex_printf_d(tmp.c_str());
		break;
	case EX_LOG_LEVEL_VERBOSE:
		ex_printf_v(tmp.c_str());
		break;
	case EX_LOG_LEVEL_INFO:
		ex_printf_i(tmp.c_str());
		break;
	case EX_LOG_LEVEL_WARN:
		ex_printf_w(tmp.c_str());
		break;
	case EX_LOG_LEVEL_ERROR:
		ex_printf_e(tmp.c_str());
		break;
	default:
		PYLIB_RETURN_FALSE;
//		break;
	}

	//return pylib_PyLong_FromLong(0x010001);
	PYLIB_RETURN_TRUE;
}

PyObject* _py_log_level(PyObject* self, PyObject* args)
{
	int level = 0;
	if (!pylib_PyArg_ParseTuple(args, "i", &level))
	{
		EXLOGE("invalid args for _py_log_level().\n");
		PYLIB_RETURN_FALSE;
	}

	EXLOG_LEVEL(level);

	PYLIB_RETURN_TRUE;
}

PyObject* _py_log_console(PyObject* self, PyObject* args)
{
	bool to_console = false;
	if (!pylib_PyArg_ParseTuple(args, "p", &to_console))
	{
		EXLOGE("invalid args for _py_log_console().\n");
		PYLIB_RETURN_FALSE;
	}

//	EXLOGE("to_console=%s\n", to_console?"true":"false");

	EXLOG_CONSOLE(to_console);

	PYLIB_RETURN_TRUE;
}

PYS_BUILTIN_FUNC _demo_funcs[] = {
	{
		"log_output",			// �ű����������ڽű���ʹ��
		_py_log_output,		// ��Ӧ��C���뺯����
		PYS_TRUE,		// �����Ļ�����Ϣ���Ƿ���Ҫ�������ȵȣ�
		"write log."	// ������˵���ĵ�����ѡ�������ǿ��ַ�����
	},

	{
		"log_level",
		_py_log_level,
		PYS_TRUE,
		"set log level."
	},

	{
		"log_console",
		_py_log_console,
		PYS_TRUE,
		"set log to console or not."
	},

	// ���һ�飬��һ����ԱΪ��ָ�룬��ʾ����
	{ NULL, NULL, 0, NULL }
};

PyObject* init_web_builtin_module(void)
{
	PyObject* mod = NULL;

	mod = pys_create_module("_tpweb", _demo_funcs);

	pys_builtin_const_long(mod, "EX_LOG_LEVEL_DEBUG", EX_LOG_LEVEL_DEBUG);
	pys_builtin_const_long(mod, "EX_LOG_LEVEL_VERBOSE", EX_LOG_LEVEL_VERBOSE);
	pys_builtin_const_long(mod, "EX_LOG_LEVEL_INFO", EX_LOG_LEVEL_INFO);
	pys_builtin_const_long(mod, "EX_LOG_LEVEL_WARN", EX_LOG_LEVEL_WARN);
	pys_builtin_const_long(mod, "EX_LOG_LEVEL_ERROR", EX_LOG_LEVEL_ERROR);

// 	pys_builtin_const_bool(mod, "DEMO_CONST_2", PYS_TRUE);
// 	//pys_builtin_const_wcs(mod, "DEMO_CONST_3", L"STRING ���Ĳ��� this is string.");
// 	pys_builtin_const_wcs(mod, "DEMO_CONST_3", L"STRING this is string.");
// 	pys_builtin_const_utf8(mod, "DEMO_CONST_4", "this is string.");
//
// 	ex_u8 test_buf[12] = { 0x01, 0x02, 0x03, 0x04, 0x0a, 0x0b, 0x0c, 0x0d, 0x12, 0x34, 0xab, 0xcd };
// 	pys_builtin_const_bin(mod, "DEMO_CONST_5", test_buf, 12);

	return mod;
}
