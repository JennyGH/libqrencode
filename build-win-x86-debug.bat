@ECHO OFF
SETLOCAL
MKDIR build-win-x86-debug
CD build-win-x86-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
CD ..
ENDLOCAL
PAUSE