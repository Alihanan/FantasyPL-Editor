#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <GLFW/glfw3.h>
#include <unistd.h>

#define MAX_NUM_UBOS 10
#define MAX_NUM_VARS 20
#define NUM_ALIGN_TYPES 6
#define NUM_STAGES 5

const char* HEADER_TO_CREATE = "./include/vulkan/vDynamicShader.tmp";
char* SHADER_NAME;
const char* CURRENT_FILE_EXT;
FILE* header_file;

typedef struct structUVariable
{
    const char* type;
    unsigned int align;
    bool isGLM;
    char* type_vulkan;
} UTypeVariable;

typedef struct
{
    int location;
    char name[20];
    UTypeVariable var;
} Variable;

typedef struct 
{
    char* stageFileExt;
    char* stageVulkanName;
} ShaderStage;

typedef struct structUBO
{
    int binding;
    char name[20];
    
    Variable variables[20];
    uint32_t num_vars;

    uint32_t num_stages;
    const char* stages[10];
} UBO;




const UTypeVariable align_vars[NUM_ALIGN_TYPES] = {
    {"vec2", 8, true, "VK_FORMAT_R32G32_SFLOAT"},
    {"vec3", 16, true, "VK_FORMAT_R32G32B32_SFLOAT"},
    {"vec4", 16, true, "VK_FORMAT_R32G32B32A32_SFLOAT"},
    {"mat4", 16, true, "VK_FORMAT_R32G32B32A32_SFLOAT"},
    {"float", 4, false, "VK_FORMAT_R32_SFLOAT"},
    {"int", 4, false, "VK_FORMAT_R32_SINT"},
};

const ShaderStage SHADER_STAGES[NUM_STAGES] = {
    {".vert", "VK_SHADER_STAGE_VERTEX_BIT"}, 
    {".frag", "VK_SHADER_STAGE_FRAGMENT_BIT"}, 
    {".geom", "VK_SHADER_STAGE_GEOMETRY_BIT"},
    {".tesc", "VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT"},
    {".tese", "VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT"},
};


UTypeVariable getTypeFromString(const char* str)
{
    for(int i = 0; i < NUM_ALIGN_TYPES; i++)
    {
        if(strcmp(str, align_vars[i].type) == 0)
        {
            return align_vars[i];
        }
    }
    exit(-1);
}   

Variable ALL_IN_VARIABLES[MAX_NUM_VARS];
int IN_VAR_INDEX = 0;

UBO ALL_UBOS[MAX_NUM_UBOS];
int UBO_INDEX = 0;

void resetUBOs()
{
    for(int i = 0; i < MAX_NUM_UBOS; i++)
    {
        ALL_UBOS[i].num_vars = 0;
    }
}

