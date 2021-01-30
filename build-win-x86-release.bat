@ECHO OFF
SETLOCAL
MKDIR build-win-x86-release
CD build-win-x86-release
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
CD ..
ENDLOCAL
PAUSE