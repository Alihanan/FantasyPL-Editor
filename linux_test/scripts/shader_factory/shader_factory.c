#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

typedef unsigned int uint32_t;

const char* TMP_HEADER = "./vulkan/v_global_renderer/v_pipeline/v_shader/v_dyn_shader.tmp";
const char* HEADER_TO_CREATE = "./vulkan/v_global_renderer/v_pipeline/v_shader/v_dyn_shader.h";
const char* CPP_TO_CREATE = "./vulkan/v_global_renderer/v_pipeline/v_shader/v_dyn_shader.cpp";

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
        if(currIndex >= 100) exit(-1);
        char c = string[currIndex];
        if(c == '_') break;
        FOUND_SHADERS[NUM_FOUND_SHADERS].name[currIndex] = c;
        currIndex++;
    }
    FOUND_SHADERS[NUM_FOUND_SHADERS].name[currIndex] = '\0';

    FOUND_SHADERS[NUM_FOUND_SHADERS].className[0] = 'v';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[1] = '_';

    for(int i = 0; i < currIndex; i++)
    {
        FOUND_SHADERS[NUM_FOUND_SHADERS].className[i + 2] = 
            FOUND_SHADERS[NUM_FOUND_SHADERS].name[i];
    }
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[currIndex + 2] = '_';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[currIndex+3] = 's';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[currIndex+4] = 'h';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[currIndex+5] = 'a';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[currIndex+6] = 'd';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[currIndex+7] = 'e';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[currIndex+8] = 'r';
    FOUND_SHADERS[NUM_FOUND_SHADERS].className[currIndex+9] = '\0';


    NUM_FOUND_SHADERS++;
}

void findAllShaderClasses()
{
    FILE* f = fopen(TMP_HEADER, "r");
    char buffer[1024];
    const char* prefix = "    class v_";
    char f_prefix[13];
    while(true)
    {
        char* ret = fgets(buffer, 1024, f);
        if(ret == NULL) break;
        uint32_t len = strlen(buffer);
        if(len < 13)
        {
            continue;
        }
        strncpy(f_prefix, buffer, 12);
        f_prefix[12] = 0;
        if(strcmp(f_prefix, prefix) == 0)
        {
            addShaderClass(buffer+12, 1024-13);
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
    fprintf(f, "#include \"v_dyn_shader.h\"\n\n\n");

    fprintf(f, "namespace PL {\n");
    fprintf(f, "    "
    "va_shader* va_shader::createShader(std::string name, VkDevice& device)\n");
    fprintf(f, "    {\n");
    fprintf(f, "        if(va_shader::ALL_SHADERS_USED.find(name) "
                "!= va_shader::ALL_SHADERS_USED.end())\n"
            "        {\n"
            "            return va_shader::ALL_SHADERS_USED[name];\n"
            "        }\n\n");

    for(int i = 0; i < NUM_FOUND_SHADERS; i++)
    {
        ShaderName name = FOUND_SHADERS[i];
        fprintf(f, "        if(name == std::string(\"%s\")) {\n", name.name);
        fprintf(f, "            va_shader::ALL_SHADERS_USED[name] = new %s(device);\n", name.className);
        fprintf(f, "        }\n\n");
    }
    fprintf(f, "        if(va_shader::ALL_SHADERS_USED.find(name) "
                "!= va_shader::ALL_SHADERS_USED.end())\n"
            "        {\n"
            "            return va_shader::ALL_SHADERS_USED[name];\n"
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