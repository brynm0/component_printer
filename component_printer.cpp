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

std::vector<len_string> f_names_plaintext;
std::vector<len_string> f_names_json;
std::vector<len_string> field_names_plaintext;
std::vector<len_string> field_names_json;

flocal void write_entity_plaintext_func(len_string* builder,
                                        len_string out_file_name,
                                        len_string enum_name,
                                        std::vector<len_string> array_names,
                                        std::vector<len_string> array_types)
{
    char buf[256];
    sprintf(buf, "%s_print_buffers", enum_name.str);
    len_string f_name = l_string(buf);
    f_names_plaintext.push_back(f_name);
    field_names_plaintext.push_back(enum_name);
    sprintf(buf, "%s_PRINT_VALS_FUNC(%s)\n{\n", out_file_name.str, f_name.str);
    append_to_len_string(builder, buf);
    append_to_len_string(builder, "\tlen_string s = l_string(1);\n");
    sprintf(buf, "\tappend_to_len_string(&s, \"%s \\n\");\n",enum_name.str);
    append_to_len_string(builder, buf);
            
    LOOP(i, array_names.size())
    {
        sprintf(buf, "\t{\n\t%s val = state->%s[id];\n", array_types[i].str, array_names[i].str);
        append_to_len_string(builder, buf);
                
        if (streq("v2", array_types[i].str, 2))
        {
            append_to_len_string(builder, "\tchar buf[256];\n");
            sprintf(buf, "\tsprintf(buf, \"   %s[id] == %%.2f %%.2f\\n\", val.x, val.y);\n",
                    array_names[i].str);
            append_to_len_string(builder, buf);
            sprintf(buf, "\tappend_to_len_string(&s, buf);\n");                    
            append_to_len_string(builder, buf);
        }
        else if (streq("axis_bool_pair", array_types[i].str, 14))
        {
            append_to_len_string(builder, "\tchar buf[256];\n");
            sprintf(buf, "\tsprintf(buf, \"   %s[id] == %%d %%d\\n\", val.x_axis, val.y_axis);\n",
                    array_names[i].str);
            append_to_len_string(builder, buf);
            sprintf(buf, "\tappend_to_len_string(&s, buf);\n");                    
            append_to_len_string(builder, buf);
        }
        else if (streq("QuadData", array_types[i].str, 8))
        {
            append_to_len_string(builder, "\tchar buf[256];\n");
            sprintf(buf, "\tsprintf(buf, \"   %s[id] == %%.2f %%.2f %%.2f %%.2f\\n\", val.topLeft.x, val.topLeft.y, val.bottomRight.x, val.bottomRight.y);\n",
                    array_names[i].str);
            append_to_len_string(builder, buf);
            sprintf(buf, "\tappend_to_len_string(&s, buf);\n");                    
            append_to_len_string(builder, buf);
        }
        else if (streq("v3", array_types[i].str, 2))
        {
            append_to_len_string(builder, "\tchar buf[256];\n");
            sprintf(buf, "\tsprintf(buf, \"   %s[id] == %%.2f %%.2f %%.2f\\n\", val.x, val.y, val.z);\n",
                    array_names[i].str);
            append_to_len_string(builder, buf);
            sprintf(buf, "\tappend_to_len_string(&s, buf);\n");                    
            append_to_len_string(builder, buf);
        }
        else if (streq("v4", array_types[i].str, 2))
        {
                 
            append_to_len_string(builder, "\tchar buf[256];\n");
            sprintf(buf, "\tsprintf(buf, \"   %s[id] == %%.2f %%.2f %%.2f %%.2f\\n\", val.x, val.y, val.z, val.w);\n",
                    array_names[i].str);
            append_to_len_string(builder, buf);
            sprintf(buf, "\tappend_to_len_string(&s, buf);\n");                    
            append_to_len_string(builder, buf);   
        }
        else if (streq("r32", array_types[i].str, 3))
        {
            append_to_len_string(builder, "\tchar buf[256];\n");
            sprintf(buf, "\tsprintf(buf, \"   %s[id] == %%.2f\\n\", val);\n",
                    array_names[i].str);
            append_to_len_string(builder, buf);
            sprintf(buf, "\tappend_to_len_string(&s, buf);\n");                    
            append_to_len_string(builder, buf);   
        }
        else
        {
            append_to_len_string(builder, "\tchar buf[256];\n");
            sprintf(buf, "\tsprintf(buf, \"   %s[id] == %%d\\n\", val);\n",
                    array_names[i].str);
            append_to_len_string(builder, buf);
            sprintf(buf, "\tappend_to_len_string(&s, buf);\n");                    
            append_to_len_string(builder, buf);                       
        }
        append_to_len_string(builder, "\t}\n");
    }
    append_to_len_string(builder, "\treturn s;\n");
    append_to_len_string(builder, "}\n\n");
    
}

