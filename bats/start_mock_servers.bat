@echo off
REM Запуск трёх серверов с разными IP в отдельных окнах с заголовками

start "Server 127.0.0.1" cmd /k "mock_server.exe 127.0.0.1"
start "Server 127.0.0.2" cmd /k "mock_server.exe 127.0.0.2"
start "Server 127.0.0.3" cmd /k "mock_server.exe 127.0.0.3"

pause

