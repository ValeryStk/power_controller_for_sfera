@echo off
echo clear icon cache...

:: Остановка проводника
taskkill /f /im explorer.exe

:: Удаление кеша иконок
del /A /Q "%localappdata%\IconCache.db"
del /A /F /Q "%localappdata%\Microsoft\Windows\Explorer\iconcache*"

:: Перезапуск проводника
start explorer.exe

echo icon cache is cleared.
pause
