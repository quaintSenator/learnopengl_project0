#pragma once

class ImageLoader
{

    static unsigned char* readImage(char const* path,int* width,int* height,int* nrComponents);
    static void ImageFree(unsigned char*);

    static int generateAndReadImage(char const* path);
};
