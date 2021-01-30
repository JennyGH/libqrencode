@ECHO OFF
SETLOCAL
MKDIR build-win-x64-debug
CD build-win-x64-debug
cmake .. -A x64 -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
CD ..
ENDLOCAL
PAUSE