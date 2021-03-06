/*
============================================================================
AgmdUtilities - Utilities Lib
Author : Cyril Basset (basset.cyril@gmail.com - https://github.com/Agamand)
https://github.com/Agamand/AgmdEngine
============================================================================
*/

#include <Utilities/File.h>
#include <algorithm>
#include <fstream>


namespace AgmdUtilities
{
    File::File(const std::string& name) :
    m_Name(name)
    {
        std::replace(m_Name.begin(), m_Name.end(), '/', '\\');
    }

    File::File(const char* name) :
    m_Name(name)
    {
        std::replace(m_Name.begin(), m_Name.end(), '/', '\\');
    }

    bool File::Exists() const
    {
        std::ifstream File(m_Name.c_str());

        return File.is_open();
    }

    const std::string& File::Fullname() const
    {
        return m_Name;
    }

    std::string File::Filename() const
    {
        std::string::size_type pos = m_Name.find_last_of("\\/");

        if (pos != std::string::npos)
            return m_Name.substr(pos + 1, std::string::npos);
        else
            return m_Name;
    }

    std::string File::ShortFilename() const
    {
        return Filename().substr(0, Filename().find_last_of("."));
    }

    std::string File::Extension() const
    {
        std::string::size_type pos = m_Name.find_last_of(".");
        if (pos != std::string::npos)
            return m_Name.substr(pos + 1, std::string::npos);
        else
            return "";
    }

    std::string File::Path() const
    {
        std::string::size_type pos = m_Name.find_last_of('\\');
        if (pos != std::string::npos)
            return m_Name.substr(0, pos);
        else
            return "";
    }

}