#ifndef __PYS_H__
#define __PYS_H__

#include <ex.h>

//=========================================================================
// Type define
//=========================================================================
#if defined(EX_OS_WIN32)
#	define DYLIB_HANDLE		HINSTANCE
#else
#	define DYLIB_HANDLE		void*
#endif

//=========================================================================
// Python API
//=========================================================================
#define MS_NO_COREDLL 1
#ifdef __cplusplus
extern "C" {
#endif

#if defined(EX_OS_WIN32)
#	define PYS_USE_PYLIB_SHARED
#	include <Python.h>
#elif defined(EX_OS_LINUX)
#	define PYS_USE_PYLIB_STATIC
#	include <Python.h>
#else
#	error This platform not supported yet.
#endif

#ifdef __cplusplus
}
#endif

#ifdef PYS_USE_PYLIB_SHARED
//========================================================
// WIN32
//========================================================
#define EXTDECLPROC(result, name, args) \
    typedef result (__cdecl* __PROC__ ## name) args; \
    extern __PROC__ ## name pylib_ ## name;

#define EXTDECLVAR(vartyp, name) \
    typedef vartyp __VAR__ ## name; \
    extern __VAR__ ## name* pylib_ ## name;


EXTDECLVAR(int, Py_FrozenFlag);
EXTDECLVAR(int, Py_NoSiteFlag);
EXTDECLVAR(int, Py_OptimizeFlag);
EXTDECLVAR(const char*, Py_FileSystemDefaultEncoding);
EXTDECLVAR(int, Py_VerboseFlag);
EXTDECLVAR(int, Py_IgnoreEnvironmentFlag);
EXTDECLVAR(int, Py_DontWriteBytecodeFlag);
EXTDECLVAR(int, Py_NoUserSiteDirectory);

EXTDECLPROC(void, Py_Initialize, (void));
EXTDECLPROC(void, Py_Finalize, (void));
EXTDECLPROC(void, Py_IncRef, (PyObject *));
EXTDECLPROC(void, Py_DecRef, (PyObject *));
EXTDECLPROC(void, Py_SetProgramName, (wchar_t *));
EXTDECLPROC(void, Py_SetPythonHome, (wchar_t *));
EXTDECLPROC(void, Py_SetPath, (wchar_t *));  /* new in Python 3 */
EXTDECLPROC(int, PySys_SetArgvEx, (int, wchar_t **, int));
EXTDECLPROC(PyObject *, PyImport_ImportModule, (const char *));
EXTDECLPROC(PyObject *, PyObject_GetAttrString, (PyObject *, const char *));

// in python3.0~3.4, it is _Py_char2wchar, but renamed to Py_DecodeLocale in python3.5.  WTF.
//EXTDECLPROC(wchar_t *, _Py_char2wchar, (char *, size_t *));

//EXTDECLPROC(PyObject*, PyUnicode_FromWideChar, (const wchar_t*, size_t size	));

EXTDECLPROC(PyObject *, Py_BuildValue, (char *, ...));

EXTDECLPROC(void, PyErr_Clear, (void));
EXTDECLPROC(PyObject *, PyErr_Occurred, (void));
EXTDECLPROC(void, PyErr_Print, (void));

EXTDECLPROC(PyObject *, PyObject_Call, (PyObject *callable_object, PyObject *args, PyObject *kw));
EXTDECLPROC(int, PyArg_Parse, (PyObject *, const char *, ...));

EXTDECLPROC(PyObject *, PyObject_CallFunction, (PyObject *, char *, ...));
EXTDECLPROC(PyObject *, PyModule_GetDict, (PyObject *));
EXTDECLPROC(PyObject *, PyDict_GetItemString, (PyObject *, char *));
EXTDECLPROC(int, PyDict_SetItemString, (PyObject *dp, const char *key, PyObject *item));
EXTDECLPROC(long, PyLong_AsLong, (PyObject *));
EXTDECLPROC(PyObject *, PyLong_FromLong, (long));
EXTDECLPROC(PyObject *, PyLong_FromUnsignedLong, (unsigned long));
EXTDECLPROC(PyObject *, PyLong_FromUnsignedLongLong, (unsigned PY_LONG_LONG));
EXTDECLPROC(PyObject *, PyBytes_FromString, (const char *));
EXTDECLPROC(PyObject *, PyBytes_FromStringAndSize, (const char *, Py_ssize_t));
EXTDECLPROC(PyObject *, PyUnicode_FromString, (const char *));
EXTDECLPROC(PyObject *, PyBool_FromLong, (long));


EXTDECLPROC(int, PyImport_ExtendInittab, (struct _inittab *newtab));
EXTDECLPROC(PyObject *, PyModule_Create2, (struct PyModuleDef*, int apiver));
EXTDECLPROC(int, PyArg_ParseTuple, (PyObject *, const char *, ...));
EXTDECLPROC(PyObject *, PyTuple_Pack, (Py_ssize_t, ...));


#else // for linux, link to static python lib.

#define pylib_Py_FrozenFlag Py_FrozenFlag
#define pylib_Py_NoSiteFlag Py_NoSiteFlag
#define pylib_Py_OptimizeFlag Py_OptimizeFlag
#define pylib_Py_FileSystemDefaultEncoding Py_FileSystemDefaultEncoding
#define pylib_Py_VerboseFlag Py_VerboseFlag
#define pylib_Py_IgnoreEnvironmentFlag Py_IgnoreEnvironmentFlag
#define pylib_Py_DontWriteBytecodeFlag Py_DontWriteBytecodeFlag
#define pylib_Py_NoUserSiteDirectory Py_NoUserSiteDirectory
#define pylib_Py_Initialize Py_Initialize
#define pylib_Py_Finalize Py_Finalize
#define pylib_Py_IncRef Py_IncRef
#define pylib_Py_DecRef Py_DecRef
#define pylib_Py_SetProgramName Py_SetProgramName
#define pylib_Py_SetPythonHome Py_SetPythonHome
#define pylib_Py_SetPath Py_SetPath
#define pylib_PySys_SetArgvEx PySys_SetArgvEx
#define pylib_PyImport_ImportModule PyImport_ImportModule
#define pylib_PyObject_GetAttrString PyObject_GetAttrString
#define pylib_Py_BuildValue Py_BuildValue
#define pylib_PyErr_Clear PyErr_Clear
#define pylib_PyErr_Occurred PyErr_Occurred
#define pylib_PyErr_Print PyErr_Print
#define pylib_PyObject_Call PyObject_Call
#define pylib_PyArg_Parse PyArg_Parse
#define pylib_PyObject_CallFunction PyObject_CallFunction
#define pylib_PyModule_GetDict PyModule_GetDict
#define pylib_PyDict_GetItemString PyDict_GetItemString
#define pylib_PyDict_SetItemString PyDict_SetItemString
#define pylib_PyLong_AsLong PyLong_AsLong
#define pylib_PyLong_FromLong PyLong_FromLong
#define pylib_PyLong_FromUnsignedLong PyLong_FromUnsignedLong
#define pylib_PyLong_FromUnsignedLongLong PyLong_FromUnsignedLongLong
#define pylib_PyBytes_FromString PyBytes_FromString
#define pylib_PyBytes_FromStringAndSize PyBytes_FromStringAndSize
#define pylib_PyUnicode_FromString PyUnicode_FromString
#define pylib_PyBool_FromLong PyBool_FromLong
#define pylib_PyImport_ExtendInittab PyImport_ExtendInittab
#define pylib_PyModule_Create2 PyModule_Create2
#define pylib_PyArg_ParseTuple PyArg_ParseTuple
#define pylib_PyTuple_Pack PyTuple_Pack

#define pylib_Py_IncRef Py_IncRef
#define pylib_Py_DecRef Py_DecRef
#define pylib_PyBool_FromLong PyBool_FromLong
#define pylib_PyBool_FromLong PyBool_FromLong

#endif

#define PYLIB_XINCREF(o)    pylib_Py_IncRef(o)
#define PYLIB_XDECREF(o)    pylib_Py_DecRef(o)
#define PYLIB_DECREF(o)     PYLIB_XDECREF(o)
#define PYLIB_INCREF(o)     PYLIB_XINCREF(o)

#define PYLIB_RETURN_TRUE return pylib_PyBool_FromLong(1)
#define PYLIB_RETURN_FALSE return pylib_PyBool_FromLong(0)


typedef int PYS_BOOL;
#define PYS_TRUE      1
#define PYS_FALSE     0


//=========================================================================
// PyShell API
//=========================================================================
typedef unsigned long PYS_RET;
#define PYSR_OK			0x00000000
#define PYSR_FAILED		0x00000005

#if 0
#ifdef EX_OS_WIN32
#	ifdef EX_DEBUG
#		if defined(_M_X64)
#			pragma comment(lib, "pys_64d.lib")
#		elif defined(_M_IX86)
#			pragma comment(lib, "pys_32d.lib")
#		else
#			error unsupport platform.
#		endif
#	else
#		if defined(_M_X64)
#			pragma comment(lib, "pys_64.lib")
#		elif defined(_M_IX86)
#			pragma comment(lib, "pys_32.lib")
#		else
#			error unsupport platform.
#		endif
#	endif
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

	typedef void* PYS_HANDLE;

	// ����һ��PyShell��������в�������Ӧ�˾�����У�һ�����̽���һ�������
	PYS_HANDLE pys_create(void);
	// ����һ��PyShell���
	void pys_destroy(PYS_HANDLE* pysh);

	// ʹ��ָ��������ʱ·�����г�ʼ��������ʱ·���а���pythonXX.dll/python.zip/modules�ȵȣ�
	PYS_BOOL pys_init_runtime(PYS_HANDLE pysh, const wchar_t* exec_file, const wchar_t* runtime_path);

	// ����python������·�����ɶ�ε��ý���׷�ӣ���ʡ�ԣ�
	PYS_BOOL pys_add_search_path(PYS_HANDLE pysh, const wchar_t* path);

	// ����python����ʱ�������в�������ʡ�ԣ�
	void pys_set_argv(PYS_HANDLE pysh, int argc, wchar_t** argv);
	// ׷��python����ʱ�������в�������ʡ�ԣ�
	void pys_add_arg(PYS_HANDLE pysh, const wchar_t* arg);
	// ����python���������ƣ���ʡ�ԣ�Ĭ��Ϊ��ǰ��ִ�г����ļ����ľ���·����
	void pys_set_program(PYS_HANDLE pysh, const wchar_t* program_name);

	// ������ڽű��ļ�����������һ��.py�ļ���Ҳ������һ��.zip�ļ�
	void pys_set_startup_file(PYS_HANDLE pysh, const wchar_t* filename);

	// ��������ģ��������ں�������func_nameΪNULLʱĬ��ִ��ָ��ģ���е�main����
	// ����������ʡ�ԣ�Ĭ������£�
	// ���startup_file��һ��.py�ļ�����Ĭ��module_name����.py�ļ����ļ�������
	// ���startup_file��һ��.zip�ļ�����Ĭ��module_name��`pysmain`��
	void pys_set_bootstrap_module(PYS_HANDLE pysh, const char* module_name, const char* func_name);

	// ��ʼ��ģ��ĺ���ԭ��
	typedef PyObject* (*pys_init_module_func)(void);

	typedef struct PYS_BUILTIN_FUNC
	{
		const char* py_func_name;	// Python�е���ʱʹ�õĺ�����
		PyCFunction c_func_addr;	// ��Ӧ��C�ĺ���
		PYS_BOOL have_args;			// �˺����Ƿ���Ҫ����
		const char* py_func_desc;	// �˺������ĵ�ע�ͣ�����ΪNULL��
	}PYS_BUILTIN_FUNC;

	typedef enum PYS_CONST_TYPE
	{
		PYS_CONST_BOOL,		// Python�еõ� True/False ��ֵ
		PYS_CONST_LONG,		// Python�еõ�һ������
		PYS_CONST_STRING,	// Python�еõ�һ���ַ���
		PYS_CONST_BYTES		// Python�еõ�һ��Bytes��������
	}PYS_CONST_TYPE;

	typedef struct PYS_BUILTIN_CONST
	{
		char* py_const_name;	// Python�е���ʱʹ�õı�����
		PYS_CONST_TYPE type;	// ��������
		size_t size;			// �������ݵĳ���
		void* buffer;			// �������ݵ�����
	}PYS_BUILTIN_CONST;

	// ����һ���ڽ�ģ�飬���У����û�к�����������ô��Ӧ��funcs/consts����ΪNULL��
	// �ɶ�ε��ñ���������������ڽ�ģ�顣�����ε���ʱʹ����ͬ��ģ�����������ͳ�����׷�ӵ���ģ����
	// ͬһ��ģ���У��������ͳ����������ظ���������ͨ����Сд���֣�
	PYS_BOOL pys_add_builtin_module(PYS_HANDLE pysh, const char* module_name, pys_init_module_func init_func);

	PyObject* pys_create_module(const char* module_name, PYS_BUILTIN_FUNC* funcs);
	void pys_builtin_const_bool(PyObject* mod, const char* name, PYS_BOOL val);
	void pys_builtin_const_long(PyObject* mod, const char* name, long val);
	void pys_builtin_const_utf8(PyObject* mod, const char* name, const char* val);		// val ������utf8������ַ���
	void pys_builtin_const_wcs(PyObject* mod, const char* name, const wchar_t* val);
	void pys_builtin_const_bin(PyObject* mod, const char* name, const ex_u8* val, size_t size);

	// ����python������
	int pys_run(PYS_HANDLE pysh);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class PysHandleHolder
{
public:
	PysHandleHolder(PYS_HANDLE h) :m_handle(h) { }
	~PysHandleHolder() { pys_destroy(&m_handle); }
private:
	PYS_HANDLE m_handle;
};
#endif

#endif // __PYS_H__
