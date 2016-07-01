#include "stdinc.h"
#include "program.hpp"
#include "error.hpp"

enum class Section
{
    None,
    SomeReadable,
    SomeUnreadable,
};

static bool nextline(const char*& line, char* output, size_t output_size)
{
    const char* output_beg = output;
    const char* output_max = (output + output_size) - 1;

    while(*line == ' ' || *line == '\t' || *line == '\r' || *line == '\n')
        ++line;

    if(*line == '#')
    {
        while(*line != 0 && *line != '\n')
            ++line;
    }

    for(; output < output_max; )
    {
        if(*line == 0 || *line == '\n')
            break;

        if(*line < ' ' || *line == ',')
        {
            *output++ = ' ';
            ++line;
        }
        else
        {
            *output++ = *line++;
        }
    }

    *output = '\0';

    if(output != output_beg)
    {
        for(--output; output != output_beg; --output)
        {
            if(*output == ' ')
                *output = '\0';
            else
                break;
        }

        return (*line != 0);
    }
    else if(*line == 0)
    {
        return false;
    }
    else
    {
        // this line is empty and its not EOF, try the next
        return nextline(line, output, output_size);
    }
}

void ProgramContext::load_ide(const fs::path& filepath, bool is_default_ide)
{
    std::string file_data;

    try
    {
        file_data = read_file_utf8(filepath).value();
    }
    catch(const bad_optional_access&)
    {
        throw ConfigError("Failed to read IDE file '{}'.", filepath.generic_u8string());
    }

    char buffer[512];
    Section section = Section::None;

    for(const char* it = file_data.c_str();
        nextline(it, buffer, std::size(buffer)); )
    {
        switch(section)
        {
            case Section::None:
            {
                if(is_default_ide || !strncmp(buffer, "objs", 4) || !strncmp(buffer, "tobj", 4) || !strncmp(buffer, "anim", 4))
                    section = Section::SomeReadable;
                else
                    section = Section::SomeUnreadable;
                break;
            }

            case Section::SomeReadable:
            {
                char model_name[64];
                unsigned int id;

                if(!strncmp(buffer, "end", 3))
                {
                    section = Section::None;
                    break;
                }

                if(sscanf(buffer, "%u %63s", &id, model_name) != EOF)
                {
                    if(is_default_ide)
                        this->default_models.emplace(model_name, id);
                    else
                        this->level_models.emplace(model_name, id);
                }
                else
                {
                    __debugbreak();
                }
                break;
            }

            case Section::SomeUnreadable:
            {
                if(!strncmp(buffer, "end", 3))
                {
                    section = Section::None;
                    break;
                }
                break;
            }

            default:
                Unreachable();
        }
    }
}

void ProgramContext::load_dat(const fs::path& filepath, bool is_default_dat)
{
    std::string file_data;

    try
    {
        file_data = read_file_utf8(filepath).value();
    }
    catch(const bad_optional_access&)
    {
        throw ConfigError("Failed to read DAT file '{}'.", filepath.generic_u8string());
    }

    char buffer[512];

    fs::path gamedir = filepath;
    gamedir.remove_filename(); // remove gta.dat
    gamedir.remove_filename(); // remove data/

    for(const char* it = file_data.c_str();
        nextline(it, buffer, std::size(buffer)); )
    {
        if(!strncmp(buffer, "IDE", 3))
        {
            this->load_ide(gamedir / (buffer+4), is_default_dat);
        }
    }
}

bool ProgramContext::is_model_from_ide(const std::string& name) const
{
    if(!this->default_models.empty() || !this->level_models.empty())
    {
        auto it = this->default_models.find(name);
        if(it != default_models.end())
            return true;

        it = this->level_models.find(name);
        if(it != level_models.end())
            return true;

        return false;
    }
    else
    {
        // TODO provide a better fallback, perhaps by XML... or maybe not?
        // this is used to resolve the conflict between var names and model constants in gta3 src.
        if(iequal_to()(name, "PLAYERSDOOR")
        || iequal_to()(name, "DEADMAN1")
        || iequal_to()(name, "BACKDOOR")
        || iequal_to()(name, "HELIX_BARRIER")
        || iequal_to()(name, "AIRPORTDOOR1")
        || iequal_to()(name, "AIRPORTDOOR2"))
            return true;
        return false;
    }
}