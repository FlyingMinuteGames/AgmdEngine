/*
============================================================================
GLRender - Opengl Renderer specialization
Author : Cyril Basset (basset.cyril@gmail.com - https://github.com/Agamand)
https://github.com/Agamand/AgmdEngine
============================================================================
*/

#include <Renderer/OpenGL/GlRenderer.h>
#include <Renderer/OpenGL/GlTexture2D.h>
#include <Renderer/OpenGL/GlTextureCube.h>
#include <Renderer/OpenGL/GlShader.h>
#include <Renderer/OpenGL/GlShaderProgram.h>
#include <Renderer/OpenGL/GlEnums.h>
#include <Renderer/OpenGL/GlFrameBuffer.h>
#include <Renderer/OpenGL/GlRenderBuffer.h>
#include <Renderer/OpenGL/GlUniformBuffer.h>
#include <Renderer/OpenGL/GlTextureBuffer.h>
#include <Core/MatStack.h>
#include <core/Tools/Statistics.h>
#include <Utilities/Color.h>
#include <Utilities/PixelUtils.h>
#include <Debug/Logger.h>
#include <Matrix4.h>
#include <Debug/New.h>


SINGLETON_IMPL(Agmd::GLRenderer)

using namespace AgmdUtilities;

namespace Agmd
{
    extern "C" __declspec(dllexport) void LoadPlugin()
    {
        Renderer::Change(&GLRenderer::Instance());
    }


    extern "C" __declspec(dllexport) void UnloadPlugin()
    {
        Renderer::Destroy();
    }

    template <class T> inline void LoadExtension(T& Proc, const char* Name)
    {
        Proc = reinterpret_cast<T>(wglGetProcAddress(Name));
    }
    #define LOAD_EXTENSION(Ext) LoadExtension(Ext, #Ext)

    #define BUFFER_OFFSET(n) ((byte*)NULL + (n))

