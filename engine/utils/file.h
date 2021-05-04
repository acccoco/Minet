#ifndef RENDER_FILE_H
#define RENDER_FILE_H

#include <vector>
#include <string>

class File {
protected:

public:
    static std::string file_load_str(const std::string &file_name);
    static std::vector<std::string> file_load_lines(const std::string &file_name);
};


#endif //RENDER_FILE_H
