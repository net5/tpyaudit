;=========================================================
;	tpyaudit-assist Run-time Package Installer Script.
;=========================================================
;
; 定义常量
; 如果不常更新的常量，或基于本文件常量推导出来的常量定义到main.nsh
; 安装函数，流程 定义到 setup.nsh
;
;==========================================================================;

!define COMPANY_NAME		"TPYSoft"

!define BASE_NAME			"tpyaudit-assist"
!define PRODUCT_NAME		"TpyAudit-Assist"
!define PRODUCT_WEBSITE 	"www.tianpuyun.com"

; 产品安装包GUID
!define PRODUCT_GUID			"{2FFD2BF7-24C0-4AC5-BF7B-B8C7420CF694_TP_ASSIST}"
!define PRODUCT_NAME_DESC		"TPYSoft-TpyAudit"
!define PRODUCT_NAME_DISPLAY	"TpyAudit by TPYSoft"

!define PRODUCT_VER			"3.1"
!define FILE_VER			"3.1.0.0"

; 包含文件名称 ，为了支持通配，都不加后缀
!define PFNAME_ASSIST			"tp_assist"

; 输出setup文件的名称不含文件后缀 （.exe），如果不定义，使用默认规则创建输出名称

; 指定输出文件名称
!define DEF_OUTPUT_RUNTIME_PACKAGE_NAME			"${BASE_NAME}-windows-${FILE_VER}"

; 支持的语言,如果支持多于一种语言，自动选择安装语言
;!define _SUPPORT_MUTI_LANGS_
; 如果支持多语言，设置默认首选语言
;!ifdef _SUPPORT_MUTI_LANGS_
;	!define _LANG				1033
;	!define _LANG_NAME			"English"
;!endif

; 如果没有设置 _SUPPORT_MUTI_LANGS_，以下语言只能选择一种
!define _SUPPORT_LANG_SIMPCHINESE
;!define _SUPPORT_LANG_ENGLISH
;!define _SUPPORT_LANG_TRADCHINESE


!include "main.nsh"
!include "setup.nsh"

; This text show at the bottom of the installer UI. Replace the "Nullsoft Install System vXXX".
BrandingText ${PRODUCT_WEBSITE}

;EOF
