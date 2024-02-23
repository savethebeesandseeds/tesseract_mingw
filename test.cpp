// compile with: i686-w64-mingw32-g++ test.cpp -o test.exe -I/usr/local/include/ -I/external/ -I./include -l:/external/tesseract/lib/libtesseract_4_1_1_all.a -static
// i686-w64-mingw32-g++ test.cpp -o test.exe -I/usr/local/include/ -I/external/ -I./include -L/external/tesseract/lib/ -l:libtesseract_4_1_1_all.a -static



// compile with: i686-w64-mingw32-g++ test.cpp -o test.exe -I/usr/local/include/ -I/external/ -I./include -L/external/tesseract/lib/ -L/usr/local/lib/ -L/external/mingw32-bin -lws2_32 -lpng -ltiff -ljpeg -lstdc++ -llept -ltesseract41 -static





// i686-w64-mingw32-g++ test.cpp -o test.exe -I/usr/local/include/ -I/external/ -I./include -L/external/tesseract/lib -ltesseract41 -llept -lws2_32 -lstdc++ -std=c++17

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>

int main() {
    // Load the image
    PIX *image = pixRead("test.png"); 

    // Create a Tesseract API instance
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    // Initialize Tesseract (English language in this example)
    if (api->Init("./resources/languages", "fra")) { 
        std::cerr << "Error initializing Tesseract!\n";
        return 1;
    }

    // Set the image for OCR
    api->SetImage(image);

    // Extract the text
    char* text = api->GetUTF8Text(); 
    std::cout << "OCR Result:\n" << text << std::endl;

    // Clean up
    api->End();
    delete [] text;
    pixDestroy(&image);

    return 0;
}
