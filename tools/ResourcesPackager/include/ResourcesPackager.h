#pragma once

#include <string>
#include <filesystem>

namespace corgi
{
    class ResourcePackager
    {
        public:

            ResourcePackager(const std::string& resource_folder) :
                resource_folder_(resource_folder){}


            void run();

            void clear();

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
            bool convert(const std::string& filepath);

        private:

        std::string resource_folder_;
    };
}