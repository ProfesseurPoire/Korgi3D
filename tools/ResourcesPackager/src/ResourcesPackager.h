#pragma once

#include <string>
#include <filesystem>

namespace corgi
{
    class ResourcePackager
    {
        public:

            ResourcePackager(const std::string& resource_folder) :
                resource_folder_(resource_folder)
            {

            }

            /*!
             * @brief   Returns the file's extension at @a filepath
             */
            static std::string extension(const std::string& filepath)
            {
                std::string str(filepath);
                std::string ext;
                bool start_ext = false;

                // Would actually be better to do this with 
                // a recursive iterator here btfw

                // TODO : It would be actually nice not to remake this thing
                // all the fucking time because I'm pretty sure there is something
                // that is actually used elsewhere
                for(auto it = filepath.rbegin(); it != filepath.rend(); it++)
                {
                    if(str)
                }

                for (size_t i = 0; i < str.size(); ++i)
                {
                    if (start_ext)
                        ext += str[i];
                    if (str[i] == '.')
                        start_ext = true;
                }
                return ext;
            }
            

            /*!
             * @brief   Checks if a resource file exist in the resource folder
             */
            bool file_exist(const std::string& filepath)
            {
                return std::filesystem::exists(resource_folder_+"/"+filepath);
            }

            /*!
             * @brief   Convert the image at @a filepath 
             * 
             *          Will only work with png files for now
             * 
             * @param   filepath    filepath relative to the resource directory where
             *                      containing the thing
             */
            bool convert(const std::string& filepath)
            {
                std::filesystem::
            }

        private:


        std::string resource_folder_;
    };
}