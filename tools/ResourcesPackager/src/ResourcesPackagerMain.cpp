#include "ResourcesPackager.h"

int main(int argc, char** argv)
{
    corgi::ResourcePackager rp(argv[1]);
    rp.run();
}