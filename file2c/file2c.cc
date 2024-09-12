#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_set>
#include <fstream>
#include <memory>
#include <cassert>

const char *inputFileName = nullptr, *outputFileName = nullptr;
const char *varName = nullptr;
std::unordered_set<std::string> includeFilePaths;
size_t chunkSize = 4096;

static const char *_requireArg(int argc, char* argv[], int &i) {
    i += 1;
    if(i >= argc)
        return nullptr;
    return argv[i];
}

#define _printError(msg, ...) fprintf(stderr, "Error: " msg "\n", ##__VA_ARGS__)
#define _printWarning(msg, ...) fprintf(stderr, "Warning: " msg "\n", ##__VA_ARGS__)

char digit2Hex(uint8_t digit) {
    assert(digit < 16);

    return digit < 10 ? digit + '0' : (digit - 10) + 'a';
}

int main(int argc, char *argv[]) {
    for(int i = 1 ; i < argc; ++i) {
        const char *curArg = argv[i];

        if(!strcmp(curArg, "-I")) {
            const char *path = _requireArg(argc, argv, i);
            if(!path) {
                _printError("-I requires an include path");
                return -1;
            }

            includeFilePaths.insert(path);
        } else if(!strcmp(curArg, "-o")) {
            const char *path = _requireArg(argc, argv, i);
            if(!path) {
                _printError("-o requires an output path");
                return -1;
            }

            if(outputFileName) {
                _printError("Output file name is already set");
                return -1;
            }

            outputFileName = path;
        } else if(!strcmp(curArg, "-n")) {
            const char *name = _requireArg(argc, argv, i);
            if(!name) {
                _printError("-n requires a variable name");
                return -1;
            }

            if(varName) {
                _printError("Variable name is already set");
                return -1;
            }

            varName = name;
        } else if(!strcmp(curArg, "-s")) {
            const char *chunkSizeStr = _requireArg(argc, argv, i);
            if(!chunkSizeStr) {
                _printError("-s requires a chunk size");
                return -1;
            }

            unsigned long long chunkSizeIn = strtoull(chunkSizeStr, nullptr, 10);
            if(chunkSizeIn > SIZE_MAX) {
                _printError("Chunk size is bigger than the size supported by the machine");
                return -1;
            }

            chunkSize = chunkSizeIn;
        } else {
            if(inputFileName) {
                _printError("Input file name is already set");
                return -1;
            }

            inputFileName = curArg;
        }
    }

    if(!inputFileName) {
        _printError("Missing input file name");
        return -1;
    }
    
    if(!outputFileName) {
        _printError("Missing output file name");
        return -1;
    }
    
    if(!varName) {
        _printError("Missing variable name");
        return -1;
    }

    std::ifstream is(inputFileName, std::ios::in | std::ios::binary);
    if(is.bad() || is.fail()) {
        _printError("Error opening the input file");
        return -1;
    }

    std::ofstream os(outputFileName, std::ios::out | std::ios::trunc);
    if(os.bad() || os.fail()) {
        _printError("Error opening the output file");
        return -1;
    }

    for(auto i : includeFilePaths) {
        os << "#include " << i << std::endl;
    }

    os << "const char " << varName << "[] = \"";

    size_t totalFileSize = 0;
    {
        std::unique_ptr<char[]> chunk = std::make_unique<char[]>(chunkSize);

        char byteBuf[] = "\\x00";

        while(true) {
            std::streamsize szRead = is.readsome(chunk.get(), chunkSize);
            totalFileSize += szRead;

            if(is.fail() || is.bad()) {
                _printError("Error reading the input file");
                return -1;
            }

            for(size_t i = 0 ; i < (size_t)szRead; ++i) {
                uint8_t curByte = *(uint8_t*)(chunk.get() + i);
                
                byteBuf[2] = digit2Hex(curByte >> 4);
                byteBuf[3] = digit2Hex(curByte & 0b1111);
                os.write(byteBuf, sizeof(byteBuf) - 1);
            }
            
            if(os.fail() || os.bad()) {
                _printError("Error reading the output file");
                return -1;
            }

            if((size_t)szRead < chunkSize)
                break;
        }
    }

    os << "\";" << std::endl;

    os << "size_t " << varName << "_length = " << totalFileSize << ";" << std::endl;

    is.close();
    os.close();

    return 0;
}
