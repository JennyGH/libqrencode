@ECHO OFF
SETLOCAL
MKDIR build-win-x64-release
CD build-win-x64-release
cmake .. -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
CD ..
ENDLOCAL
PAUSE