void dieMessage(const char* msg)
{
    printf("%s\n", msg);
    exit(-1);
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

void writeSingleVar(Variable var, bool isAlign)
{
    if(isAlign)
        fprintf(header_file, "alignas(%d) ", var.var.align);
    fprintf(header_file, "%s%s %s;\n",            
            var.var.isGLM ? "glm::" : "", 
            var.var.type, var.name);
}

void writeInVariables()
{
    // Sort by location
    for(int i = 0; i < IN_VAR_INDEX; i++)
    {
        for(int j = 0; j < (IN_VAR_INDEX - i - 1); j++)
        {
            Variable var2 = ALL_IN_VARIABLES[j + 1];
            Variable var1 = ALL_IN_VARIABLES[j];
            if(var1.location > var2.location)
            {
                ALL_IN_VARIABLES[j+1] = var1;
                ALL_IN_VARIABLES[j] = var2;
            }
        }
    }
    
    // Write struct
    fprintf(header_file, "        struct %s_Vertex {\n", SHADER_NAME);
    for(int i = 0; i < IN_VAR_INDEX; i++)
    {
        Variable var = ALL_IN_VARIABLES[i];

        fprintf(header_file, "            ");
        writeSingleVar(var, false);
    }
    fprintf(header_file, "        };\n");

    if(IN_VAR_INDEX > 0){
    // Binding desc
        fprintf(header_file, ""
    "        const std::vector<VkVertexInputBindingDescription> binding = \n"   
    "           {{0, sizeof(%s_Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};\n\n",

        SHADER_NAME);
    }
    else {
        fprintf(header_file, ""
    "        const std::vector<VkVertexInputBindingDescription> binding = \n"   
    "           {};\n\n" 
        );
    }

    fprintf(header_file, 
"        const std::vector<VkVertexInputBindingDescription>&\n"
"            getBindingDescriptions()\n"
"        {\n"
"            return binding;\n");
    fprintf(header_file, ""
"        }\n"
    );

    // Attrib desc
    fprintf(header_file, ""
"        const std::vector<VkVertexInputAttributeDescription> attrib = {\n"
    );
    
    for(int i = 0; i < IN_VAR_INDEX; i++)
    {
        Variable var = ALL_IN_VARIABLES[i];
        fprintf(header_file, "                "
        "{%d, 0, %s, offsetof(%s_Vertex, %s)},\n", 
                    var.location, var.var.type_vulkan, SHADER_NAME, var.name);
    }

    fprintf(header_file, ""
"        };\n"
"        const std::vector<VkVertexInputAttributeDescription>&\n"
"            getAttributeDescriptions()\n"
"        {\n"
"            return attrib;\n"
"        }\n");  
}

void writeUBOS()
{
    fprintf(header_file, "        "
        "const std::vector<VkPushConstantRange> pushConstRanges = {\n");

    for(int i = 0; i < UBO_INDEX; i++)
    {
        
        fprintf(header_file, "        "
            "   { ");

        fprintf(header_file, "%s ", ALL_UBOS[i].stages[0]);
        for(int j = 1; j < ALL_UBOS[i].num_stages; j++)
        {
            fprintf(header_file, "| %s", ALL_UBOS[i].stages[j]);
        }

        fprintf(header_file, ", 0, sizeof(%s_ubo)},\n", ALL_UBOS[i].name);
    }



    fprintf(header_file, "        "
        "};\n");
    fprintf(header_file, "        "
        "const std::vector<VkPushConstantRange>& getPushConstantRanges() {\n");
    fprintf(header_file, "        "
        "   return pushConstRanges;\n");
    fprintf(header_file, "        "
        "};\n");

    for(int i = 0; i < UBO_INDEX; i++)
    {
        // Add struct type
        UBO* ubo = &ALL_UBOS[i];
        fprintf(header_file, "        "
            "struct %s_ubo{\n", ubo->name);

        for(int v = 0; v < ubo->num_vars; v++)
        {
            fprintf(header_file, "            ");
            writeSingleVar(ubo->variables[v], true);
        }
        fprintf(header_file, "        "
            "};\n");


        

        // getters + setters for struct
        fprintf(header_file, "        "
                "%s_ubo %s;\n\n", ubo->name, ubo->name);

        fprintf(header_file, "        "
                "%s_ubo get_%s()\n"
                "        "
                " { return this->%s; }\n\n",
                ubo->name, ubo->name, ubo->name);

        fprintf(header_file, "        "
                "void set_%s(%s_ubo in) \n"
                "        "
                "{\n"
                "        "
                "   this->%s = in;\n"
                ,
                ubo->name, ubo->name, ubo->name);
        
        
        

        fprintf(header_file, "        "
                    "}");
        
    }
    
    // TODO
    fprintf(header_file, "\n        "
                "void pushUBOS(VkCommandBuffer buffer) {\n");
    for(int i = 0; i < UBO_INDEX; i++)
    {  
        UBO* ubo = &ALL_UBOS[i];
        fprintf(header_file, "        "
                    "    vkCmdPushConstants(buffer, this->layout, \n                ");
        fprintf(header_file, "%s ", ALL_UBOS[i].stages[0]);
        for(int j = 1; j < ALL_UBOS[i].num_stages; j++)
        {
            fprintf(header_file, "| %s", ALL_UBOS[i].stages[j]);
        }
        fprintf(header_file, ",\n                0, sizeof(%s_ubo),\n", ubo->name);
        fprintf(header_file, "                &this->%s\n            );\n", ubo->name);
    }
    fprintf(header_file, "        "
                    "}\n");

}


const char* getCurrentStage()
{
    printf("Current shader: %s\n", CURRENT_FILE_EXT);
    for(int i = 0; i < NUM_STAGES; i++)
    {
        if(strcmp(SHADER_STAGES[i].stageFileExt, CURRENT_FILE_EXT) == 0)
        {
            printf("Vulkan name: %s\n", SHADER_STAGES[i].stageVulkanName);
            return SHADER_STAGES[i].stageVulkanName;
        }
    }
    return NULL;
}


bool checkIfUBOExists()
{
    const char* currentName = ALL_UBOS[UBO_INDEX].name;
    
    for(int i = 0; i < UBO_INDEX; i++)
    {
        if(strcmp(currentName, ALL_UBOS[i].name) == 0)
        {
            
            ALL_UBOS[i].stages[ALL_UBOS[i].num_stages] = getCurrentStage();
            ALL_UBOS[i].num_stages++;
            printf("Adding stage: %d : %s\n", ALL_UBOS[i].num_stages, ALL_UBOS[i].stages[ALL_UBOS[i].num_stages - 1]);

            ALL_UBOS[UBO_INDEX].name[0] = 0;
            for(int i = 0; i < ALL_UBOS[UBO_INDEX].num_vars; i++)
            {
                ALL_UBOS[UBO_INDEX].variables[i].name[0] = 0;
            }
            ALL_UBOS[UBO_INDEX].num_vars = 0;
            UBO_INDEX--;

            

            return true;
        }
    }

    uint32_t curr_num = ALL_UBOS[UBO_INDEX].num_stages;
    ALL_UBOS[UBO_INDEX].stages[curr_num] = getCurrentStage();
    ALL_UBOS[UBO_INDEX].num_stages = 1;
    printf("Adding stage: %d : %s\n", ALL_UBOS[UBO_INDEX].num_stages, ALL_UBOS[UBO_INDEX].stages[ALL_UBOS[UBO_INDEX].num_stages - 1]);
    return false;
}


bool insideUBO = false;

void processStartUBO(char** statement, int index)
{
    printf("UBO: %s {\n", statement[5]);
    //strncpy(ALL_UBOS[UBO_INDEX].name, statement[5], 20);
    //ALL_UBOS[UBO_INDEX].binding = atoi(statement[4]);
}
void processAddVarUBO(char** statement, int index)
{
    if(strcmp(statement[1], ";") == 0)
    {
        strncpy(ALL_UBOS[UBO_INDEX].name, statement[0], 20);
        UBO_INDEX++;
        insideUBO = false;
        printf("END OF UBO\n");
        return;
    }

    uint32_t current = ALL_UBOS[UBO_INDEX].num_vars;
    ALL_UBOS[UBO_INDEX].variables[current].var = getTypeFromString(statement[0]);
    strncpy(ALL_UBOS[UBO_INDEX].variables[current].name, statement[1], 20);
    ALL_UBOS[UBO_INDEX].num_vars++;
    printf("%s %s\n", statement[0], statement[1]);
}
void processEndUBO(char** statement, int index)
{
    printf("}\n");
    bool ret = checkIfUBOExists();
    if(ret)
    {
        insideUBO = false;
    }
    //UBO_INDEX++;
}
void processVariable(char** statement, int index)
{
    int location = atoi(statement[4]);
    char* in_out = statement[6];
    if(strcmp(in_out, "out") == 0) return; // out vars are useless
    char* type = statement[7];
    char* name = statement[8];

    printf("     >>>>>> adding variable: loc=%d, %s %s\n", location, type, name);

    ALL_IN_VARIABLES[IN_VAR_INDEX].location = location;
    strncpy(ALL_IN_VARIABLES[IN_VAR_INDEX].name, name, 20);
    ALL_IN_VARIABLES[IN_VAR_INDEX].var = getTypeFromString(type);
    IN_VAR_INDEX++;
}

void processStatement(char** statement, int index)
{
    for(int i = 0; i < index; i++)
    {
        printf("[%s] ", statement[i]);
    }
    printf("\n");
    

    if(strcmp(statement[0], "}") == 0)
    {
        if(insideUBO)
            processEndUBO(statement, index);
        //insideUBO = false;
        return;
    }
    else if(
        (strcmp(statement[2], "push_constant") == 0 )&&
        (strcmp(statement[4], "uniform") == 0) &&
        (strcmp(statement[6], "{") == 0))
    {
        insideUBO = true;
        processStartUBO(statement, index);
    }
    else if(!insideUBO && (strcmp(statement[0], "layout") != 0))
    {
        return;
    }
    else if(insideUBO)
    {
        processAddVarUBO(statement, index);
    }
    else if(strcmp(CURRENT_FILE_EXT, ".vert") != 0)
    {
        return;
    }
    else if(strcmp(statement[2], "location") == 0)
    {
        // process variable
        processVariable(statement, index);
    }
    


}

void clearStatement(char** statement, int index)
{
    for(int i = 0; i < index; i++)
    {
        //printf("[%s],", statement[i]);
        free(statement[i]);
    }   
    //printf("\n");
}

/**
 *  Working with extracting statements from files
 */
void fileReadUntilNewline(FILE* f)
{
    char c;
    while(true){
        c = fgetc(f);
        if(c == '\n' || c == EOF)
            break;
    }
}
void fileReadUntilBigCommentEnd(FILE* f)
{
    char curr;
    char prevCurr = 0;
    while(true){
        curr = fgetc(f);
        if(curr == EOF)
            break;
        if(curr == '/' && prevCurr == '*')
        {
            break;;
        }
        prevCurr = curr;
    }
}
char prevBreaker = EOF;

char* readWord(FILE* f)
{
    char* word = (char*)malloc(sizeof(char) * 100);
    char current;
    int index = 0;

    bool toBreak = false;
    while(true)
    {
        if(prevBreaker != EOF)
        {
            word[0] = prevBreaker;
            prevBreaker = EOF;
            index++;
            break;
        }
        if(toBreak) break;

        current = fgetc(f);

        if(current == EOF)
        {
            if(index == 0)
            {
                free(word);
                return NULL;
            }
            break;          
        }
        if(index >= 1 && word[index-1] == '/')
        {
            index = 0;
            if(current == '*')
                fileReadUntilBigCommentEnd(f);
            else if(current == '/')
                fileReadUntilNewline(f);
            continue;
        }


        if(current == ' ' || current == '\n')
        {
            if(index == 0) continue;
            break;
        }
        if( current == '{' || current == ';' || current == '=' ||
            current == '}' || current == ')' || current == '(')
        {          
            if(index == 0) toBreak = true;
            else{          
                prevBreaker = current;     
                break;
            }           
        }

        word[index] = current;
        index++;
    }
    word[index] = '\0';
    return word;
}

bool checkIfMain(char** statement)
{
    bool ret1 = (strcmp(statement[0], "void") == 0);
    ret1 = ret1 && (strcmp(statement[1], "main") == 0);
    ret1 = ret1 && (strcmp(statement[2], "(") == 0);
    ret1 = ret1 && (strcmp(statement[3], ")") == 0);
    ret1 = ret1 && (strcmp(statement[4], "{") == 0);

    return ret1;
}

bool pushStatement(char** statement, int* index)
{
    if(*index == 5)
    {
        bool ret = checkIfMain(statement);
        if(ret) return true;
    }

    processStatement(statement, *index);
    clearStatement(statement, *index);    
    *index = 0;
    return false;
} 


void readShader(const char* file)
{
    size_t size = strlen(file);
    const char* format = (file + size - 5);
    CURRENT_FILE_EXT = format;

    //bool isVertex = (strcmp(format, ".vert") == 0);

    FILE* shader_file = fopen(file, "r");

    char** statement = (char**)malloc(sizeof(char*) * 100);
    int index = 0;

    while(true)
    {
        // Read a word
        char* word = readWord(shader_file);
        
        // if NULL -> EOF encountered
        if(word == NULL){
            pushStatement(statement, &index);
            printf("EOF!\n");
            break;
        } 

        // if #version -> first word -> just read until the end
        if(strcmp(word, "#version") == 0){
            fileReadUntilNewline(shader_file);
            free(word);
            continue;
        }

        // save word to current statement
        statement[index] = word;
        index++;

        if(strcmp(word, "}") == 0 || strcmp(word, "{") == 0 || strcmp(word, ";") == 0)
        {
            // recursion
            bool ret = pushStatement(statement, &index);
            if(ret) break;
        }

       
    }

    fclose(shader_file);
}

/**
 *  CONSTANT STRING INSERTION + CLASS DECLARATIONS
 * 
*/
void createHeader()
{
    if(access(HEADER_TO_CREATE, F_OK) == 0)
    {
        header_file = fopen(HEADER_TO_CREATE, "a"); // "a"       

        return;
    }

    header_file = fopen(HEADER_TO_CREATE, "w"); // "a" 
    fprintf(header_file, "#pragma once\n");
    fprintf(header_file, "#include \"vShader.h\"\n\n\n");
}
void startProtectedHeaderClass()
{
    fprintf(header_file, "  protected:\n");
}
void closeHeader()
{
    fclose(header_file);
}
void beginHeaderClass(int argc, char** argv)
{
    fprintf(header_file, "// Dynamically generated from shaders:\n//    ");
    for(int i = 1; i < argc; i++)
    {
        fprintf(header_file, "%s ", argv[i]);
    }
    
    fprintf(header_file, "\n");
    fprintf(header_file, "namespace PL{\n");
    fprintf(header_file, "    class vShader_%s : public vShader\n", SHADER_NAME);
    fprintf(header_file, "    {\n");
    fprintf(header_file, "    public:\n");
    fprintf(header_file, "        vShader_%s(vDevice* device)\n", SHADER_NAME);
    fprintf(header_file, "            : vShader({\n            ");
    for(int i = 1; i < argc; i++)
    {
        fprintf(header_file, "\"%s\",", argv[i]);
    }
    fprintf(header_file, "\n            }, device)\n");
    fprintf(header_file, "        {}\n");
    fprintf(header_file, "        ~vShader_%s() {}\n", SHADER_NAME);
    
}
void endHeaderClass()
{
    fprintf(header_file, "    };\n");
    fprintf(header_file, "}\n");
}


/**
 *   Main function 
*/
int main(int argc, char* argv[])
{
    printf("Given shader files:%d\n", argc-1);
    if(argc < 3)
    {
        dieMessage("Not enough shader name given! Should be at least 2 -> vert + frag");
    }
    resetUBOs();

    createHeader();
    SHADER_NAME = extractName(argv[1]);
    beginHeaderClass(argc, &argv[0]);
    for(int i = 1; i < argc; i++)
    {
        printf("%s\n", argv[i]);     
        readShader(argv[i]);      
    }
    writeInVariables();
    writeUBOS();
    //startProtectedHeaderClass();
    endHeaderClass();
    closeHeader();

    return 0;
}