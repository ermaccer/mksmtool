// mksmtool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <filesystem>
#include <string>

#include "afs.h"

#define PS2_PADSIZE 2048


// TODO
// apply file dates to afs sounds

int makePad(int value, int pad)
{
    return (value + pad - 1) & -pad;
}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::cout << "Usage: mksmtool <file>\n";
        return 0;
    }

    std::string input = argv[argc - 1];


    std::string outputFolder = input;
    outputFolder += "_out";

    std::ifstream pFile(input, std::ifstream::binary);

    if (!pFile)
    {
        std::cout << "ERROR: Could not open file " << input << "!" << std::endl;
        return 0;
    }

    std::filesystem::create_directory(outputFolder);
    std::filesystem::current_path(outputFolder);
    
    std::vector<int> afsSizes;
    std::vector<int> afsOffsets;

    int afsCounter = 0;

    while (!pFile.eof())
    {
        afs_header header;


        pFile.read((char*)&header, sizeof(afs_header));

        if (header.header == '\0SFA')
        {
            unsigned int currentOffset = pFile.tellg();
            currentOffset -= sizeof(afs_header);

            std::vector<afs_entry> files;

            std::string folder = "AFS_" + std::to_string(afsCounter);
            std::filesystem::create_directory(folder);
            std::filesystem::current_path(folder);

            for (int i = 0; i < header.numFiles; i++)
            {
                afs_entry file;
                pFile.read((char*)&file, sizeof(afs_entry));
                files.push_back(file);
            }

            afs_dictionary dictInfo;

            pFile.read((char*)&dictInfo, sizeof(afs_dictionary));

            std::vector<afs_dictionary_entry> dictionary;



            int numDict = dictInfo.size / sizeof(afs_dictionary_entry);

            pFile.seekg(currentOffset + dictInfo.offset, pFile.beg);
            for (int i = 0; i < numDict; i++)
            {
                afs_dictionary_entry ent;
                pFile.read((char*)&ent, sizeof(afs_dictionary_entry));
                dictionary.push_back(ent);
            }
            unsigned int lastOffset = pFile.tellg();
            for (unsigned int i = 0; i < header.numFiles; i++)
            {
                afs_dictionary_entry& dict = dictionary[i];
                afs_entry& file = files[i];
                std::cout << "Processing: " << dict.name << std::endl;
                
                pFile.seekg(file.offset + currentOffset, pFile.beg);

                int dataSize = file.size;

                std::unique_ptr<char[]> dataBuff = std::make_unique<char[]>(dataSize);
                pFile.read(dataBuff.get(), dataSize);

                std::ofstream oFile(dict.name, std::ofstream::binary);
                oFile.write(dataBuff.get(), dataSize);

            }


            int headerSize = makePad(sizeof(afs_header) + sizeof(afs_dictionary) + (header.numFiles * sizeof(afs_entry)), PS2_PADSIZE);
            int dataSize = 0;

            for (auto& f : files)
                dataSize += makePad(f.size, PS2_PADSIZE);

            int dictSize = makePad(dictInfo.size, PS2_PADSIZE);

            std::cout << "Header size: " << headerSize << std::endl;
            std::cout << "Data size: " << dataSize << std::endl;
            std::cout << "Dictionary size: " << dictSize << std::endl;

            int finalSize = headerSize + dataSize + dictSize;
            std::cout << "Final size: " << finalSize << std::endl;

            std::cout << "AFS " << afsCounter << std::endl;
            afsCounter++;

            pFile.seekg(lastOffset);
            std::filesystem::current_path("..");
        }


        
    }


    
    return 1;
}

