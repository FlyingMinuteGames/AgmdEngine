/*
============================================================================
Agmd3D - 3D Engine
Author : Cyril Basset (basset.cyril@gmail.com - https://github.com/Agamand)
https://github.com/Agamand/AgmdEngine
============================================================================
*/

#include <Core/Texture/TextureBase.h>
#include <Debug/Exception.h>


namespace Agmd
{

    TextureBase::TextureBase(const ivec2& size, TPixelFormat format, bool hasMipmaps, bool autoMipmaps) :
    m_Size       (size),
    m_Format     (format),
    m_HasMipmaps (hasMipmaps),
    m_AutoMipmaps(autoMipmaps)
    {}

    TextureBase::~TextureBase()
    {}
}
