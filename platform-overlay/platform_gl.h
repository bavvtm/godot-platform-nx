#pragma once

#ifndef GL_API_ENABLED
#define GL_API_ENABLED // allow using desktop GL.
#endif

#ifndef GLES_API_ENABLED
#define GLES_API_ENABLED // allow using GLES.
#endif

#ifndef GLAD_GLES2
#define GLAD_GLES2
#endif

#ifndef GLAD_GLES3
#define GLAD_GLES3
#endif

#include <thirdparty/glad/glad/gl.h>
