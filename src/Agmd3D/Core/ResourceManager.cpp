/*
============================================================================
Agmd3D - 3D Engine
Author : Cyril Basset (basset.cyril@gmail.com - https://github.com/Agamand)
https://github.com/Agamand/AgmdEngine
============================================================================
*/

#include <Core/ResourceManager.h>
#include <Debug/Logger.h>
#include <Debug/Exception.h>
#include <Debug/New.h>


SINGLETON_IMPL(Agmd::ResourceManager);

namespace Agmd
{

    

    ResourceManager::ResourceManager()
    {}

    ResourceManager::~ResourceManager()
    {
        if (!m_Resources.empty())
        {
            Logger::Log(LOGNORMAL,"** warning ** Des ressources n'ont pas �t� lib�r�es :");

            for (TResourcesMap::const_iterator i = m_Resources.begin(); i != m_Resources.end(); ++i)
            {
                Logger::Log(LOGNORMAL," - %s",i->second->GetName().c_str());
            }
        }
    }

    void ResourceManager::Add(const std::string& name, Resource* resource)
    {
        Assert(resource != NULL);

        if (m_Resources.find(name) != m_Resources.end())
            Logger::Log(LOGNORMAL,"%S : ressource d�j� charg�e !",name.c_str());

        m_Resources[name] = resource;
        resource->m_Name = name;
    }

    void ResourceManager::Remove(const std::string& name)
    {
        TResourcesMap::iterator itr = m_Resources.find(name);

        if (itr == m_Resources.end())
            Logger::Log(LOGNORMAL,"%S : ressource d�truite mais non-charg�e !",name.c_str());

        m_Resources.erase(itr);
    }

}
