@echo off
setlocal
cd /d "%~dp0"
where py >nul 2>nul
if not errorlevel 1 (
  start "" "http://localhost:8765/"
  py -3 -m http.server 8765
  goto :end
)
where python >nul 2>nul
if not errorlevel 1 (
  start "" "http://localhost:8765/"
  python -m http.server 8765
  goto :end
)
echo Python 3 is required to run the local browser test.
echo Install Python from https://www.python.org/downloads/
pause
:end
