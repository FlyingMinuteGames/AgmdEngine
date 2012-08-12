#include <Core/MediaManager.h>
#include <Debug/New.h>


namespace Agmd
{

	SINGLETON_IMPL(MediaManager)

	MediaManager::MediaManager()
	{
		m_Paths.insert("");
	}

	MediaManager::~MediaManager()
	{}

	void MediaManager::AddSearchPath(const std::string& path)
	{
		if (path.empty() || (*path.rbegin() == '\\') || (*path.rbegin() == '/'))
			m_Paths.insert(path);
		else
			m_Paths.insert(path + "\\");
	}

	File MediaManager::FindMedia(const File& filename) const
	{
		for (std::set<std::string>::const_iterator itr = m_Paths.begin(); itr != m_Paths.end(); ++itr)
		{
			File retFile = *itr + filename.Fullname();
			if (retFile.Exists())
				return retFile;
		}

		throw LoadingFailed(filename.Fullname(), "Fichier introuvable dans les répertoires de recherche");
	}

}
