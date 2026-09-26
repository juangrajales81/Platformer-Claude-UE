@echo off
rem Compila el editor del proyecto (requiere MSVC + Windows SDK).
set UE_ROOT=D:\EPIC\UE_5.8
call "%UE_ROOT%\Engine\Build\BatchFiles\Build.bat" PlatformerEditor Win64 Development -Project="%~dp0..\Platformer.uproject" -WaitMutex
