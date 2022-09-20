#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

typedef unsigned int uint32_t;

const char* TMP_HEADER = "./include/vulkan/vDynamicShader.tmp";
const char* HEADER_TO_CREATE = "./include/vulkan/vDynamicShader.h";
const char* CPP_TO_CREATE = "./source/vulkan/vDynamicShader.cpp";

typedef struct _struct_SHADERNAME
{
    char name[100];
    char className[150];
} ShaderName;
ShaderName FOUND_SHADERS[250] = {
};
uint32_t NUM_FOUND_SHADERS = 0;


void addShaderClass(char* string, uint32_t size)
{
    int currIndex = 0;
    while(true)
    {
        if(currIndex >= size) exit(-1);
        //if(currIndex >= 100) exit(-1);
        char c = string[currIndex];
        if(c == '_' || c=='(' || c == ' ') break;
        FOUND_SHADERS[NUM_FOUND_SHADERS].name[currIndex] = c;
        currIndex++;
    }

    printf("Found name of size: %d\n", currIndex);

    FOUND_SHADERS[NUM_FOUND_SHADERS].name[currIndex] = '\0';

    FOUND_SHADERS[NUM_FOUND_SHADERS].className[0] = 'v';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[1] = 'S';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[2] = 'h';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[3] = 'a';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[4] = 'd';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[5] = 'e';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[6] = 'r';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[7] = '_';

    for(int i = 0; i < currIndex; i++)
    {
        FOUND_SHADERS[NUM_FOUND_SHADERS].className[i + 8] = 
            FOUND_SHADERS[NUM_FOUND_SHADERS].name[i];
    }
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[currIndex+8] = '\0';


    NUM_FOUND_SHADERS++;
}

void findAllShaderClasses()
{
    FILE* f = fopen(TMP_HEADER, "r");
    char buffer[1024];
    const char* prefix = "    class vShader_";
    char f_prefix[19];
    while(true)
    {
        char* ret = fgets(buffer, 1024, f);
        if(ret == NULL) break;
        uint32_t len = strlen(buffer);
        if(len < 19)
        {
            continue;
        }
        strncpy(f_prefix, buffer, 18);
        f_prefix[18] = 0;
        
        if(strcmp(f_prefix, prefix) == 0)
        {
            addShaderClass(buffer+18, 1024-18);
        }

    }
}

char* extractName(const char* fullname)
{
    size_t size = strlen(fullname);

    // skip *.frag or similar
    while(size > 0)
    {
        char c = fullname[size-1];
        if(c == '.')
        {
            break;
        }
        size--;
    }

    size_t end = size - 1;
    
    while(size > 0)
    {
        char c = fullname[size - 1];
        if(c == '/')
        {
            break;
        }
        size--;
    }
    size_t start = size;

    char* ret = (char*)malloc(sizeof(char) * (end - start + 1));
    for(size_t i = start; i < end; i++)
    {
        ret[i - start] = fullname[i];
    }
    ret[end - start] = '\0';
    return ret;
}

void writeStaticFactory(FILE* f)
{
    fprintf(f, "#include \"../../include/vulkan/vDynamicShader.h\"\n\n\n");

    fprintf(f, "namespace PL {\n");
    fprintf(f, "    "
    "vShader* vShader::createShader(std::string name, vDevice* device)\n");
    fprintf(f, "    {\n");
    fprintf(f, "        if(vShader::ALL_SHADERS_USED.find(name) "
                "!= vShader::ALL_SHADERS_USED.end())\n"
            "        {\n"
            "            return vShader::ALL_SHADERS_USED[name];\n"
            "        }\n");
    fprintf(f, "        // START OF FACTORY\n\n");

    for(int i = 0; i < NUM_FOUND_SHADERS; i++)
    {
        ShaderName name = FOUND_SHADERS[i];
        fprintf(f, "        if(name == std::string(\"%s\")) {\n", name.name);
        fprintf(f, "            vShader::ALL_SHADERS_USED[name] = new %s(device);\n", name.className);
        fprintf(f, "        }\n\n");
    }

    fprintf(f, "        // END OF FACTORY\n");
    fprintf(f, "        if(vShader::ALL_SHADERS_USED.find(name) "
                "!= vShader::ALL_SHADERS_USED.end())\n"
            "        {\n"
            "            return vShader::ALL_SHADERS_USED[name];\n"
            "        }\n\n");

    fprintf(f, "        return nullptr;\n");
    fprintf(f, "    }\n");
    fprintf(f, "}\n\n");
}

void copyOneFileToAnother(FILE* from, FILE* to)
{
    char c;
    while(true)
    {
        c = fgetc(from);
        if(c == EOF) break;
        fputc(c, to);
    }
}

void writeHeader()
{
    FILE* cpp_file = fopen(CPP_TO_CREATE, "w");
    FILE* tmp_file = fopen(TMP_HEADER, "r");
    FILE* h_file = fopen(HEADER_TO_CREATE, "w");

    writeStaticFactory(cpp_file);
    copyOneFileToAnother(tmp_file, h_file);

    fclose(cpp_file);
    fclose(tmp_file);
    fclose(h_file);
}

int main(int argc, char* argv[])
{
    findAllShaderClasses();
    writeHeader();

    return 0;
}