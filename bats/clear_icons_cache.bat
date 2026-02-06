@echo off
echo Close Explorer...
taskkill /f /im explorer.exe

ie4uinit.exe -show  (очищает кэш дополнительно) [web:1]
attrib -h -s "%localappdata%\IconCache.db"
del /f /q "%localappdata%\IconCache.db"
del /f /q "%localappdata%\Microsoft\Windows\Explorer\iconcache*"
del /f /q "%localappdata%\Microsoft\Windows\Explorer\thumbcache*"
start explorer.exe

echo Run explorer again...
start explorer.exe

echo icon cache is cleared!
pause
