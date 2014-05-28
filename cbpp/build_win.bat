
@echo off

setlocal

::
:: Search for the location of Visual Studio
::
if DEFINED VS110COMNTOOLS (
	set "VS_TOOLS_DIR=%VS110COMNTOOLS%"
) else if DEFINED VS100COMNTOOLS (
	set "VS_TOOLS_DIR=%VS100COMNTOOLS%"
) else if DEFINED VS90COMNTOOLS (
	set "VS_TOOLS_DIR=%VS90COMNTOOLS%"
) else if DEFINED VS80COMNTOOLS (
	set "VS_TOOLS_DIR=%VS80COMNTOOLS%"
) else (
	echo Microsoft Visual Studio not found
	exit
)


::
:: Apply environment necessary to use cl.exe
::
set VC_DIR=%VS_TOOLS_DIR%..\..\VC
call "%VC_DIR%\vcvarsall.bat"

::
:: Search for the windows SDK
::
set KEY_NAME="HKLM\SOFTWARE\Microsoft\Microsoft SDKs\Windows"
set VALUE_NAME=CurrentInstallFolder
FOR /F "usebackq skip=2 tokens=1,2*" %%A IN (`REG QUERY %KEY_NAME% /v %VALUE_NAME% 2^>nul`) DO (
	set "ValueName=%%A"
	set "ValueType=%%B"
	set WINDOWS_SDK_DIR=%%C
)
if not defined WINDOWS_SDK_DIR (
	echo %KEY_NAME%\%VALUE_NAME% not found.
	exit
)


::
:: Unconditional compile/link of all files
::
set SRC=%~dp0/src
set OUT=%~dp0/bin
cl.exe %SRC%/cbpp.cpp /EHsc /nologo /Fo%OUT%/cbpp.obj /c
cl.exe %SRC%/ComputeParser.c /EHsc /nologo /Fo%OUT%/ComputeParser.obj /c
link.exe /LIBPATH:"%WINDOWS_SDK_DIR%lib" /OUT:%~dp0/bin/cbpp.exe %OUT%/cbpp %OUT%/ComputeParser
