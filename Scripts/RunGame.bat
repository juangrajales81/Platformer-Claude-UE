@echo off
rem Lanza el juego directamente (sin abrir el editor).
set UE_ROOT=D:\EPIC\UE_5.8
start "" "%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" "%~dp0..\Platformer.uproject" -game -windowed -ResX=1280 -ResY=720 -log
