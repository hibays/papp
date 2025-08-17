@echo off
setlocal enabledelayedexpansion

:: ---------- 1. 查找 Python 并检测版本 ----------
set "PY_CMD="
for %%c in (py python python3) do (
    %%c -c "import sys; assert sys.version_info>=(3,9)" >nul 2>&1
    if !errorlevel! equ 0 (
        set "PY_CMD=%%c"
        goto :found
    )
)
echo [ERROR] Python 3.9+ not found in PATH.
echo        Please install Python 3.9 or later and add it to PATH, then run again.
pause
exit /b 1

:found
echo [INFO] Using Python: %PY_CMD%

:: ---------- 2. 虚拟环境路径 ----------
set "VENV_DIR=%~dp0.venv"

:: ---------- 3. 创建或复用虚拟环境 ----------
if exist "%VENV_DIR%\Scripts\activate.bat" (
    echo [INFO] Reusing existing virtual environment.
) else (
    echo [INFO] Creating virtual environment ...
    %PY_CMD% -m venv "%VENV_DIR%"
    if errorlevel 1 (
        echo [ERROR] Failed to create venv
        pause & exit /b 1
    )
)

:: ---------- 4. 激活 & 安装依赖 ----------
call "%VENV_DIR%\Scripts\activate.bat"
python "%~dp0check_require.py" >nul 2>&1
if !errorlevel! equ 0 (
    echo [INFO] Requirements ok.
    goto :run_main
) else (
    echo [INFO] Requirements checked failed.
    echo [INFO] Trying to install requirements ...
    python -m pip install --upgrade pip >nul
    python -m pip install -r "%~dp0requirements.txt"
    if !errorlevel! equ 0 (
        echo [INFO] Requirements installed.
        goto :run_main
    ) else (
        echo [ERROR] pip install failed.
        echo [INFO] Please install requirements manually.
        pause & exit /b 1
    )
)

:: ---------- 5. 运行主脚本 ----------
:run_main
echo [INFO] Running create_papp.py ...
python -m streamlit run "%~dp0create_papp.py"
