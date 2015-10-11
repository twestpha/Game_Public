#ifndef File_h
#define File_h

#include <string>

using namespace std;

class File {
public:
    File();
    File(string filepath);
    File(string directory, string filename);

    bool isBlank();

    void setFilename(string filename);
    void setDirectory(string directory);

    string getFilename();
    string getDirectory();
    string getFilepath();

    void save();
    virtual void saveAs(string filepath);


private:

    bool hasLeadingDirectory(string filepath, int delim_index);
    bool needsTrailingDelimiter(string directory);

    string filename;
    string directory;

    char path_delimiter;
};

#endif
