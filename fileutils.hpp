#ifndef FILEUTILS_H
#define FILEUTILS_H
#include<iostream>
#include<stdexcept>
#include<fstream>
#include<sys/stat.h>

namespace pygame{

    std::wstring loadStringFile(const std::wstring fname){
        std::wifstream file;
        file.open(fname.c_str());
        std::wstring st;
        wchar_t ch;
        if(!file){
            std::wcerr << fname << std::endl;
            throw std::logic_error("Error: file does not exist");
        }
        while(!file.eof()){
            file >> std::noskipws >> ch;
            st += ch;
        }
        file.close();
        return st;
    }
    bool fileexists(std::wstring filename){
        struct stat trash;
        return wstat(filename.c_str(),&trash) != -1;
    }
    
}
#endif//FILEUTILS_H
