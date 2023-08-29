#include "ImageLoader.h"
#include "../main.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned char* ImageLoader::readImage(char const* path, int* width, int* height, int* nrComponents)
{
    return stbi_load(path,width,height,nrComponents,0);
}
void ImageLoader::ImageFree(unsigned char* data)
{
    stbi_image_free(data);
}
