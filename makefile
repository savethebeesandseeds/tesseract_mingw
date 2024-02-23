ifeq ($(MAKEFLAGS),)  # Check if MAKEFLAGS already set from the environment
   MAKEFLAGS := -j$(nproc)
endif

test64:
	x86_64-w64-mingw32-g++ test.cpp -o test.exe \
		-I/src/build/x86_64-w64-mingw32/include/ -I./include \
		-L/src/build/x86_64-w64-mingw32/lib \
		-l:libtesseract53.a -l:libleptonica-1.84.1.a \
		-l:libpng16.a -l:libjpeg.a \
		-lzlibstatic -lws2_32 -static-libgcc -static-libstdc++ -std=c++17
