

template <class T> inline void GLContext::LoadExtension(T& Proc, const char* Name)
{
    Proc = reinterpret_cast<T>(wglGetProcAddress(Name));
}

inline void GLContext::InitOpenGL()
{
    LOAD_EXTENSION(wglCreateContextARB);
}

inline void GLContext::LoadFunction(const char* fname)
{
    LOAD_EXTENSION(fname);
}

inline void GLContext::LoadAllFunction()
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
}