    PFNGLGETSTRINGIPROC                 GLRenderer::glGetStringi;
    PFNGLBINDBUFFERPROC                 GLRenderer::glBindBuffer;
    PFNGLDELETEBUFFERSPROC              GLRenderer::glDeleteBuffers;
    PFNGLGENBUFFERSPROC                 GLRenderer::glGenBuffers;
    PFNGLBUFFERDATAPROC                 GLRenderer::glBufferData;
    PFNGLBUFFERSUBDATAPROC              GLRenderer::glBufferSubData;
    PFNGLGETBUFFERSUBDATAPROC           GLRenderer::glGetBufferSubData;
    PFNGLMAPBUFFERPROC                  GLRenderer::glMapBuffer;
    PFNGLMAPBUFFERRANGEPROC             GLRenderer::glMapBufferRange;
    PFNGLUNMAPBUFFERPROC                GLRenderer::glUnmapBuffer;
    PFNGLDELETESYNCPROC                 GLRenderer::glDeleteSync;
    PFNGLGETSYNCIVPROC                  GLRenderer::glGetSynciv;
    PFNGLWAITSYNCPROC                   GLRenderer::glWaitSync;
    PFNGLCLIENTWAITSYNCPROC             GLRenderer::glClientWaitSync;
    PFNGLFENCESYNCPROC                  GLRenderer::glFenceSync;
    PFNGLACTIVETEXTUREPROC              GLRenderer::glActiveTexture;
    PFNGLCLIENTACTIVETEXTUREPROC        GLRenderer::glClientActiveTexture;
    PFNGLCOMPRESSEDTEXIMAGE2DPROC       GLRenderer::glCompressedTexImage2D;
    PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC    GLRenderer::glCompressedTexSubImage2D;
    PFNGLPATCHPARAMETERIPROC            GLRenderer::glPatchParameteri;
    PFNGLGENPROGRAMPIPELINESPROC        GLRenderer::glGenProgramPipelines;
    PFNGLBINDPROGRAMPIPELINEPROC        GLRenderer::glBindProgramPipelines;
    PFNGLCREATEPROGRAMPROC              GLRenderer::glCreateProgram;
    PFNGLDELETEPROGRAMPROC              GLRenderer::glDeleteProgram;
    PFNGLCREATESHADERPROC               GLRenderer::glCreateShader;
    PFNGLDELETESHADERPROC               GLRenderer::glDeleteShader;
    PFNGLSHADERSOURCEPROC               GLRenderer::glShaderSource;
    PFNGLCOMPILESHADERPROC              GLRenderer::glCompileShader;
    PFNGLGETSHADERIVPROC                GLRenderer::glGetShaderiv;
    PFNGLGETSHADERINFOLOGPROC           GLRenderer::glGetShaderInfoLog;
    PFNGLATTACHSHADERPROC               GLRenderer::glAttachShader;
    PFNGLLINKPROGRAMPROC                GLRenderer::glLinkProgram;
    PFNGLGETPROGRAMIVPROC               GLRenderer::glGetProgramiv;
    PFNGLGETPROGRAMINFOLOGPROC          GLRenderer::glGetProgramInfoLog;
    PFNGLGETACTIVEUNIFORMSIVPROC        GLRenderer::glGetActiveUniformsiv;
    PFNGLGETACTIVEUNIFORMNAMEPROC       GLRenderer::glGetActiveUniformName;
    PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC  GLRenderer::glGetActiveUniformBlockName;
    PFNGLGETACTIVEUNIFORMBLOCKIVPROC    GLRenderer::glGetActiveUniformBlockiv;
    PFNGLGETINTEGERI_VPROC              GLRenderer::glGetIntegeri_v;
    PFNGLGETACTIVEUNIFORMPROC           GLRenderer::glGetActiveUniform;
    PFNGLGETUNIFORMLOCATIONPROC         GLRenderer::glGetUniformLocation;
    PFNGLGETACTIVEATTRIBPROC            GLRenderer::glGetActiveAttrib;
    PFNGLGETATTRIBLOCATIONPROC          GLRenderer::glGetAttribLocation;
    PFNGLGETACTIVEUNIFORMBLOCKIVPROC    GLRenderer::glGetActiveUniformBlock;
    PFNGLGETUNIFORMBLOCKINDEXPROC       GLRenderer::glGetUniformBlockIndex;
    PFNGLUNIFORMBLOCKBINDINGPROC        GLRenderer::glUniformBlockBinding;
    PFNGLBINDBUFFERBASEPROC             GLRenderer::glBindBufferBase;
    PFNGLUSEPROGRAMPROC                 GLRenderer::glUseProgram;
    PFNGLUNIFORM1IPROC                  GLRenderer::glUniform1i;
    PFNGLUNIFORM2IPROC                  GLRenderer::glUniform2i;
    PFNGLUNIFORM3IPROC                  GLRenderer::glUniform3i;
    PFNGLUNIFORM4IPROC                  GLRenderer::glUniform4i;
    PFNGLUNIFORM1FPROC                  GLRenderer::glUniform1f;
    PFNGLUNIFORM2FPROC                  GLRenderer::glUniform2f;
    PFNGLUNIFORM3FPROC                  GLRenderer::glUniform3f;
    PFNGLUNIFORM4FPROC                  GLRenderer::glUniform4f;
    PFNGLUNIFORM4FVPROC                 GLRenderer::glUniform1fv;
    PFNGLUNIFORM4FVPROC                 GLRenderer::glUniform2fv;
    PFNGLUNIFORM4FVPROC                 GLRenderer::glUniform3fv;
    PFNGLUNIFORM4FVPROC                 GLRenderer::glUniform4fv;
    PFNGLUNIFORMMATRIX2FVPROC           GLRenderer::glUniformMatrix2fv;
    PFNGLUNIFORMMATRIX3FVPROC           GLRenderer::glUniformMatrix3fv;
    PFNGLUNIFORMMATRIX4FVPROC           GLRenderer::glUniformMatrix4fv;
    PFNGLVERTEXATTRIB1FNVPROC           GLRenderer::glVertexAttrib1f;
    PFNGLVERTEXATTRIB2FNVPROC           GLRenderer::glVertexAttrib2f;
    PFNGLVERTEXATTRIB3FNVPROC           GLRenderer::glVertexAttrib3f;
    PFNGLVERTEXATTRIB4FNVPROC           GLRenderer::glVertexAttrib4f;
    PFNGLVERTEXATTRIBPOINTERPROC        GLRenderer::glVertexAttribPointer;
    PFNGLENABLEVERTEXATTRIBARRAYPROC    GLRenderer::glEnableVertexAttribArray;
    PFNGLBINDATTRIBLOCATIONPROC         GLRenderer::glBindAttribLocation;
    PFNGLGENRENDERBUFFERSPROC           GLRenderer::glGenRenderbuffers;
    PFNGLDELETERENDERBUFFERSPROC        GLRenderer::glDeleteRenderbuffers;
    PFNGLBINDRENDERBUFFERPROC           GLRenderer::glBindRenderbuffer;
    PFNGLRENDERBUFFERSTORAGEPROC        GLRenderer::glRenderbufferStorage;
    PFNGLGENFRAMEBUFFERSPROC            GLRenderer::glGenFramebuffers;
    PFNGLDELETEFRAMEBUFFERSPROC         GLRenderer::glDeleteFramebuffers;
    PFNGLBINDFRAMEBUFFERPROC            GLRenderer::glBindFramebuffer;
    PFNGLDRAWBUFFERSPROC                GLRenderer::glDrawBuffers;
    PFNGLFRAMEBUFFERRENDERBUFFERPROC    GLRenderer::glFramebufferRenderbuffer;
    PFNGLFRAMEBUFFERTEXTUREPROC         GLRenderer::glFramebufferTexture;
    PFNGLFRAMEBUFFERTEXTURE2DPROC       GLRenderer::glFramebufferTexture2D;
    PFNGLFRAMEBUFFERTEXTURE3DPROC       GLRenderer::glFramebufferTexture3D;
    PFNGLCHECKFRAMEBUFFERSTATUSPROC     GLRenderer::glCheckFramebufferStatus;


    PFNWGLCREATECONTEXTATTRIBSARBPROC   GLRenderer::wglCreateContextAttribsARB;
    PFNWGLSWAPINTERVALEXTPROC           GLRenderer::wglSwapIntervalEXT;

