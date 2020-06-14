#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <corgi/resources/image.h>
#include <corgi/filesystem/FileSystem.h>
#include <string>
#include "stb_image.h"

#include <fstream>
#include <vector>

//#include <font.h>

#include <corgi/string/String.h>

#include <corgi/ecs/Entity.h>

namespace Editor
{
    namespace Utils
    {
        static void WriteBinaryImage(std::ofstream& file, int x, int y,
            int channels, unsigned char* pixels)
        {
            file.write(reinterpret_cast<const char*>(&x), sizeof(x));
            file.write(reinterpret_cast<const char*>(&y), sizeof(y));
            file.write(reinterpret_cast<const char*>(&channels), 
                sizeof(channels));

            for (int i = 0; i < x*y*channels; ++i)
            {
                file << pixels[i];
            }
        }

        

        static corgi::Image* LoadImageForReal(const corgi::String& imageFile)
        {
            int x, y, channels;
            // Images are horizontal on OpenGL otherwise
            stbi_set_flip_vertically_on_load(true);

            stbi_uc* imageData = stbi_load(imageFile.c_str(), &x, &y, &channels
                , STBI_rgb_alpha);

            return new corgi::Image(x, y, channels, imageData);
        }

        //void WriteFont(const std::string& file,
        //    const std::string& outputFolder)
        //{
        //    //corgi::FontLoader font;
        //    //font.load(file.c_str());
        //    //
        //    //std::string filename = corgi::filesystem::filename(file);
        //    //filename = filename.substr(0, filename.length() - 4);
        //    //
        //    //corgi::filesystem::create_directory(outputFolder);
        //    //
        //    //std::ofstream fontImage(outputFolder + "/" + filename + ".dat", 
        //    //std::ofstream::trunc, std::ofstream::binary);
        //    //WriteBinaryImage(fontImage, font.width, font.height, 4,
        //    //font.image);
        //    //
        //    //std::ofstream fontAtlas(outputFolder+"/"+filename+"Atlas.dat",
        //    //std::ofstream::trunc, std::ofstream::binary);
        //    //fontAtlas.close();
        //    //
        //    ////fontAtlas.open(outputFolder + "/" + filename + "Atlas.dat",
        //    //std::ofstream::app, std::ofstream::binary);
        //    //    
        //    //std::string outputFile = outputFolder + "/" + filename + 
        //    //"Atlas.dat";
        //    //
        //    //FILE* fileOld; 
        //    //fileOld = fopen(outputFile.c_str(), "wb");  // r for read, b 
        //    //for binary
        //    //fwrite(font.characters, sizeof(corgi::character_info), 256,
        //    //fileOld); // read 10 bytes to our buffer
        //    //fclose(fileOld);
        //    //corgi::character_info infos[256];
        //    //
        //    //FILE* fileRead;
        //    //fileRead = fopen(outputFile.c_str(), "rb");  // r for read,
        //    //b for binary
        //    //fread(infos, sizeof(corgi::character_info), 256, fileRead); 
        //    // read 10 bytes to our buffer
        //    //fclose(fileRead);
        //}

        static std::vector<char*> ConvertString(std::vector<corgi::String>& strings)
        {
            std::vector<char*> result;
            for (auto& string : strings)
            {
                result.push_back(&string[0]);
            }
            return result;
        }

        static void WriteImage(const corgi::String& image_file,
            const corgi::String& output_folder)
        {
            int x, y, channels;
            
            // Images are horizontal on OpenGL otherwise
            //stbi_set_flip_vertically_on_load(true);
            
            stbi_uc* image = stbi_load(image_file.c_str(), &x, &y, &channels,
                STBI_rgb_alpha);
			std::string f = image_file.c_str();
			std::string ff = output_folder.c_str();
            std::string filename = corgi::filesystem::filename(f);

            filename = filename.substr(0, filename.size() - 4);
            
            corgi::filesystem::create_directory(ff);

			std::string fff = (ff+ "/" + filename + ".dat");
            const char* s = fff.c_str();
            std::ofstream file(s, std::ofstream::trunc | std::ofstream::binary);
            
            WriteBinaryImage(file, x, y, channels, (unsigned char*)image);
            
            stbi_image_free(image);
        }
    }
}