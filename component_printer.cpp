/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Bryn Murrell $
   ======================================================================== */
#include "blib_utils.h"
#include <stdio.h>
#include <malloc.h>
#include <string>
#include "Parsing.h"
#include "len_string.h"
#include "DebugFileIO.cpp"
#include "ComponentType.h"
#include <unordered_map>

#include "metaprogramming.h"

std::vector<len_string> f_names;
std::vector<len_string> field_names;

int main (int argc, char** argv)
{
    if (!(argc < 3) && is_cpp_or_h_file(argv[1]))
    {
        //printf("cpp or h file passed\n");
    }
    else
    {
        //printf("cpp or h file passed\n");
    }
    u64 length = 0;
    char* full_file_str = read_entire_file_text(argv[1], &length);
#define RCW "REGISTER_COMPONENT_WIDGET"
#define RWV "REGISTER_WIDGET_VAL"
#define RWEE "REGISTER_WIDGET_ENUM_ENTRY"
    u32 id = 0;
    len_string builder = l_string(1);
    
    Token rwv =  token(TOKEN_IDENTIFIER, strlen(RWV), RWV);
    Token rwee = token(TOKEN_IDENTIFIER, strlen(RWEE), RWEE);
    Token state = token(TOKEN_IDENTIFIER, 5, "state");
#define ENTITIES "entities"
    Token entities = token(TOKEN_IDENTIFIER, strlen(ENTITIES), ENTITIES);

    char* out_name_only = argv[2];
    u32 last_id = 0;
    do
    {
        if (*out_name_only == '\\' ||
            *out_name_only == '/')
        {
            last_id = out_name_only - argv[2];
        }
    } while(*out_name_only++);
    if (last_id)
    {
        out_name_only = &argv[2][last_id+1];
    }
    else
    {
        out_name_only = argv[2];
    }

    last_id = 0;
    char* tmp = out_name_only;
    while (*tmp != '.')
    {
        last_id++;
        tmp++;
    }
    len_string out_file_name = l_string(out_name_only, last_id);

    char buf[256];
    sprintf(buf, "%s_print_vals_func", out_file_name.str);
    len_string func_type_name = l_string(buf);
    sprintf(buf, "#define %s_PRINT_VALS_FUNC(name) len_string name(State* state, u32 id)\ntypedef %s_PRINT_VALS_FUNC(%s);\n",
            out_file_name.str,out_file_name.str, func_type_name.str);
    
    append_to_len_string(&builder, buf);
    while(find_next_id_in_file(&full_file_str, RCW, strlen(RCW)))
    {
        Tokenizer tok = {};
        tok.at = full_file_str;
        std::vector<len_string> array_names;
        std::vector<len_string> array_types;
        len_string enum_name;
        while(true)
        {
            Token t = getToken(&tok);
            if (t == token(TOKEN_BRACE_CLOSE, 1, "}"))
            {
                
                break;
            }
            if (t == rwee)
            {
                t = getToken(&tok);
                t = getToken(&tok);
                if (t == state &&
                    getToken(&tok) == token(TOKEN_POINTER_ACCESS, 2, "->"))
                {
                    Token arr = getToken(&tok);
                    if (arr == entities)
                    {
                        find_next_token_in_file(&tok.at, token(TOKEN_OR_ASSIGNMENT, 2, "|="));
                        Token enum_id = getToken(&tok);
                        enum_name = l_string(enum_id.text, enum_id.length);
                        //printf("%s\n",enum_name.str);
                    }
                                    
                }
            }
            else if (t == rwv)
            {
                t = getToken(&tok);
                t = getToken(&tok);
                if (t == state &&
                    getToken(&tok) == token(TOKEN_POINTER_ACCESS, 2, "->"))
                {
                    Token arr = getToken(&tok);
                    find_next_token(&tok, token(TOKEN_COMMA, 1, ","));
                    Token type = getToken(&tok);
                    len_string arr_str = l_string(arr.text, arr.length);
                    len_string type_str = l_string(type.text, type.length);
                    array_names.push_back(arr_str);
                    array_types.push_back(type_str);
                    //printf("\t%s is an array of %s\n",arr_str.str, type_str.str);
                }
            }
        }
        if (array_names.size() > 0)
        {
            sprintf(buf, "%s_print_buffers", enum_name.str);
            len_string f_name = l_string(buf);
            f_names.push_back(f_name);
            field_names.push_back(enum_name);
            sprintf(buf, "%s_PRINT_VALS_FUNC(%s)\n{\n", out_file_name.str, f_name.str);
            append_to_len_string(&builder, buf);
            append_to_len_string(&builder, "\tlen_string s = l_string(1);\n");
            sprintf(buf, "\tappend_to_len_string(&s, \"%s \\n\");\n",enum_name.str);
            append_to_len_string(&builder, buf);
            
            LOOP(i, array_names.size())
            {
                sprintf(buf, "\t{\n\t%s val = state->%s[id];\n", array_types[i].str, array_names[i].str);
                append_to_len_string(&builder, buf);
                
                if (streq("v2", array_types[i].str, 2))
                {
                    append_to_len_string(&builder, "\tchar buf[256];\n");
                    sprintf(buf, "\tsprintf(buf, \"   %s[id] == %%.2f %%.2f\\n\", val.x, val.y);\n",
                            array_names[i].str);
                    append_to_len_string(&builder, buf);
                    sprintf(buf, "\tappend_to_len_string(&s, buf);\n");                    
                    append_to_len_string(&builder, buf);
                }
                else if (streq("v3", array_types[i].str, 2))
                {
                    append_to_len_string(&builder, "\tchar buf[256];\n");
                    sprintf(buf, "\tsprintf(buf, \"   %s[id] == %%.2f %%.2f %%.2f\\n\", val.x, val.y, val.z);\n",
                            array_names[i].str);
                    append_to_len_string(&builder, buf);
                    sprintf(buf, "\tappend_to_len_string(&s, buf);\n");                    
                    append_to_len_string(&builder, buf);
                }
                else if (streq("v4", array_types[i].str, 2))
                {
                 
                    append_to_len_string(&builder, "\tchar buf[256];\n");
                    sprintf(buf, "\tsprintf(buf, \"   %s[id] == %%.2f %%.2f %%.2f %%.2f\\n\", val.x, val.y, val.z, val.w);\n",
                            array_names[i].str);
                    append_to_len_string(&builder, buf);
                    sprintf(buf, "\tappend_to_len_string(&s, buf);\n");                    
                    append_to_len_string(&builder, buf);   
                }
                else if (streq("r32", array_types[i].str, 3))
                {
                    append_to_len_string(&builder, "\tchar buf[256];\n");
                    sprintf(buf, "\tsprintf(buf, \"   %s[id] == %%.2f\\n\", val);\n",
                            array_names[i].str);
                    append_to_len_string(&builder, buf);
                    sprintf(buf, "\tappend_to_len_string(&s, buf);\n");                    
                    append_to_len_string(&builder, buf);   
                }
                else
                {
                    append_to_len_string(&builder, "\tchar buf[256];\n");
                    sprintf(buf, "\tsprintf(buf, \"   %s[id] == %%d\\n\", val);\n",
                            array_names[i].str);
                    append_to_len_string(&builder, buf);
                    sprintf(buf, "\tappend_to_len_string(&s, buf);\n");                    
                    append_to_len_string(&builder, buf);                       
                }
                append_to_len_string(&builder, "\t}\n");
            }
            append_to_len_string(&builder, "\treturn s;\n");
            append_to_len_string(&builder, "}\n\n");
        }        
        
    }

    
    sprintf(buf, "std::unordered_map<u32, %s*> %s_functions = \n", func_type_name.str, out_file_name.str);
    append_to_len_string(&builder, buf);
    append_to_len_string(&builder, "{\n");
    LOOP(i, f_names.size())
    {
        sprintf(buf, "\t{ (u32)%s,  %s }", field_names[i].str, f_names[i].str);
        append_to_len_string(&builder, buf);
        if (i != f_names.size() - 1)
        {
            append_to_len_string(&builder, ",\n");
        }
        else
        {
            append_to_len_string(&builder, "\n");
        }
    }
    append_to_len_string(&builder, "};\n\n");
    
    FILE* out_file = fopen(argv[2],"wb");
    fwrite(builder.str, 1, builder.string_len, out_file);
    fclose(out_file);
    
}