flocal void write_entity_json_func(len_string* builder,
                                   len_string out_file_name,
                                   len_string enum_name,
                                   std::vector<len_string> array_names,
                                   std::vector<len_string> array_types)
{
    char buf[256];
    sprintf(buf, "%s_print_buffers_json", enum_name.str);
    len_string f_name = l_string(buf);
    f_names_json.push_back(f_name);
    field_names_json.push_back(enum_name);
    
    sprintf(buf, "%s_PRINT_JSON_FUNC(%s)\n{\n", out_file_name.str, f_name.str);
    append_to_len_string(builder, buf);
    append_to_len_string(builder, "\tjson j;\n");
    sprintf(buf, "\tj[\"name\"] = \"%s\";\n",enum_name.str);
    append_to_len_string(builder, buf);

    
    LOOP(i, array_names.size())
    {
        sprintf(buf, "\t{\n\t\t%s val = state->%s[id];\n", array_types[i].str, array_names[i].str);
        append_to_len_string(builder, buf);
        
        if (streq("v2", array_types[i].str, 2))
        {
            append_to_len_string(builder, "\t\t json obj_js;\n");
            
            append_to_len_string(builder, "\t\t obj_js[\"value\"] = {val.x, val.y};\n");
            append_to_len_string(builder, "\t\t obj_js[\"type\"]  = \"v2\";\n");
            sprintf(buf, "\t\tj[\"%s\"] = obj_js;", array_names[i].str);
            append_to_len_string(builder, buf);
        }
        else if (streq("v3", array_types[i].str, 2))
        {
            append_to_len_string(builder, "\t\t json obj_js;\n");
            append_to_len_string(builder, "\t\t obj_js[\"value\"] = {val.x, val.y, val.z};\n");
            append_to_len_string(builder, "\t\t obj_js[\"type\"]  = \"v3\";\n;");
            
            sprintf(buf, "\t\tj[\"%s\"] = obj_js;", array_names[i].str);
            append_to_len_string(builder, buf);
        }
        else if (streq("v4", array_types[i].str, 2))
        {
            append_to_len_string(builder, "\t\t json obj_js;\n");
            append_to_len_string(builder, "\t\t obj_js[\"value\"] = {val.x, val.y, val.z, val.w};\n");
            append_to_len_string(builder, "\t\t obj_js[\"type\"]  = \"v4\";\n");
            
            sprintf(buf, "\t\tj[\"%s\"] = obj_js;", array_names[i].str);
            append_to_len_string(builder, buf);
        }
        else if (streq("r32", array_types[i].str, 3))
        {
            append_to_len_string(builder, "\t\t json obj_js;\n");
            append_to_len_string(builder, "\t\t obj_js[\"value\"] = val;\n");
            append_to_len_string(builder, "\t\t obj_js[\"type\"]  = \"r32\";\n");
            sprintf(buf, "\t\tj[\"%s\"] = obj_js;", array_names[i].str);
            append_to_len_string(builder, buf);
        }
        else if (streq("b32", array_types[i].str, 3))
        {
            append_to_len_string(builder, "\t\t json obj_js;\n");
            append_to_len_string(builder, "\t\t obj_js[\"value\"] = val;\n");
            append_to_len_string(builder, "\t\t obj_js[\"type\"]  = \"b32\";\n");
            
            sprintf(buf, "\t\tj[\"%s\"] = obj_js;", array_names[i].str);
            append_to_len_string(builder, buf);
        }
        else if (streq("axis_bool_pair", array_types[i].str, 14))
        {
            append_to_len_string(builder, "\t\t json obj_js;\n");
            append_to_len_string(builder, "\t\t obj_js[\"type\"]  = \"axis_bool_pair\";\n");
            
            sprintf(buf, "\t\tjson abp_j;\n");
            append_to_len_string(builder, buf);
            sprintf(buf, "\t\tabp_j[\"x_axis\"] = val.x_axis;\n");
            append_to_len_string(builder, buf);
            sprintf(buf, "\t\tabp_j[\"y_axis\"] = val.y_axis;\n");
            append_to_len_string(builder, buf);
            append_to_len_string(builder, "\t\t obj_js[\"value\"] = abp_j;\n");
            sprintf(buf, "\t\tj[\"%s\"] = obj_js;\n",array_names[i].str);
            append_to_len_string(builder, buf);
        }
        else if (streq("QuadData", array_types[i].str, 8))
        {
            append_to_len_string(builder, "\t\t json obj_js;\n");
            append_to_len_string(builder, "\t\t obj_js[\"type\"]  = \"QuadData\";\n");

            sprintf(buf, "\t\tjson qd_j;\n");
            append_to_len_string(builder, buf);
            sprintf(buf, "\t\tqd_j[\"topLeft\"] = { val.topLeft.x, val.topLeft.y };\n");
            append_to_len_string(builder, buf);
            sprintf(buf, "\t\tqd_j[\"bottomRight\"] = { val.bottomRight.x, val.bottomRight.y };\n");
            append_to_len_string(builder, buf);
            append_to_len_string(builder, "\t\t obj_js[\"value\"]  = \"qd_j\";\n");
            
            sprintf(buf, "\t\tj[\"%s\"] = obj_js;\n",array_names[i].str);
            append_to_len_string(builder, buf);
        }
        else // default
        {
            append_to_len_string(builder, "\t\t json obj_js;\n");
            append_to_len_string(builder, "\t\t obj_js[\"value\"] = (u32)val;\n");
            append_to_len_string(builder, "\t\t obj_js[\"type\"]  = \"default\";\n");
            
            sprintf(buf, "\t\tj[\"%s\"] = obj_js;", array_names[i].str);
            append_to_len_string(builder, buf);            
        }
        append_to_len_string(builder, "\n\t}\n");
    }
    
    append_to_len_string(builder, "\treturn j;\n");
    append_to_len_string(builder, "}\n\n");
}
    
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
    len_string builder = l_string(1);
    
    Token rwv =  token(TOKEN_IDENTIFIER, strlen(RWV), RWV);
    Token rwee = token(TOKEN_IDENTIFIER, strlen(RWEE), RWEE);
    Token state = token(TOKEN_IDENTIFIER, 5, "state");
