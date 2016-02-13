#include "pap_bfs.h"


std::string loadFileContent(std::string filename)
{
    std::ifstream sourcefile(filename, std::ios::in);

    if (!sourcefile)
    {
        std::string msg("Could not load file: ");
        msg.append(filename);
        throw msg;
    }

    std::stringstream buffer;
    buffer << sourcefile.rdbuf();
    return buffer.str();

}
