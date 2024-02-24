::-DCURL_INCLUDE_DIR:PATH=C:\curl-8.4.0_6-win64-mingw\include -DCURL_LIBRARY:PATH=C:\curl-8.4.0_6-win64-mingw\lib\libcurl.a -DCURL_DIR:PATH=C:\curl-8.4.0_6-win64-mingw -DCURL_LIBRARIES:PATH=C:\curl-8.4.0_6-win64-mingw\lib\libcurl.a
cmake -G "Ninja" -B "./build" -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc -DCURL_INCLUDE_DIR=curl
pause