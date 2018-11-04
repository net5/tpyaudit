;=========================================================
;	tpyaudit-assist Run-time Package Installer Script.
;=========================================================

; 名称缩写，用于默认注册表路径
!ifndef COMPANY_NAME
!define COMPANY_NAME		"TPYSoft"
!endif

!define COPYRIGHT			"Copyright (C) 2018, TPYSoft Tech. Co., Ltd. All Rights Reserved."
!define PRODUCT_DESC		"${PRODUCT_NAME_DESC}"
!define SRC_RC_PATH			".\rc"
!define SRC_APPS_PATH		".\apps"

; 指定生成的安装包文件名称
!define FILE_NAME_RUNTIMEPAGE		"${DEF_OUTPUT_RUNTIME_PACKAGE_NAME}.exe"

;注册表路径
!define TARGET_REG_ROOT		"Software\${COMPANY_NAME}"
!define TARGET_REG_BASE		"${TARGET_REG_ROOT}\${PRODUCT_NAME}"

;安装路径
!define TARGET_DIR_ROOT		"${COMPANY_NAME}"
!define TARGET_DIR_BASE		"${TARGET_DIR_ROOT}\${PRODUCT_NAME}"

;;开始菜单路径
!define START_MENU_ROOT				"$SMPROGRAMS\$(STR_STARTMENU_ROOT)"
!define START_MENU_BASE				"${START_MENU_ROOT}\$(STR_STARTMENU_BASE)"

;EOF
