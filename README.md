## Note

These are modern instructions on how to Cross-Compile Tesseract in Linux to run on Windows. 

Instructions and files are for 64 bits, but if carefull you might be able to transform these instructions to do the same for 32 bits.

I don't distribute the builds to respect the original libraries, you'll see these woud've been builds with reduced functionality. Since these are Dockerized instructions you can be quite sure they can be replicated. Until the Github sources are updated and these instructions become out of date. 

## Known issues

Not all features are supported: 

    ENABLE_ZLIB=ON
    ENABLE_PNG=ON
    ENABLE_GIF=ON
    ENABLE_JPEG=ON
    ENABLE_WEBP=ON
    ENABLE_OPENJPEG=ON
    USE_SYSTEM_ICU=ON       (there is not full Unicode support)
    GRAPHICS_DISABLED=ON
    BUILD_TRAINING_TOOLS=OFF(Training tools introduce many deps)
    BUILD_TESTS=OFF         (Didn't build the test)
    DISABLE_CURL=ON         (Avoided CURL deps)
    DISABLE_ARCHIVE=ON      (Avoided Archive deps)
    DISABLE_TIFF=ON         (There be warnings or missing tiff)
    

I see some runtime errors when running the library: 

    Error in pixReadMemTiff: function not present
    Error in pixReadMem: tiff: no pix returned
    Error in pixaGenerateFontFromString: pix not made
    Error in bmfCreate: font pixa not made

I try building TIFF to solve it, but i cound't. 

    Anyway the results works, there is an example makefile that show you how to compile the static tesseract to run in Windows.

## Create the build docker enviroment

Open CMD and type these commands:

    cd /path/to/project

    docker pull debian:11

    docker run -it --name=tesseract_mingw -v .:/src debian:11

    docker exec -it tesseract_mingw /bin/bash

## Install linux dependencies

Now you are no longer in Windows, but inside a Linux Docker. Type these commands.

    apt update && apt install mingw-w64 make cmake g++ pkg-config git --no-install-recommends -y

    apt install --reinstall ca-certificates --no-install-recommends -y

    mkdir /external

## Download mingw-std-threads (as MinGW does not support linux threads)

    cd /external

    git clone https://github.com/meganz/mingw-std-threads

    cp /src/resources/generate-std-like-headers.sh /external/mingw-std-threads/generate-std-like-headers.sh

    cd /external/mingw-std-threads

    chmod +x generate-std-like-headers.sh

    ./generate-std-like-headers.sh

## Download and install zlib

    cd /external

    git clone https://github.com/madler/zlib

    cp /src/resources/mingw64_toolchain.cmake /external/zlib

    mkdir /external/zlib/build && cd /external/zlib/build

    cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_TOOLCHAIN_FILE=/external/zlib/mingw64_toolchain.cmake -G"Unix Makefiles" /external/zlib -DCMAKE_INSTALL_PREFIX=/src/build/x86_64-w64-mingw32

    make -j$(nproc) && make install

## Install local static ICU (not cross compiled)

apt install libicu-dev --no-install-recommends -y


## Download and install libjpeg from source (optional)

    cd /external

    git clone https://github.com/winlibs/libjpeg

    cp /src/resources/mingw64_toolchain.cmake /external/libjpeg

    export ZLIB_DIR=/src/build/x86_64-w64-mingw32

    mkdir /external/libjpeg/build && cd /external/libjpeg/build

    cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_TOOLCHAIN_FILE=/external/libjpeg/mingw64_toolchain.cmake -G"Unix Makefiles" /external/libjpeg -DCMAKE_INSTALL_PREFIX=/src/build/x86_64-w64-mingw32

    make -j$(nproc) && make install

## Download and install libpng from source 

    cd /external

    git clone https://github.com/libpng/libpng

    cp /src/resources/mingw64_toolchain.cmake /external/libpng

    mkdir /external/libpng/build && cd /external/libpng/build

    export ZLIB_DIR=/src/build/x86_64-w64-mingw32

    cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_TOOLCHAIN_FILE=/external/libpng/mingw64_toolchain.cmake -G"Unix Makefiles" /external/libpng -DCMAKE_INSTALL_PREFIX=/src/build/x86_64-w64-mingw32 -DZLIB_LIBRARY=/src/build/x86_64-w64-mingw32/lib/libzlibstatic.a -DZLIB_INCLUDE_DIR=/src/build/x86_64-w64-mingw32/include

    make -j$(nproc) && make install

## Download and build Leptonica from source

    cd /external

    git clone https://github.com/DanBloomberg/leptonica.git

    cp /src/resources/mingw64_toolchain.cmake /external/leptonica

    mkdir /external/leptonica/build && cd /external/leptonica/build

    export ZLIB_DIR=/src/build/x86_64-w64-mingw32

    cmake \
    -DSW_BUILD=OFF -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_FIND_LIBRARY_SUFFIXES=".a" \
    -DCMAKE_INSTALL_PREFIX=/src/build/x86_64-w64-mingw32 \
    -DCMAKE_TOOLCHAIN_FILE=/external/leptonica/mingw64_toolchain.cmake \
    -DZLIB_INCLUDE_DIR=/src/build/x86_64-w64-mingw32/include \
    -DZLIB_LIBRARY=/src/build/x86_64-w64-mingw32/lib/libzlibstatic.a\
    -DJPEG_INCLUDE_DIR=/src/build/x86_64-w64-mingw32/include/ \
    -DJPEG_LIBRARY=/src/build/x86_64-w64-mingw32/lib/libjpeg.a \
    -DPNG_PNG_INCLUDE_DIR=/src/build/x86_64-w64-mingw32/include \
    -DPNG_LIBRARY=/src/build/x86_64-w64-mingw32/lib/libpng16.a \
    -DTIFF_INCLUDE_DIR=/src/build/x86_64-w64-mingw32/include/ \
    -DTIFF_LIBRARY=/src/build/x86_64-w64-mingw32/lib/libtiffxx.a \
    -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++" \
    -DENABLE_ZLIB=ON \
    -DENABLE_PNG=ON \
    -DENABLE_GIF=ON \
    -DENABLE_JPEG=ON \
    -DENABLE_TIFF=OFF \
    -DENABLE_WEBP=ON \
    -DENABLE_OPENJPEG=ON \
    -G"Unix Makefiles" \
    /external/leptonica

    make -j$(nproc) && make install

## Download and build tesseract source

    cd /external

    git clone https://github.com/tesseract-ocr/tesseract.git

    cp /src/resources/mingw64_toolchain.cmake /external/tesseract

    ln -s /usr/x86_64-w64-mingw32/lib/libws2_32.a /usr/x86_64-w64-mingw32/lib/libWs2_32.a

    mkdir /external/tesseract/build && cd /external/tesseract/build

    export ZLIB_DIR=/src/build/x86_64-w64-mingw32

    cmake \
    -G"Unix Makefiles" \
    -DCMAKE_CXX_STANDARD=17 \
    -DBUILD_SHARED_LIBS=OFF \
    -DSW_BUILD=OFF \
    -DCMAKE_INSTALL_PREFIX=/src/build/x86_64-w64-mingw32 \
    -DCMAKE_TOOLCHAIN_FILE=/external/tesseract/mingw64_toolchain.cmake \
    -DLeptonica_DIR=/src/build/x86_64-w64-mingw32/cmake/leptonica \
    -DZLIB_INCLUDE_DIR=/src/build/x86_64-w64-mingw32/include \
    -DZLIB_LIBRARY=/src/build/x86_64-w64-mingw32/lib/libzlibstatic.a\
    -DTIFF_INCLUDE_DIR=/src/build/x86_64-w64-mingw32/include/ \
    -DTIFF_LIBRARY=/src/build/x86_64-w64-mingw32/lib/libtiffxx.a \
    -DCMAKE_EXE_LINKER_FLAGS="-L/src/build/x86_64-w64-mingw32/lib/ -lws2_32 -static-libgcc -static-libstdc++" \
    -DCMAKE_PREFIX_PATH="/src/build/x86_64-w64-mingw32" \
    -DUSE_SYSTEM_ICU=ON \
    -DGRAPHICS_DISABLED=ON \
    -DBUILD_TRAINING_TOOLS=OFF \
    -DBUILD_TESTS=OFF \
    -DDISABLE_CURL=ON \
    -DDISABLE_ARCHIVE=ON \
    -DDISABLE_TIFF=ON \
    /external/tesseract
    make -j$(nproc) && make install

## Download other tessdata languages if needed

You can get better perfomance if you specify the correct language, do that by downloading the files here https://tesseract-ocr.github.io/tessdoc/Data-Files.html and put it into /resources/languages folder

## Build the test code 

    cd /src/

    make test64

Finaly you will see an static executable .exe that you can use in Windows.

It reads the text in the test image and prints out the result. 


<!-- ## Download and install ICU -->

<!-- Be aware this repository is very large and is hard to clone. 

> cd /external

> git clone --depth 1 https://github.com/unicode-org/icu

> apt update && apt install python --no-install-recommends -y

Here it's required first to build on the Linux host

> mkdir /external/icu/icu4c/source/build_Linux64 && cd /external/icu/icu4c/source/build_Linux64

> sh /external/icu/icu4c/source/runConfigureICU Linux \
    --enable-debug \
    --with-library-bits=64 \
    --with-data-packaging=static \
    --enable-static \
    LDFLAGS="-L/src/build/x86_64-w64-mingw32/lib" \
    CFLAGS="-std=c11" \
    CXXFLAGS="-std=c++17"
    
> make -j$(nproc) && make install -->

<!-- These symbolic links are quired to solve a bug in the library: 

> ln -s /external/icu/icu4c/source/build_Linux64/lib/libicudata.a /external/icu/icu4c/source/build_Linux64/lib/libicudt.a

> ln -s /external/icu/icu4c/source/build_Linux64/lib/libicui18n.a /external/icu/icu4c/source/build_Linux64/lib/libicuin.a -->

<!-- and then build for windows and cross-compile

> mkdir /external/icu/icu4c/source/build_Win64 && cd /external/icu/icu4c/source/build_Win64

> sh /external/icu/icu4c/source/configure \
    --enable-debug \
    --build=x86_64-linux-gnu \
    --host=x86_64-w64-mingw32 \
    --with-cross-build=/external/icu/icu4c/source/build_Linux64 \
    --prefix=/src/build/x86_64-w64-mingw32 \
    --enable-static --disable-shared \
    --with-data-packaging=static \
    CFLAGS="-std=c11 -lpthread -DWIN32_LEAN_AND_MEAN -I/src/build/x86_64-w64-mingw32 -I/src/build/x86_64-w64-mingw32/include -D_WIN32_WINNT=0x0A00" \
    CXXFLAGS="-std=c++17 -DWIN32_LEAN_AND_MEAN -I/src/build/x86_64-w64-mingw32 -I/src/build/x86_64-w64-mingw32/include -D_WIN32_WINNT=0x0A00" \
    LDFLAGS="-L/external/icu/icu4c/source/build_Linux64/lib/ -L/src/build/x86_64-w64-mingw32/lib -L/usr/share/mingw-w64/include/"

> make -j$(nproc) && make install -->

<!-- > apt remove python -y -->

<!-- -l:libsicudt.a -l:libsicuin.a -l:libsicuuc.a -l:libsicuio.a -->

<!-- ## Download and install xz from source 

xz provides lzma support needed by TIFF library.

> cd /external

> git clone https://github.com/tukaani-project/xz

> cp /src/resources/mingw64_toolchain.cmake /external/xz

> mkdir /external/xz/build && cd /external/xz/build

> cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_TOOLCHAIN_FILE=/external/xz/mingw64_toolchain.cmake -G"Unix Makefiles" /external/xz -DCMAKE_INSTALL_PREFIX=/src/build/x86_64-w64-mingw32

> make -j$(nproc) && make install -->

<!-- ## Download and install libtiff from source 

> cd /external

> git clone https://github.com/libsdl-org/libtiff

> cp /src/resources/mingw64_toolchain.cmake /external/libtiff

> cd /external/libtiff/

(the build folder is already in the repo)

> cd /external/libtiff/build

> export ZLIB_DIR=/src/build/x86_64-w64-mingw32

> apt install python3 --no-install-recommends -y

> cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_TOOLCHAIN_FILE=/external/libtiff/mingw64_toolchain.cmake -G"Unix Makefiles" /external/libtiff -DCMAKE_INSTALL_PREFIX=/src/build/x86_64-w64-mingw32 -DZLIB_LIBRARY=/src/build/x86_64-w64-mingw32/lib/libzlibstatic.a -DZLIB_INCLUDE_DIR=/src/build/x86_64-w64-mingw32/include

> make -j$(nproc) && make install

> apt remove python3 -y -->