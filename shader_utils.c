/**********************************************************************
 *
 * NoTile
 *
 * NoTile is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * NoTile is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NoTile.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016 Nicklas Avén
 *
 **********************************************************************/

#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

#include "theclient.h"
/**
 * Display compilation errors from the OpenGL shader compiler
 */
void print_log(GLuint object)
{
    GLint log_length = 0;
    if (glIsShader(object)) {
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    } else if (glIsProgram(object)) {
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    } else {
        fprintf(stderr, "printlog: Not a shader or a program");
        return;
    }

    char* log = (char*)malloc(log_length);

    if (glIsShader(object))
        glGetShaderInfoLog(object, log_length, NULL, log);
    else if (glIsProgram(object))
        glGetProgramInfoLog(object, log_length, NULL, log);


    printf("Log: %s", log);
    free(log);
}

/**
 * Compile the shader from file 'filename', with error handling
 */
GLuint create_shader(const char* source, GLenum type)
{
    if (source == NULL) {
        fprintf(stderr, "Error opening %s, %s, error:", source, SDL_GetError());
        return 0;
    }
    GLuint res = glCreateShader(type);
    const GLchar* sources[] = {
#ifdef GL_ES_VERSION_2_0
        "#version 100\n"  // OpenGL ES 2.0
#else
        "#version 120\n"  // OpenGL 2.1
#endif
        ,
        source
    };
    glShaderSource(res, 2, sources, NULL);

    glCompileShader(res);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE) {
        fprintf(stderr, "Error %s, error:", source);
        print_log(res);
        glDeleteShader(res);
        return 0;
    }

    return res;
}