    GLRenderer::GLRenderer() :
    m_Hwnd              (NULL),
    m_Handle            (NULL),
    m_Context           (NULL),
    m_CurrentDeclaration(NULL),
    m_Extensions        (""),
    m_CurrentProgram    (NULL),
    last_unit           (TEXTURE_UNIT_0)
    {
        std::memset(m_TextureBind,NULL,sizeof(void*)*MAX_TEXTUREUNIT);
    }

    GLRenderer::~GLRenderer()
    {
        if (m_Context)
        {
            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(m_Context);
        }

        if (m_Hwnd && m_Handle)
        {
            ReleaseDC(m_Hwnd, m_Handle);
        }
        Logger::Destroy();
    }

    std::string GLRenderer::GetRendererDesc() const
    {
        
        return std::string("OpenGL ") + reinterpret_cast<const char*>(glGetString(GL_VERSION)) + ", GLSL " + reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

    void GLRenderer::Setup(HWND Hwnd)
    {
        Logger::Instance().SetFilename("OpenGL");

        PIXELFORMATDESCRIPTOR PixelDescriptor = 
        { 
            sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd 
            1,                               // version number 
            PFD_DRAW_TO_WINDOW |             // support window 
            PFD_SUPPORT_OPENGL |             // support OpenGL 
            PFD_DOUBLEBUFFER,                // double buffered 
            PFD_TYPE_RGBA,                   // RGBA type 
            32,                              // 32-bit color depth 
            0, 0, 0, 0, 0, 0,                // color bits ignored 
            0,                               // no alpha buffer 
            0,                               // shift bit ignored 
            0,                               // no accumulation buffer 
            0, 0, 0, 0,                      // accum bits ignored 
            32,                              // 32-bit z-buffer 
            32,                              // 32-bits stencil buffer 
            0,                               // no auxiliary buffer 
            PFD_MAIN_PLANE,                  // main layer 
            0,                               // reserved 
            0, 0, 0                          // layer masks ignored 
        };

        m_Hwnd   = Hwnd;
        m_Handle = GetDC(Hwnd);

        // Get best pixel format
        assert(SetPixelFormat(m_Handle, ChoosePixelFormat(m_Handle, &PixelDescriptor), &PixelDescriptor));

        // Create Temp Glcontext for loading extentions
        m_Context = wglCreateContext(m_Handle);
        assert(wglMakeCurrent(m_Handle, m_Context));

        int attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 1,
            WGL_CONTEXT_FLAGS_ARB, 0,
            0
        };

        // Load CreateContext function
        LOAD_EXTENSION(wglCreateContextAttribsARB);

        //Recreate a real GlContext (if possible)
        if(wglCreateContextAttribsARB && false)
        {
            
            HGLRC temp =  wglCreateContextAttribsARB(m_Handle,0,attribs);
            wglMakeCurrent(NULL,NULL);
            wglDeleteContext(m_Context);
            m_Context = temp;
            wglMakeCurrent(m_Handle, m_Context);

        }

        // Load extensions
        LoadExtensions();

        // Get Supported extensions
        int n;
        glGetIntegerv(GL_NUM_EXTENSIONS, &n);
        for (int i = 0; i < n; i++)
        {
            m_Extensions += reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
            m_Extensions +="\n";
        }


        // Default states
        glClearColor(1, 1, 1, 0.0f);
        glClearDepth(1.0f);
        glDepthFunc(GL_LESS);
        glDepthRange(0.0, 1.0);
        glClearStencil(0x00);
        wglSwapIntervalEXT(0);

        glEnable(GL_DEPTH_TEST);
        glFrontFace(GL_CCW);
    }

    void GLRenderer::CheckCaps()
    {
        m_Capabilities[CAP_HW_MIPMAPPING] = CheckExtension("GL_SGIS_generate_mipmap");

        m_Capabilities[CAP_DXT_COMPRESSION] = CheckExtension("GL_ARB_texture_compression") &&
                                              CheckExtension("GL_EXT_texture_compression_s3tc");

        m_Capabilities[CAP_TEX_NON_POWER_2] = CheckExtension("GL_ARB_texture_non_power_of_two");

        m_Capabilities[CAP_TESSELATION_SHADER] = CheckExtension("GL_ARB_tessellation_shader");
    }

    bool GLRenderer::CheckExtension(const std::string& Extension) const
    {
        return m_Extensions.find(Extension) != std::string::npos;
    }

    const std::string GLRenderer::GetExtension() const
    {
        return m_Extensions;
    }

    const std::string GLRenderer::GetConstant() const
    {
        std::string str = "";
        int32 value;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &value);
        str += "Max vertex attrib : " + value;

