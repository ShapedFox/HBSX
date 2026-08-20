@echo off

make
if "%~1" == "XEMU" (
	"%XEMU_EXEC%"
)
