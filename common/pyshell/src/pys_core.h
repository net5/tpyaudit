#ifndef __PYS_CORE_H__
#define __PYS_CORE_H__

//#include "pys_str.h"

#include "pys_util.h"
#include <map>

namespace pys
{
	typedef std::map<ex_astr, pys_init_module_func> builtin_modules;
	typedef std::list<ex_astr> pys_astr_list;
	typedef std::list<ex_wstr> pys_wstr_list;

	class Core
	{
	public:
		Core();
		~Core();

		// ���ø�����Ա�������ñ�Ҫ��Ϣ֮��������
		int run(void);

		// ��ʼ��ΪĬ������
		bool init(const wchar_t* exec_file, const wchar_t* runtime_path);

		bool add_search_path(const wchar_t* wpath);
		bool add_search_path(const char* apath, int code_page = EX_CODEPAGE_DEFAULT);

		bool set_startup_file(const wchar_t* filename);
		bool add_builtin_module(const char* module_name, pys_init_module_func init_func);
		bool get_builtin_module_by_init_func(pys_init_module_func init_func, ex_astr& module_name);

	private:
		bool _load_dylib(void);

		bool _run_init_builtin_modules(void);
		bool _run_prepare(void);
		void _run_set_program(void);
		void _run_set_path(void);
		void _run_set_argv(void);


	public:
		ex_wstr m_prog_name;	// ���ݸ�Python�������ģ����û�����ô�ֵ����Ĭ��ʹ��m_exec_file��
		ex_astr m_bootstrap_module;
		ex_astr m_bootstrap_func;
		pys_wstr_list m_py_args;		// ���ݸ�Python�ű��Ĳ���

	private:
		bool m_is_zipped_app;

		ex_wstr m_exec_file;	// ��ǰ��ִ�г�����ļ���������·����
		ex_wstr m_exec_path;	// ��ǰ��ִ�г������ڵ�·��������·����
		ex_wstr m_runtime_path;	// python���л���·����Ĭ��Ϊ��ִ�г�������·���µ� `pysrt` Ŀ¼��
		ex_wstr m_start_file;
		ex_wstr m_search_path_tmp;

		pys_wstr_list m_search_path;

		builtin_modules m_builtin_modules;
		struct _inittab* m_init_tab;
	};


	typedef struct BUILTIN_MODULE_INFO
	{
		PyMethodDef* method_def;
		PyModuleDef* module_def;
	}BUILTIN_MODULE_INFO;

	typedef std::list<BUILTIN_MODULE_INFO*> builtin_module_infos;

	class BuiltinModuleInfo
	{
	public:
		BuiltinModuleInfo();
		~BuiltinModuleInfo();

		void add(PyMethodDef* method_def, PyModuleDef* module_def);

	private:
		builtin_module_infos m_infos;
	};

	extern BuiltinModuleInfo g_builtin_module_info;

} // namespace pys

#endif // __PYS_CORE_H__
