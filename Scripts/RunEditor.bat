@echo off
set UE_ROOT=D:\EPIC\UE_5.8
start "" "%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" "%~dp0..\Platformer.uproject"
