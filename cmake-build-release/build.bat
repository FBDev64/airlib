del /F /Q vdl_test.exe 
del /F /Q cmake-build-release/libVDL.dll 
del /F /Q cmake-build-release/libVDL.dll.a 
C:/Users/Adam/AppData/Local/Programs/CLion/bin/cmake/win/x64/bin/cmake.exe --build C:/Users/Adam/CLionProjects/vdl/cmake-build-release --target VDL -j 3 
C:/Users/Adam/AppData/Local/Programs/CLion/bin/cmake/win/x64/bin/cmake.exe --build C:/Users/Adam/CLionProjects/vdl/cmake-build-release --target vdl_test -j 3