/*
============================================================================
Agmd3D - 3D Engine
Author : Cyril Basset (basset.cyril@gmail.com - https://github.com/Agamand)
https://github.com/Agamand/AgmdEngine
============================================================================
*/

#include <Core/FontManager.h>
#include <Core/RenderObject/GraphicString.h>
#include <Core/SceneObject/Model.h>
#include <Core/Renderer.h>
#include <Core/Texture/Texture.h>
#include <Core/Texture/Image.h>
#include <windows.h>
#include <algorithm>
#include <vector>
#include <Debug/Logger.h>
#include <Debug/New.h>
SINGLETON_IMPL(Agmd::FontManager);

namespace Agmd
{
    FontManager::FontManager() :
    m_transform(new Transform())
    {
        m_program.LoadFromFile("Shader/text.glsl");
    }

    FontManager::~FontManager()
    {
        UnloadFonts();
        delete m_transform;
    }

    void FontManager::Initialize()
    {
        TDeclarationElement decl[] =
        {
            {0, ELT_USAGE_POSITION,  ELT_TYPE_FLOAT3},
            {0, ELT_USAGE_TEXCOORD0,    ELT_TYPE_FLOAT2}
        };

        m_Declaration = Renderer::Get().CreateVertexDeclaration(decl);

        /*m_VertexBuffer = Renderer::Get().CreateVertexBuffer<TVertex>(4, BUF_STATIC);

        TVertex* vertices = m_VertexBuffer.Lock(0, 0, LOCK_WRITEONLY);

        vertices[0].position = vec4(0,0,0,1);
        vertices[1].position = vec4(1,0,0,1);
        vertices[2].position = vec4(0,1,0,1);
        vertices[3].position = vec4(1,1,0,1);

        vertices[0].vertexId = 0;
        vertices[1].vertexId = 1;
        vertices[2].vertexId = 2;
        vertices[3].vertexId = 3;
        
        m_VertexBuffer.Unlock();

        std::vector<TIndex> Indices;
        Indices.push_back(0);
        Indices.push_back(1);
        Indices.push_back(2);
        Indices.push_back(1);
        Indices.push_back(3);
        Indices.push_back(2);

        m_IndexBuffer = Renderer::Get().CreateIndexBuffer((int)Indices.size(), 0, &Indices[0]);*/
    }

    void FontManager::UnloadFonts()
    {
        m_Fonts.clear();
    }

    void FontManager::LoadFont(const std::string& fontName, int quality)
    {
        if (m_Fonts.find(fontName) != m_Fonts.end())
            return;

        int texSize = quality * 16;

        HDC hdc = CreateCompatibleDC(NULL);
        if (!hdc)
            throw LoadingFailed(fontName, "Error : Load font fail (error in CreateCompatibleDC)");

        BITMAPINFO bitmapInfo;
        memset(&bitmapInfo, 0, sizeof(BITMAPINFO));
        bitmapInfo.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
        bitmapInfo.bmiHeader.biWidth    = texSize;
        bitmapInfo.bmiHeader.biHeight   = texSize;
        bitmapInfo.bmiHeader.biBitCount = 24;
        bitmapInfo.bmiHeader.biPlanes   = 1;

        unsigned char* data = NULL;
        HBITMAP bitmapHandle = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, reinterpret_cast<void**>(&data), NULL, 0);
        if (!bitmapHandle)
            throw LoadingFailed(fontName, "Impossible de charger la police (erreur dans CreateDIBSection)");

        // Load font
        HFONT fontHandle = CreateFont(quality, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, fontName.c_str());
        if (!fontHandle)
            throw LoadingFailed(fontName, "Impossible de charger la police (erreur dans CreateFont)");

        SelectObject(hdc, bitmapHandle);
        SelectObject(hdc, fontHandle);
        SetBkColor(hdc, RGB(0, 0, 0));
        SetTextColor(hdc, RGB(255, 255, 255));

