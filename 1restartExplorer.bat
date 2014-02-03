@echo off
taskkill /F /IM explorer.exe
regsvr32 /s /u %1
start C:\Windows\explorer.exe