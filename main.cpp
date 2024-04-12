#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>

void Error() {
    std::cout << "Invalid input, print --help to check command list\n";
}

void Parse(std::string str, std::string& command, std::string& file1, std::string& file2) {
    command = str.substr(0, str.find(' '));
    if (str.find(' ') == std::string::npos) return;
    str.erase(0, str.find(' ')+1);
    file1 = str.substr(0, str.find(' '));
    if (str.find(' ') == std::string::npos) return;
    str.erase(0, str.find(' ')+1);
    file2 = str.substr(0, str.find('\0'));
}

void Help() {
    std::cout << "-cp Copying a file; Usage: -cp SOURCE_FILE... DIRECTORY...\n"
              << "-mv Rename or move file; Usage: -mv SOURCE_MOVABLE_FILE... DIRECTORY...\n"
              << "-info Get some information about file; Usage: -info SOURCE_FILE...\n"
              << "-chmod Changes access rights; Usage: -chmod RIGHTS_FOR_UGO... SOURCE_FILE..\n"
              << "-exit Exit from program; Usage: -exit\n";
}

std::string AccessRights(mode_t str) {
    std::string res = "rwxrwxrwx";
    for (size_t i = 0; i < 9; i++)
         res[i] = (str & (1 << (8-i))) ? res[i] : '-';
    return res;
}

void Info(const char *file1) {
    struct stat fileInfo{};
    if (stat(file1, &fileInfo))
        perror("info");
    else
        std::cout << "Access rights: " << AccessRights(fileInfo.st_mode) << '\n'
                  << "Size: " << fileInfo.st_size << " bites\n"
                  << "Last time of change: " << ctime(&fileInfo.st_ctime);
}

void Chmod(std::string mode, const char* file) {
    if(mode.size()!=9) {Error(); return;}
    mode_t bitMode = 0;
    char typ[] = {'r', 'w', 'x'};
    for (size_t i = 0; i < 9; i++) {
        if(mode[i] != typ[i%3] and mode[i] != '-') {Error(); return;}
        bitMode += mode[i] == '-' ? 0 : (1 << (8-i));
    }
    if(chmod(file, bitMode))
        perror("chmod");
    else
        std::cout << "The changes have been accepted\n";
}

void Mv(const char *movableFile, const char *movedFile) {
    if(rename(movableFile, movedFile))
        perror("mv");
    else
        std::cout << "File have been moved\n";
}

void Cp(const std::string& inputFile, const std::string& outputFile) {
    if(inputFile == outputFile) {Error(); return;}
    std::ifstream fIn(inputFile, std::ios::binary);
    if(fIn.is_open()) {
        std::ofstream fOut(outputFile, std::ios::binary);
        size_t bufSize = 4;
        char* buf = new char[bufSize];
        while(!fIn.eof()) {
            fIn.read(buf, bufSize);
            if(fIn.gcount()) fOut.write(buf, fIn.gcount());
        }
        fIn.close();
        fOut.close();
        delete[] buf;
        std::cout << "File have been copped\n";
    }
    else
        std::cout << "No such file or directory\n";
}

void Run(const std::string& command, const std::string& file1, const std::string& file2) {
    if (command == "--help" and file1.empty() and file2.empty())
        Help();
    else if(command == "-info" and file2.empty() )
        Info(file1.c_str());
    else if(command == "-chmod")
        Chmod(file1, file2.c_str());
    else if(command == "-mv")
        Mv(file1.c_str(), file2.c_str());
    else if(command == "-cp")
        Cp(file1, file2);
    else if(command == "-exit" and file1.empty() and file2.empty())
        exit(0);
    else
        Error();
}

int main() {
    while(true) {
        std::string str, command, file1, file2;
        getline(std::cin, str);
        Parse(str, command, file1, file2);
        Run(command, file1, file2);
    }
}