#define ENTITIES "entities"
    Token entities = token(TOKEN_IDENTIFIER, strlen(ENTITIES), ENTITIES);

    char* out_name_only = argv[2];
    //u32 id = 0;
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

    char buf[512];
    sprintf(buf, "%s_print_vals_func", out_file_name.str);
    len_string plaintext_func_type_name = l_string(buf);

    sprintf(buf, "%s_print_vals_func_json", out_file_name.str);
    len_string json_func_type_name = l_string(buf);

    
    sprintf(buf, "\n#define %s_PRINT_VALS_FUNC(name) len_string name(State* state, u32 id)\ntypedef %s_PRINT_VALS_FUNC(%s);\n\n",
            out_file_name.str,out_file_name.str, plaintext_func_type_name.str);
    append_to_len_string(&builder, buf);


    sprintf(buf, "\n#define %s_PRINT_JSON_FUNC(name) json name(State* state, u32 id)\ntypedef %s_PRINT_JSON_FUNC(%s);\n\n",
            out_file_name.str,out_file_name.str, json_func_type_name.str);
    append_to_len_string(&builder, buf);


    while(find_next_id_in_file(&full_file_str, RCW, strlen(RCW)))
    {
        i32 brace_counter = -1;
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
                if (brace_counter == 0)
                {
                    break;
                }
                else
                {
                    brace_counter--;
                }
            }
            if (t == token(TOKEN_BRACE_OPEN, 1, "{"))
            {
                brace_counter++;
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
            write_entity_plaintext_func(&builder, out_file_name, enum_name, array_names, array_types);
           
        }        
        write_entity_json_func(&builder, out_file_name, enum_name, array_names, array_types);
    }

    {    
        sprintf(buf, "std::unordered_map<u32, %s*> %s_functions = \n", plaintext_func_type_name.str, out_file_name.str);
        append_to_len_string(&builder, buf);
        append_to_len_string(&builder, "{\n");
        LOOP(i, f_names_plaintext.size())
        {
            sprintf(buf, "\t{ (u32)%s,  %s }", field_names_plaintext[i].str, f_names_plaintext[i].str);
            append_to_len_string(&builder, buf);
            if (i != f_names_plaintext.size() - 1)
            {
                append_to_len_string(&builder, ",\n");
            }
            else
            {
                append_to_len_string(&builder, "\n");
            }
        }
        append_to_len_string(&builder, "};\n\n");
    }
    {
        sprintf(buf, "std::unordered_map<u32, %s*> %s_json_functions = \n", json_func_type_name.str, out_file_name.str);
        append_to_len_string(&builder, buf);
        append_to_len_string(&builder, "{\n");
        LOOP(i, f_names_json.size())
        {
            sprintf(buf, "\t{ (u32)%s,  %s }", field_names_json[i].str, f_names_json[i].str);
            append_to_len_string(&builder, buf);
            if (i != f_names_json.size() - 1)
            {
                append_to_len_string(&builder, ",\n");
            }
            else
            {
                append_to_len_string(&builder, "\n");
            }
        }
        append_to_len_string(&builder, "};\n\n");
    }    
    FILE* out_file = fopen(argv[2],"wb");
    fwrite(builder.str, 1, builder.string_len, out_file);
    fclose(out_file);
    
}
