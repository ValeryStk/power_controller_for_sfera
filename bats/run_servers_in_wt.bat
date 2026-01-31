@echo off
REM Запуск Windows Terminal с тремя панелями из текущей папки

wt ^
  new-tab --title "Server 127.0.0.1" -- "%cd%\mock_server.exe" 127.0.0.1 ^
  ; split-pane -H --title "Server 127.0.0.2" -- "%cd%\mock_server.exe" 127.0.0.2 ^
  ; split-pane -V --title "Server 127.0.0.3" -- "%cd%\mock_server.exe" 127.0.0.3
