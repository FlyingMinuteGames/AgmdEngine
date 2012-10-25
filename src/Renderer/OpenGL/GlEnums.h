#ifndef GLENUMS_H
#define GLENUMS_H

//==========================================================
// En-t�tes
//==========================================================
#include <Core/Enums.h>
#include <windows.h>
#include <gl/gl.h>
#include <string>


namespace Agmd
{
    class RGLEnum
    {
    public :

        struct TPixelFmt {GLenum Format, Internal, Type, _Format;};
		
        static unsigned long BufferFlags(unsigned long flags);

        static unsigned long LockFlags(unsigned long flags);

        static GLenum Get(TMatrixType value);

		static std::string GetName(TMatrixType value);

        static TPixelFmt Get(TPixelFormat value);

        static GLenum Get(TBlend value);

        static GLenum Get(TTextureOp value);

        static GLenum Get(TTextureArg value);

        static GLenum Get(TRenderParameter value);

		static GLenum Get(TRenderMode value);

		static GLenum Get(TShaderType value);

		static GLenum Get(TAttachment value);

		static GLenum Get(TComponent value);

		static GLenum Get(TTextureType value);


    private :

        static GLenum       MatrixType[];
		static std::string  MatrixName[];
        static TPixelFmt    PixelFormat[];
        static GLenum       Blend[];
        static GLenum       TextureOp[];
        static GLenum       TextureArg[];
        static GLenum       RenderParameter[];
		static GLenum       RenderMode[];
		static GLenum       ShaderType[];
		static GLenum       AttachmentType[];
		static GLenum		ComponentType[];
		static GLenum		TextureType[];
    };

}

#endif //GLENUMS_H