        glGetIntegerv(GL_MAX_VARYING_FLOATS, &value);
        str += "\nMax vertex attrib : " + value;
        return str;
    }

    void GLRenderer::LoadExtensions()
    {
        LOAD_EXTENSION(glGetStringi);
        LOAD_EXTENSION(glBindBuffer);
        LOAD_EXTENSION(glDeleteBuffers);
        LOAD_EXTENSION(glGenBuffers);
        LOAD_EXTENSION(glBufferData);
        LOAD_EXTENSION(glBufferSubData);
        LOAD_EXTENSION(glBindBufferBase);
        LOAD_EXTENSION(glGetBufferSubData);
        LOAD_EXTENSION(glMapBuffer);
        LOAD_EXTENSION(glMapBufferRange);
        LOAD_EXTENSION(glUnmapBuffer);
        LOAD_EXTENSION(glDeleteSync);
        LOAD_EXTENSION(glGetSynciv);
        LOAD_EXTENSION(glWaitSync);
        LOAD_EXTENSION(glClientWaitSync);
        LOAD_EXTENSION(glFenceSync);
        LOAD_EXTENSION(glActiveTexture);
        LOAD_EXTENSION(glClientActiveTexture);
        LOAD_EXTENSION(glCompressedTexImage2D);
        LOAD_EXTENSION(glCompressedTexSubImage2D);
        LOAD_EXTENSION(glPatchParameteri);
        LOAD_EXTENSION(glGenProgramPipelines);
        LOAD_EXTENSION(glBindProgramPipelines);
        LOAD_EXTENSION(glCreateProgram);
        LOAD_EXTENSION(glDeleteProgram);
        LOAD_EXTENSION(glCreateShader);
        LOAD_EXTENSION(glDeleteShader);
        LOAD_EXTENSION(glShaderSource);
        LOAD_EXTENSION(glCompileShader);
        LOAD_EXTENSION(glGetShaderiv);
        LOAD_EXTENSION(glGetShaderInfoLog);
        LOAD_EXTENSION(glAttachShader);
        LOAD_EXTENSION(glLinkProgram);
        LOAD_EXTENSION(glGetProgramiv);
        LOAD_EXTENSION(glGetProgramInfoLog);
        LOAD_EXTENSION(glGetActiveUniformsiv);
        LOAD_EXTENSION(glGetActiveUniformName);
        LOAD_EXTENSION(glGetActiveUniformBlockName);
        LOAD_EXTENSION(glGetActiveUniformBlockiv);
        LOAD_EXTENSION(glGetIntegeri_v);
        LOAD_EXTENSION(glGetActiveUniform);
        LOAD_EXTENSION(glGetUniformLocation);
        LOAD_EXTENSION(glGetActiveAttrib);
        LOAD_EXTENSION(glGetAttribLocation);
        LOAD_EXTENSION(glGetActiveUniformBlock);
        LOAD_EXTENSION(glUniformBlockBinding);
        LOAD_EXTENSION(glGetUniformBlockIndex);
        LOAD_EXTENSION(glUseProgram);
        LOAD_EXTENSION(glUniform1i);
        LOAD_EXTENSION(glUniform2i);
        LOAD_EXTENSION(glUniform3i);
        LOAD_EXTENSION(glUniform4i);
        LOAD_EXTENSION(glUniform1f);
        LOAD_EXTENSION(glUniform2f);
        LOAD_EXTENSION(glUniform3f);
        LOAD_EXTENSION(glUniform4f);    
        LOAD_EXTENSION(glUniform1fv);
        LOAD_EXTENSION(glUniform2fv);
        LOAD_EXTENSION(glUniform3fv);
        LOAD_EXTENSION(glUniform4fv);
        LOAD_EXTENSION(glUniformMatrix2fv);
        LOAD_EXTENSION(glUniformMatrix3fv);
        LOAD_EXTENSION(glUniformMatrix4fv);
        LOAD_EXTENSION(glVertexAttrib1f);
        LOAD_EXTENSION(glVertexAttrib2f);
        LOAD_EXTENSION(glVertexAttrib3f);
        LOAD_EXTENSION(glVertexAttrib4f);
        LOAD_EXTENSION(glVertexAttribPointer);
        LOAD_EXTENSION(glBindAttribLocation);
        LOAD_EXTENSION(glEnableVertexAttribArray);
        LOAD_EXTENSION(glGenRenderbuffers);
        LOAD_EXTENSION(glDeleteRenderbuffers);
        LOAD_EXTENSION(glBindRenderbuffer);
        LOAD_EXTENSION(glRenderbufferStorage);
        LOAD_EXTENSION(glGenFramebuffers);
        LOAD_EXTENSION(glDeleteFramebuffers);
        LOAD_EXTENSION(glBindFramebuffer);
        LOAD_EXTENSION(glDrawBuffers);
        LOAD_EXTENSION(glFramebufferRenderbuffer);
        LOAD_EXTENSION(glFramebufferTexture);
        LOAD_EXTENSION(glFramebufferTexture2D);
        LOAD_EXTENSION(glFramebufferTexture3D);
        LOAD_EXTENSION(glCheckFramebufferStatus);

        
        LOAD_EXTENSION(wglSwapIntervalEXT);
    }

    void GLRenderer::InitScene()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void GLRenderer::EndScene()
    {
        //glFinish();
        SwapBuffers(m_Handle); 
    }


    BaseBuffer* GLRenderer::CreateVB(unsigned long size, unsigned long stride, unsigned long flags) const
    {
        unsigned int VertexBuffer = 0;
        glGenBuffers(1, &VertexBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, size * stride, NULL, RGLEnum::BufferFlags(flags));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        BaseBuffer* buffer = new GLVertexBuffer(size, &VertexBuffer);
        return buffer;
    }

    BaseBuffer* GLRenderer::CreateIB(unsigned long size, unsigned long stride, unsigned long flags) const
    {
        unsigned int IndexBuffer = 0;
        glGenBuffers(1, &IndexBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, IndexBuffer);
        glBufferData(GL_ARRAY_BUFFER, size * stride, NULL, RGLEnum::BufferFlags(flags));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        BaseBuffer* buffer = new GLIndexBuffer(size, &IndexBuffer);
        return buffer;
    }

    BaseBuffer* GLRenderer::CreateUB(unsigned long size, unsigned long stride, unsigned long flags, int bindPoint, int ubflags) const
    {
        unsigned int uniformBuffer[3] = {0};
        glGenBuffers(1+ubflags, uniformBuffer);

        for(int i = 0; i < ubflags+1; i++)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer[i]);
            glBufferData(GL_UNIFORM_BUFFER, size * stride, NULL, RGLEnum::BufferFlags(flags));
            glBindBufferBase(GL_UNIFORM_BUFFER,bindPoint,uniformBuffer[i]);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        BaseBuffer* buffer = NULL;
        if(ubflags == 1)
            buffer = new GLUniformBuffer<2>(size, uniformBuffer, bindPoint);
        else if(ubflags == 2)
            buffer = new GLUniformBuffer<3>(size, uniformBuffer, bindPoint);
        else buffer = new GLUniformBuffer<>(size, uniformBuffer, bindPoint);
        return buffer;
    }

    BaseBuffer* GLRenderer::CreateTB(unsigned long size, unsigned long stride, unsigned long flags) const
    {
        unsigned int textureBuffer = 0;
        glGenBuffers(1, &textureBuffer);

        glBindBuffer(GL_TEXTURE_BUFFER, textureBuffer);
        glBufferData(GL_TEXTURE_BUFFER, size * stride, NULL, RGLEnum::BufferFlags(flags));
        glBindBuffer(GL_TEXTURE_BUFFER, 0);

        return new GLTextureBuffer(size, textureBuffer,NULL);
    }

    Declaration* GLRenderer::CreateDeclaration(const TDeclarationElement* elt, std::size_t count) const
    {
        GLDeclaration* declaration = new GLDeclaration;

        std::vector<int> offset(count, 0);
        for (const TDeclarationElement* Elt = elt; Elt < elt + count; ++Elt)
        {
            GLDeclaration::TElement CurrentElement;
            CurrentElement.usage  = Elt->usage;
            CurrentElement.type   = Elt->dataType;
            CurrentElement.offset = offset[Elt->stream];

            declaration->AddElement(Elt->stream, CurrentElement);

            static const unsigned int Size[] = {4, 8, 12, 16, 4};
            offset[Elt->stream] += Size[Elt->dataType];
        }

        return declaration;
    }

    void GLRenderer::SetVB(unsigned int stream, const BaseBuffer* buffer, unsigned long stride, unsigned long minVertex, unsigned long maxVertex)
    {
        const GLVertexBuffer* VertexBuffer = static_cast<const GLVertexBuffer*>(buffer);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer->GetBuffer());

        static const unsigned int Size[] = {1, 2, 3, 4, 4, 1};
        static const unsigned int Type[] = {GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE, GL_INT};

        const GLDeclaration::TElementArray& StreamDesc = m_CurrentDeclaration->GetStreamElements(stream);
        for (GLDeclaration::TElementArray::const_iterator i = StreamDesc.begin(); i != StreamDesc.end(); ++i)
        {
            switch (i->usage)
            {
                case ELT_USAGE_POSITION :
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, Size[i->type], Type[i->type], GL_FALSE, stride, BUFFER_OFFSET(i->offset + minVertex * stride));
                    break;

                case ELT_USAGE_NORMAL :
                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(1, Size[i->type], Type[i->type], GL_FALSE, stride, BUFFER_OFFSET(i->offset + minVertex * stride));
                    break;

                case ELT_USAGE_DIFFUSE :
                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer(2, Size[i->type], Type[i->type], GL_FALSE, stride, BUFFER_OFFSET(i->offset + minVertex * stride));
                    break;

                case ELT_USAGE_TEXCOORD0 :
                    glEnableVertexAttribArray(3);
                    glVertexAttribPointer(3, Size[i->type], Type[i->type], GL_FALSE, stride, BUFFER_OFFSET(i->offset + minVertex * stride));
                    break;

                case ELT_USAGE_TEXCOORD1 :
                    glEnableVertexAttribArray(4);
                    glVertexAttribPointer(4, Size[i->type], Type[i->type], GL_FALSE, stride, BUFFER_OFFSET(i->offset + minVertex * stride));
                    break;

                case ELT_USAGE_TEXCOORD2 :
                    glEnableVertexAttribArray(5);
                    glVertexAttribPointer(5, Size[i->type], Type[i->type], GL_FALSE, stride, BUFFER_OFFSET(i->offset + minVertex * stride));
                    break;

                case ELT_USAGE_TEXCOORD3 :
                    glEnableVertexAttribArray(6);
                    glVertexAttribPointer(6, Size[i->type], Type[i->type], GL_FALSE, stride, BUFFER_OFFSET(i->offset + minVertex * stride));
                    break;

                case ELT_USAGE_TANGENT :
                    glEnableVertexAttribArray(7);
                    glVertexAttribPointer(7, Size[i->type], Type[i->type], GL_FALSE, stride, BUFFER_OFFSET(i->offset + minVertex * stride));
                    break;

                case ELT_USAGE_BONE_WEIGHT :
                    glEnableVertexAttribArray(8);
                    glVertexAttribPointer(8, Size[i->type], Type[i->type], GL_FALSE, stride, BUFFER_OFFSET(i->offset + minVertex * stride));
                    break;

                case ELT_USAGE_BONE_INDEX :
                    glEnableVertexAttribArray(9);
                    glVertexAttribPointer(9, Size[i->type], Type[i->type], GL_FALSE, stride, BUFFER_OFFSET(i->offset + minVertex * stride));
                    break;

                case ELT_USAGE_BONE_COUNT :
                    glEnableVertexAttribArray(10);
                    glVertexAttribPointer(10, Size[i->type], Type[i->type], GL_FALSE, stride, BUFFER_OFFSET(i->offset + minVertex * stride));
                    break;

                default:
                    glEnableVertexAttribArray(i->usage);
                    glVertexAttribPointer(i->usage, Size[i->type], Type[i->type], GL_FALSE, stride, BUFFER_OFFSET(i->offset + minVertex * stride));

            }
        }
    }

    void GLRenderer::SetIB(const BaseBuffer* buffer, unsigned long stride)
    {
        const GLIndexBuffer* IndexBuffer = static_cast<const GLIndexBuffer*>(buffer);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer->GetBuffer());

        m_IndexStride = stride;
    }

    void GLRenderer::SetDeclaration(const Declaration* declaration)
    {
        m_CurrentDeclaration = static_cast<const GLDeclaration*>(declaration);
    }

    void GLRenderer::DrawPrimitives(TPrimitiveType type, unsigned long firstVertex, unsigned long count)
    {
        if(!m_CurrentProgram)
            return;

        m_CurrentProgram->SetParameter(MAT_MODEL,m_CurrentTransform ? m_CurrentTransform->ModelMatrix() : mat4(1.0f));
        m_CurrentProgram->SetParameter("u_textureFlags",(int)m_TextureFlags);
        m_stats->IncrDrawCall();
        m_stats->IncrTriangleCount(count);
        

        switch (type)
        {
        case PT_TRIANGLELIST :  glDrawArrays(GL_TRIANGLES,      firstVertex, count * 3); m_stats->IncrVertexCount(count*3); break;
            case PT_TRIANGLESTRIP : glDrawArrays(GL_TRIANGLE_STRIP, firstVertex, count + 2); break;
            case PT_TRIANGLEFAN :   glDrawArrays(GL_TRIANGLE_FAN,   firstVertex, count + 1); break;
            case PT_LINELIST :      glDrawArrays(GL_LINES,          firstVertex, count * 2); break; 
            case PT_LINESTRIP :     glDrawArrays(GL_LINE_STRIP,     firstVertex, count + 1); break;
            case PT_POINTLIST :     glDrawArrays(GL_POINTS,         firstVertex, count);     break;
        }
    }

    void GLRenderer::DrawIndexedPrimitives(TPrimitiveType type, unsigned long firstIndex, unsigned long count)
    {
        if(!m_CurrentProgram)
            return;

        unsigned long indicesType = (m_IndexStride == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT);
        const void*   offset      = BUFFER_OFFSET(firstIndex * m_IndexStride);

        m_CurrentProgram->SetParameter(MAT_MODEL,m_CurrentTransform ? m_CurrentTransform->ModelMatrix() : mat4(1.0f));
        m_CurrentProgram->SetParameter("u_textureFlags",(int)m_TextureFlags);
        m_stats->IncrDrawCall();
        m_stats->IncrTriangleCount(count);
        m_stats->IncrVertexCount(count*3);

        switch (type)
        {
        case PT_TRIANGLELIST :  glDrawElements(GL_TRIANGLES,      count, indicesType, offset); break;
            case PT_TRIANGLESTRIP : glDrawElements(GL_TRIANGLE_STRIP, count, indicesType, offset); break;
            case PT_TRIANGLEFAN :   glDrawElements(GL_TRIANGLE_FAN,   count, indicesType, offset); break;
            case PT_LINELIST :      glDrawElements(GL_LINES,          count, indicesType, offset); break; 
            case PT_LINESTRIP :     glDrawElements(GL_LINE_STRIP,     count, indicesType, offset); break;
            case PT_POINTLIST :     glDrawElements(GL_POINTS,         count, indicesType, offset); break;
            case PT_PATCHLIST :     glDrawElements(GL_PATCHES,        count, indicesType, offset); break;
        }
    }

    uint32 GLRenderer::ConvertColor(const Color& color) const
    {
        return color.ToABGR();
    }

    void GLRenderer::SetTexture(uint32 unit, const TextureBase* texture, TTextureType type)
    {

        if(unit > MAX_TEXTUREUNIT)
            return;
        
        if(m_TextureBind[unit] == texture)
            return; // NO CHANGE? -> return!

        const GLTexture* oGLTexture = static_cast<const GLTexture*>(texture);

        if(last_unit != unit)
        {
            glActiveTexture(GL_TEXTURE0+unit);
            last_unit = unit;
        }

        if (texture)
        {
            glBindTexture(RGLEnum::Get(oGLTexture->GetType()), oGLTexture->GetGLTexture());
        }
        else if(m_TextureBind[unit])
        {
            glBindTexture(RGLEnum::Get(m_TextureBind[unit]->GetType()), 0);
        }

        m_TextureBind[unit] = texture;
    }

    TextureBase* GLRenderer::CreateTexture(const ivec2& size, TPixelFormat format, TTextureType type, unsigned long flags) const
    {
        switch(type)
        {
        case TEXTURE_2D:
            return CreateTexture2D(size,format,flags);
        case TEXTURE_CUBE:
            return CreateTextureCube(size,format,flags);
        default:
            Logger::Log(LOGERROR,"Error on CreateTexture - unknown or unsupported TextureType : %d",(int)type);
            return NULL;
        }
    }

    TextureBase* GLRenderer::CreateTexture2D(const ivec2& size, TPixelFormat format, unsigned long flags) const
    {
        uint32 texture;
        glGenTextures(1, &texture);
        int width  = size.x;
        int height = size.y;
        int nbMipmaps = 0;
        glBindTexture(GL_TEXTURE_2D, texture);

        if(format == PXF_DEPTH)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            nbMipmaps = 0;
        }else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            nbMipmaps = flags & TEX_NOMIPMAP ? 0 : GetNbMipLevels(size.x, size.y);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, nbMipmaps);

            if ((nbMipmaps > 0) && (HasCapability(CAP_HW_MIPMAPPING)))
                glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

        }

        for (int i = 0; i <= nbMipmaps; ++i)
        {
            glTexImage2D(GL_TEXTURE_2D, i, RGLEnum::Get(format).Internal, width, height, 0, RGLEnum::Get(format)._Format, format == PXF_DEPTH ? GL_FLOAT : GL_UNSIGNED_BYTE, NULL);

            if (width > 1)  width  /= 2;
            if (height > 1) height /= 2;
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        return new GLTexture2D(size, format, nbMipmaps > 0, HasCapability(CAP_HW_MIPMAPPING), texture);
    }

    TextureBase* GLRenderer::CreateTextureCube(const ivec2& size, TPixelFormat format, unsigned long flags) const
    {
        uint32 texture;
        glGenTextures(1, &texture);
        int width  = size.x;
        int height = size.y;
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


        for (int i = 0; i < 6; i++)
        {

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, RGLEnum::Get(format).Internal, width, height, 0,  RGLEnum::Get(format)._Format, GL_UNSIGNED_BYTE, NULL);
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        return new GLTextureCube(size,format,false,false,texture);
    }

    void GLRenderer::SetupAlphaBlending(TBlend src, TBlend dest) const
    {
        glBlendFunc(RGLEnum::Get(src), RGLEnum::Get(dest));
    }

    void GLRenderer::SetupDepthTest(TDepth mode)
    {
        glDepthFunc(RGLEnum::Get(mode));
    }

    void GLRenderer::SetupTextureUnit(uint32 unit, TTextureOp op, TTextureArg arg1, TTextureArg arg2, const Color& constant) const
    {
        /* <!> Deprecated feature <!> */

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0 + unit);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

        if (op < TXO_ALPHA_FIRSTARG)
        {
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, RGLEnum::Get(op));
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, RGLEnum::Get(arg1));
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, RGLEnum::Get(arg2));
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
        }
        else
        {
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, RGLEnum::Get(op));
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, RGLEnum::Get(arg1));
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, RGLEnum::Get(arg2));
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, GL_SRC_ALPHA);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_SRC_ALPHA);
        }

        if ((arg1 == TXA_CONSTANT) || (arg2 == TXA_CONSTANT))
        {
            float color[4];
            constant.ToFloat(color);
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR,color);
        }
    }
    
    void GLRenderer::Enable(TRenderParameter param, bool value)
    {
        switch (param)
        {
            case RENDER_ZWRITE :
            {
                glDepthMask(value);
                break;
            }
            case RENDER_TRANSPARENT: /* <!> Deprecated feature <!> */
            { 
                /*if(value)
                    glEnable(GL_ALPHA_TEST);
                else glDisable(GL_ALPHA_TEST);
                glAlphaFunc(GL_GEQUAL,0.7f);
                */
                break;
            }
            case RENDER_RENDERING:
                glDrawBuffer(value ? GL_BACK : GL_NONE);
            default :
            {
                int _value = RGLEnum::Get(param);

                if(!_value)
                    break;

                if (value)
                    glEnable(_value);
                else
                    glDisable(_value);
            }
        }
    }

    void GLRenderer::SetRenderMode(TRenderMode mode)
    {

        glPolygonMode(GL_FRONT_AND_BACK,RGLEnum::Get(mode));
    }

    BaseShader* GLRenderer::CreateShader(std::string source,TShaderType type) const
    {
        
        int index;
        index = source.find("#ifdef");
        std::string define = "";
        switch(type)
        {
        case SHADER_VERTEX:
            if(source.find("_VERTEX_") == std::string::npos)
                return NULL;
            define = "#define _VERTEX_\n";
            break;
        case SHADER_TESS_CONTROL:
            if(source.find("_TESS_CONTROL_") == std::string::npos)
                return NULL;
            define = "#define _TESS_CONTROL_\n";
            break;
        case SHADER_TESS_EVALUATION:
            if(source.find("_TESS_EVALUATION_") == std::string::npos)
                return NULL;
            define = "#define _TESS_EVALUATION_\n";
            break;
        case SHADER_GEOMETRY:
            if(source.find("_GEOMETRY_") == std::string::npos)
                return NULL;
            define = "#define _GEOMETRY_\n";
            break;
        case SHADER_PIXEL:
            if(source.find("_FRAGMENT_") == std::string::npos)
                return NULL;
            define = "#define _FRAGMENT_\n";
            break;

        }
        if(index != std::string::npos)
            source = source.substr(0,index)+define+source.substr(index,source.size()-1);
        std::replace(source.begin(), source.end(), '\r','\n');
        const char* src = source.c_str();
        uint32 shader = glCreateShader(RGLEnum::Get(type));
        glShaderSource(shader, 1, (const char**)&src, NULL);
    
        glCompileShader(shader);
    
        int32 compile_status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
        int32 logsize;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);
        
        char *log = new char[logsize+1];
        memset(log, '\0', logsize + 1);
        
        glGetShaderInfoLog(shader, logsize, &logsize, log);
        Logger::Log(LOGNORMAL, "%s", log);
       
        delete[] log;
        if(compile_status != GL_TRUE)
        {
            glDeleteShader(shader);
            return NULL;
        }
    
        return new GLShader(shader,type);
    }

    BaseShaderProgram* GLRenderer::CreateShaderProgram(BaseShader* vertex, BaseShader* eval, BaseShader* control, BaseShader* geom, BaseShader* frag) const
    {
        uint32 id = glCreateProgram();
        if(vertex)
            glAttachShader(id,static_cast<GLShader*>(vertex)->GetId());

        if(eval)
            glAttachShader(id,static_cast<GLShader*>(eval)->GetId());

        if(control)
            glAttachShader(id,static_cast<GLShader*>(control)->GetId());

        if(geom)
            glAttachShader(id,static_cast<GLShader*>(geom)->GetId());

        if(frag)
            glAttachShader(id,static_cast<GLShader*>(frag)->GetId());


        /* Bind engine variable*/
        glBindAttribLocation(id, 0,  "in_Vertex");
        glBindAttribLocation(id, 1,  "in_Normal");
        glBindAttribLocation(id, 2,  "in_Color");
        glBindAttribLocation(id, 3,  "in_TexCoord0");
        glBindAttribLocation(id, 4,  "in_TexCoord1");
        glBindAttribLocation(id, 5,  "in_TexCoord2");
        glBindAttribLocation(id, 6,  "in_TexCoord3");
        glBindAttribLocation(id, 7,  "in_Tangent");
        glBindAttribLocation(id, 8,  "in_BoneWeight");
        glBindAttribLocation(id, 9,  "in_BoneIndex");
        glBindAttribLocation(id, 10, "in_BoneCount");
        /**/

        glLinkProgram(id);

        int32 link;

        glGetProgramiv(id, GL_LINK_STATUS, &link);

        int32 logsize;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logsize);
    
        char* log = new char[logsize + 1];

        glGetProgramInfoLog(id, logsize, &logsize, log);
        log[logsize] = '\0';

        Logger::Log(LOGNORMAL, "%s", log);

        delete[] log;

        if(link != GL_TRUE)
        {
            glDeleteProgram(id);
            return NULL;
        }

        GLShaderProgram* program = new GLShaderProgram(id);
        program->SetParameter("cameraInfoBlock",(uint32)UNIFORM_CAMERA3D_BIND);
        program->SetParameter("lightInfoBlock",(uint32)UNIFORM_LIGHT_BIND);
        program->SetParameter("camera2DInfoBlock",(uint32)UNIFORM_CAMERA2D_BIND);
        program->UniformShaderInfo();
        return program;
    }

    const BaseShaderProgram* GLRenderer::GetCurrentProgram()
    {
        return m_CurrentProgram;
    }

    FrameBuffer* GLRenderer::CreateFrameBuffer() const
    {
        uint32 id = 0;
        glGenFramebuffers(1, &id);
        return new GLFrameBuffer(id);
    }

    RenderBuffer* GLRenderer::CreateRenderBuffer(const ivec2& size, TPixelFormat format) const
    {
        uint32 id = 0;

        glGenRenderbuffers(1, &id);
        glBindRenderbuffer(GL_RENDERBUFFER,id);
        glRenderbufferStorage(GL_RENDERBUFFER,RGLEnum::Get(format).Internal,size.x,size.y);
        glBindRenderbuffer(GL_RENDERBUFFER,0);

        return new GLRenderBuffer(id,size);
        
    }

    void GLRenderer::SetCurrentProgram(const BaseShaderProgram* prog)
    {
        m_CurrentProgram = prog;
        if(m_CurrentProgram)
            m_CurrentProgram->Use(true);
    }

    void GLRenderer::SetViewPort(const ivec2& xy, const ivec2& size)
    {
        glViewport(xy.x, xy.y, size.x, size.y);
    }

    #define FRONT 1
    #define BACK  2

    void GLRenderer::SetCullFace(int face)
    {
        if(face)
            glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);
        if(face == FRONT)
            glCullFace(GL_FRONT);
        else if(face == BACK)
            glCullFace(GL_BACK);
        else if(face & (FRONT | BACK))
            glCullFace(GL_FRONT_AND_BACK);
    }

}

