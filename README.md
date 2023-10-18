# intro
xx2d web version game engine ( also support desktop development )

# language
c++20 with coroutine


# projects
web: ( all in ./CMakeLists.txt )

s*

desktop: ( single project )

d*


# env
https://emscripten.org/index.html
emsdk with install & activate latest version

system env add ( copy from emsdk_env.bat ):

EMSDK          ???????????\emsdk
EMSDK_NODE     ???????????\emsdk\node\??????????????64bit\bin\node.exe
EMSDK_PYTHON   ???????????\emsdk\python\?????-nuget_64bit\python.exe
JAVA_HOME      ???????????\emsdk\java\?????_64bit

system env path add:

???????????\emsdk
???????????\emsdk\upstream\emscripten
???????????\emsdk\upstream\bin
???????????\emsdk\node\?????????64bit\bin

# ide1
clion

path add for ninja64.exe:
C:\Program Files\JetBrains\CLion 20????????????????????????????\bin\ninja\win\x64

path add for mingw32-make.exe:
C:\Program Files\JetBrains\CLion 20????????????????????????????\bin\mingw\bin

config ( for every cmake project ):
File -- Settings -- Build, Execution, Deployment -- CMake -- Debug ( default mingw ) CMake options:

-DCMAKE_TOOLCHAIN_FILE=???????????\emsdk\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake

more: ( ignore vs build dir )
mouse right click on "dir", Mark Directory As -- Excluded

# build
click clion "hammer" icon

# browse
clion terminal window:
cd cmake-build-????????
emrun gemini.html

# ide2 ( better for c++ because clion lag & keep use so many cpu )
vs22 with llvm installed

use cmake gui fill -T        ClangCL         generate .sln & open

after open .sln,  delete    CMakeCCompilerId.c  &   CMakeCXXCompilerId.cpp   from project

# build & browse
in vs22, hot key : ctrl + `  open developer powershells:
mkdir ?????
cd ?????
emcmake cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo                      = Debug | Release | RelWithDebInfo
emmake make -j24
emrun gemini.html

# online view:
https://denghe.github.io/xx2dw_shooter/


# others
aseprite build:
download aseprite source code from https://github.com/aseprite/aseprite/releases
download skia from https://github.com/aseprite/skia/releases
extract zips
run "x64 Native Tools Command Prompt for VS 2022"
cd C:/Codes/Aseprite
mkdir build
cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DLAF_BACKEND=skia -DSKIA_DIR=C:/Codes/Skia-Windows-Release-x64 -DSKIA_LIBRARY_DIR=C:/Codes/Skia-Windows-Release-x64/out/Release-x64 ..
ninja aseprite