        ivec2 charSize[256];
        char character = 0;
        for (int j = 0; j < 16; ++j)
            for (int i = 0; i < 16; ++i, ++character)
            {
 
                SIZE size;
                GetTextExtentPoint32(hdc, &character, 1, &size);
                charSize[i + j * 16].x = size.cx;
                charSize[i + j * 16].y = size.cy;

                RECT rect = {i * quality, j * quality, (i + 1) * quality, (j + 1) * quality};
                DrawText(hdc, &character, 1, &rect, DT_LEFT);
            }

        Image imgFont(ivec2(texSize, texSize), PXF_A8L8);
        for (int i = 0; i < texSize; ++i)
            for (int j = 0; j < texSize; ++j)
            {
                unsigned char* CurPixel = data + (i + j * texSize) * 3;
                imgFont.SetPixel(i, j, Color(CurPixel[0], CurPixel[1], CurPixel[2], CurPixel[0]));
            }

        DeleteObject(fontHandle);
        DeleteObject(bitmapHandle);
        DeleteDC(hdc);


        m_Fonts[fontName].texture.CreateFromImage(imgFont, PXF_A8L8, TEX_NOMIPMAP, fontName);

        std::copy(charSize, charSize + 256, m_Fonts[fontName].charSize);

        Logger::Log(LOGNORMAL,"Chargement de la police : %s ",fontName.c_str());
    }

    void FontManager::DrawString(const GraphicString& str)
    {        
        uint32 color = Renderer::Get().ConvertColor(str.m_color);

        if (str.m_Text == "")
            return;

        if (!m_Declaration)
            Initialize();

        if (m_Fonts.find(str.m_Font) == m_Fonts.end())
            LoadFont(str.m_Font);

        const TFont& curFont = m_Fonts[str.m_Font];
        
        Renderer::Get().Enable(RENDER_ZTEST, false);
        Renderer::Get().SetCurrentTransform(str.m_transform);
        Renderer::Get().SetDeclaration(m_Declaration);
        Renderer::Get().SetTexture(0, curFont.texture.GetTexture());
        Renderer::Get().SetVertexBuffer(0, str.m_VertexBuffer);
        Renderer::Get().SetIndexBuffer(str.m_IndexBuffer);
        Renderer::Get().SetCurrentProgram(m_program.GetShaderProgram());
        Renderer::Get().DrawIndexedPrimitives(PT_TRIANGLELIST,0, str.m_IndexBuffer.GetCount());
        Renderer::Get().SetCurrentProgram(NULL);
    }

    void FontManager::GenerateStringMesh(GraphicString& str)
    {
        if (str.m_Text == "")
            return;

        if (!m_Declaration)
            Initialize();

        if (m_Fonts.find(str.m_Font) == m_Fonts.end())
            LoadFont(str.m_Font);

        const TFont& curFont = m_Fonts[str.m_Font];
        float texUnit = 1.0f / 16.0f;
        float ratio = str.m_Size * 16.0f / curFont.texture.GetSize().x;
        float offset  = 0.5f / curFont.texture.GetSize().x;
        float x, y = x = 0.0f;


        std::vector<TVertex> _vertex;
        int nbChars = 0;
        for (std::string::const_iterator i = str.m_Text.begin(); (i != str.m_Text.end()) && (nbChars < nbCharMax); ++i)
        {
            unsigned char c = *i;

            switch (c)
            {
                // Saut de ligne
                case '\n' :
                    x = 0.0f;
                    y -= curFont.charSize['\n'].y * ratio;
                    continue;

                // Retour au d�but de ligne
                case '\r' :
                    x = 0.0f;
                    continue;

                // Tabulation horizontale
                case '\t' :
                    x += 4 * curFont.charSize[' '].x * ratio;
                    continue;

                // Tabulation verticale
                case '\v' :
                    y -= 4 * curFont.charSize['\n'].y * ratio;
                    continue;

                // Espace
                case ' ' :
                    x += curFont.charSize[' '].x * ratio;
                    continue;
            }
            /*
			texCoord0 = vec2(texUnit * ((c % 16) + 0) + offset, 1.0f - texUnit * ((c / 16) + 1) - offset);
			break;
		case 1:
			texCoord0 = vec2(texUnit * ((c % 16) + 1) - offset, 1.0f - texUnit * ((c / 16) + 1) - offset);
			break;
		case 2:
			texCoord0 = vec2(texUnit * ((c % 16) + 0) + offset, 1.0f - texUnit * ((c / 16) + 0) + offset);
			break;
		case 3:
			texCoord0 = vec2(texUnit * ((c % 16) + 1) - offset, 1.0f - texUnit * ((c / 16) + 0) + offset);
            */

            TVertex v[] = {
                {vec3(x,y,0), vec2(texUnit * ((c % 16) + 0) + offset, 1.0f - texUnit * ((c / 16) + 1) - offset)},
                {vec3(str.m_Size+x,y,0), vec2(texUnit * ((c % 16) + 1) - offset, 1.0f - texUnit * ((c / 16) + 1) - offset)},
                {vec3(x,str.m_Size+y,0), vec2(texUnit * ((c % 16) + 0) + offset, 1.0f - texUnit * ((c / 16) + 0) + offset)},
                {vec3(str.m_Size+x,str.m_Size+y,0), vec2(texUnit * ((c % 16) + 1) - offset, 1.0f - texUnit * ((c / 16) + 0) + offset)}
            };
            _vertex.push_back(v[0]);
            _vertex.push_back(v[1]);
            _vertex.push_back(v[2]);
            _vertex.push_back(v[3]);
            /*
            for(uint32 i = 0, len = _vertex.size()/4; i < len; i++)
            {
                _indices.push_back(0+i*4);
                _indices.push_back(1+i*4);
                _indices.push_back(2+i*4);
                _indices.push_back(1+i*4);
                _indices.push_back(3+i*4);
                _indices.push_back(2+i*4);
            }*/
            //m_transform->SetPosition(vec3(x,y,0));
            //Renderer::Get().DrawIndexedPrimitives(PT_TRIANGLELIST, 0, 6);

            x += curFont.charSize[c].x * ratio;
            ++nbChars;
        }
        str.m_VertexBuffer.Release();
        str.m_IndexBuffer.Release();

        std::vector<TIndex> _indices;
        for(uint32 i = 0, len = _vertex.size()/4; i < len; i++)
        {
            _indices.push_back(0+i*4);
            _indices.push_back(1+i*4);
            _indices.push_back(2+i*4);
            _indices.push_back(1+i*4);
            _indices.push_back(3+i*4);
            _indices.push_back(2+i*4);
        }
        str.m_VertexBuffer = Renderer::Get().CreateVertexBuffer<TVertex>(_vertex.size(),0,&(_vertex[0]));
        str.m_IndexBuffer = Renderer::Get().CreateIndexBuffer<TIndex>(_indices.size(),0,&(_indices[0]));
    }

    ivec2 FontManager::GetStringPixelSize(const GraphicString& str)
    {
        if (m_Fonts.find(str.m_Font) == m_Fonts.end())
            LoadFont(str.m_Font);

        const TFont& curFont = m_Fonts[str.m_Font];
        const float  ratio   = str.m_Size * 16.0f / curFont.texture.GetSize().x;

        std::vector<int> lengths;

        ivec2 size(0, str.m_Size);

        for (std::string::const_iterator i = str.m_Text.begin(); i != str.m_Text.end(); ++i)
        {
            unsigned char c = *i;

            switch (c)
            {
                // Retour � la ligne
                case '\n' :
                    lengths.push_back(size.x);
                    size.y += str.m_Size;
                    size.x = 0;
                    break;

                // Retour au d�but de ligne
                case '\r' :
                    lengths.push_back(size.x);
                    size.x = 0;
                    break;

                // Tabulation horizontale
                case '\t' :
                    size.x += (int)(4 * curFont.charSize[' '].x * ratio);
                    break;

                // Tabulation verticale
                case '\v' :
                    size.y += 4 * str.m_Size;
                    break;

                // D�faut
                default :
                    size.x += (int)(curFont.charSize[c].x * ratio);
                    break;
            }
        }

        lengths.push_back(size.x);
        size.x = *std::max_element(lengths.begin(), lengths.end());

        return size;
    }

}
