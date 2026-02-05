@echo off
echo Close Explorer...
taskkill /f /im explorer.exe

echo Clear icons cache...
del /a /q "%localappdata%\IconCache.db"
del /a /f /q "%localappdata%\Microsoft\Windows\Explorer\iconcache*"

echo Run explorer again...
start explorer.exe

echo icon cache is cleared!
pause
