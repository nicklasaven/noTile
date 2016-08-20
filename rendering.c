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

#include "theclient.h"

int loadPoint(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{
    GLESSTRUCT *rb = oneLayer->res_buf;

    glGenBuffers(1, &(oneLayer->vbo));
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(rb->first_free-rb->vertex_array), rb->vertex_array, GL_STATIC_DRAW);

    renderPoint( oneLayer, theMatrix);
    return 0;
}



int renderPoint(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{
    uint32_t i;//, np, pi;
    GLfloat *color;
    GLenum err;
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    GLESSTRUCT *rb = oneLayer->res_buf;
    // glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(2);
    glUseProgram(oneLayer->program);
    glEnableVertexAttribArray(oneLayer->attribute_coord2d);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
//   glBindBuffer(GL_ARRAY_BUFFER, vbo_line);
    glVertexAttribPointer(
        oneLayer->attribute_coord2d, // attribute
        2,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
    );
    while ((err = glGetError()) != GL_NO_ERROR) {
        DEBUG_PRINT(("Problem1\n"));
        fprintf(stderr,"opengl error:%d\n", err);
    }
//    glUniform1fv(uniform_bbox, 4, bbox);
    glUniformMatrix4fv(oneLayer->uniform_theMatrix, 1, GL_FALSE,theMatrix );
    while ((err = glGetError()) != GL_NO_ERROR) {
        DEBUG_PRINT(("Problem 2\n"));
        fprintf(stderr,"opengl error:%d\n", err);
    }


// glUniform4fv(oneLayer->uniform_color,1,c );
//glUniform4fv(oneLayer->uniform_color,1,oneLayer->color );
//    glUniform4i(oneLayer->uniform_color,  0,1,0,0 );
    //  glUniformMatrix4fv(uniform_rw2oglw_translate, 1, GL_FALSE, transl);

    while ((err = glGetError()) != GL_NO_ERROR) {
        DEBUG_PRINT(("Problem3: %d\n", err));
        fprintf(stderr,"opengl error:%d\n", err);
    }
    //   glMultiDrawArrays(GL_LINE_STRIP, res_buf->start_index, res_buf->npoints,res_buf->used_n_pa);

    for (i=0; i<rb->used_n_pa; i++)
    {
        /*    np = *(rb->npoints+i);
            pi = *(rb->start_index+i);*/
  //      color = oneLayer->styles[*(rb->styleID+i)]->color;
          Uint32 styleID = *(rb->styleID+i);
          if(styleID<length_global_styles && global_styles[styleID].styleID == styleID)
        {
	color = global_styles[styleID].color;
	}
        glUniform4fv(oneLayer->uniform_color,1,color );
        glDrawArrays(GL_POINTS, *(rb->start_index+i), *(rb->npoints+i));
    }
    glDisableVertexAttribArray(oneLayer->attribute_coord2d);

    return 0;

}



int loadLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{
    GLESSTRUCT *rb = oneLayer->res_buf;

    glGenBuffers(1, &(oneLayer->vbo));
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(rb->first_free-rb->vertex_array), rb->vertex_array, GL_STATIC_DRAW);

    renderLine( oneLayer, theMatrix, 0);
    return 0;
}



int renderLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix, int outline)
{
    DEBUG_PRINT(("Entering renderLine\n"));
    uint32_t i;//, np, pi;
    GLfloat *color, lw;
    GLenum err;
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    GLESSTRUCT *rb = oneLayer->res_buf;

    glUseProgram(oneLayer->program);
    glEnableVertexAttribArray(oneLayer->attribute_coord2d);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
//   glBindBuffer(GL_ARRAY_BUFFER, vbo_line);
    glVertexAttribPointer(
        oneLayer->attribute_coord2d, // attribute
        2,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
    );
    while ((err = glGetError()) != GL_NO_ERROR) {
        DEBUG_PRINT(("Problem1\n"));
        fprintf(stderr,"opengl error:%d", err);
    }
//    glUniform1fv(uniform_bbox, 4, bbox);
    glUniformMatrix4fv(oneLayer->uniform_theMatrix, 1, GL_FALSE,theMatrix );
    while ((err = glGetError()) != GL_NO_ERROR) {
        DEBUG_PRINT(("Problem 2\n"));
        fprintf(stderr,"opengl error:%d", err);
    }



// glUniform4fv(oneLayer->uniform_color,1,c );
//glUniform4fv(oneLayer->uniform_color,1,oneLayer->color );
//    glUniform4i(oneLayer->uniform_color,  0,1,0,0 );
    //  glUniformMatrix4fv(uniform_rw2oglw_translate, 1, GL_FALSE, transl);

    while ((err = glGetError()) != GL_NO_ERROR) {
        DEBUG_PRINT(("Problem3: %d\n", err));
        fprintf(stderr,"opengl error:%d", err);
    }
    //   glMultiDrawArrays(GL_LINE_STRIP, res_buf->start_index, res_buf->npoints,res_buf->used_n_pa);
//glLineWidth(1.5);
    for (i=0; i<rb->used_n_pa; i++)
    {
        lw = 0;
        /*    np = *(rb->npoints+i);
            pi = *(rb->start_index+i);*/
//	style = oneLayer->styles[*(rb->styleID+i)] ;

        Uint32 styleID = *(rb->styleID+i);
        if(styleID<length_global_styles && global_styles[styleID].styleID == styleID)
        {
            if(outline)
                color = global_styles[styleID].outlinecolor;
            else
                color = global_styles[styleID].color;

            lw = global_styles[styleID].lineWidth;

        }
        else
        {
            return 1;

        }
        glLineWidth(lw);
        if(lw)
        {
            glUniform4fv(oneLayer->uniform_color,1,color );
            glDrawArrays(GL_LINE_STRIP, *(rb->start_index+i), *(rb->npoints+i));
        }
    }
    glDisableVertexAttribArray(oneLayer->attribute_coord2d);

    return 0;

}




int loadPolygon(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{
    GLESSTRUCT *rb = oneLayer->res_buf;
//	 int i,j, offset=0;
    glGenBuffers(1, &(oneLayer->vbo));
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(rb->first_free-rb->vertex_array), rb->vertex_array, GL_STATIC_DRAW);


    ELEMENTSTRUCT *ti = oneLayer->tri_index;

    //   int size =  sizeof(GLshort)*(ti->first_free-ti->vertex_array);
    glGenBuffers(1, &(oneLayer->ebo));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oneLayer->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort)*(ti->first_free-ti->index_array), ti->index_array, GL_STATIC_DRAW);
    renderPolygon( oneLayer, theMatrix);
    renderLine(oneLayer, theMatrix,1);
    return 0;
}



int renderPolygon(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{
    DEBUG_PRINT(("Entering renderPolygon\n"));
    uint32_t i;//, np, pi;
    GLfloat *color;
    GLenum err;
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    GLESSTRUCT *rb = oneLayer->res_buf;
    ELEMENTSTRUCT *ti = oneLayer->tri_index;

    glUseProgram(oneLayer->program);
    glEnableVertexAttribArray(oneLayer->attribute_coord2d);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
//   glBindBuffer(GL_ARRAY_BUFFER, vbo_line);



    for (i=0; i<ti->used_n_pa; i++)
    {
        size_t  vertex_offset = *(rb->polygon_offset+i)  ;
        glVertexAttribPointer(
            oneLayer->attribute_coord2d, // attribute
            2,                 // number of elements per vertex, here (x,y)
            GL_FLOAT,          // the type of each element
            GL_FALSE,          // take our values as-is
            0,                 // no extra data between each position
            (GLvoid*) vertex_offset                  // offset of first element
        );
        while ((err = glGetError()) != GL_NO_ERROR) {
            DEBUG_PRINT(("Problem1\n"));
            fprintf(stderr,"opengl error:%d", err);
        }


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oneLayer->ebo);
//    glUniform1fv(uniform_bbox, 4, bbox);
        glUniformMatrix4fv(oneLayer->uniform_theMatrix, 1, GL_FALSE,theMatrix );
        while ((err = glGetError()) != GL_NO_ERROR) {
            DEBUG_PRINT(("Problem 2\n"));
            fprintf(stderr,"opengl error:%d", err);
        }

        Uint32 styleID = *(ti->styleID+i);
        if(styleID<length_global_styles && global_styles[styleID].styleID == styleID)
        {
            color = global_styles[styleID].color;
        }
        else
        {
            return 1;
        }

//GLfloat color[4] = {0.5, 0.5, 0.5,0.5};
        glUniform4fv(oneLayer->uniform_color,1,color );


        while ((err = glGetError()) != GL_NO_ERROR) {
            DEBUG_PRINT(("Problem3: %d\n", err));
            fprintf(stderr,"opengl error:%d", err);
        }





        while ((err = glGetError()) != GL_NO_ERROR) {
            DEBUG_PRINT(("Problem3: %d\n", err));
            fprintf(stderr,"opengl error:%d", err);
        }
        /*    np = *(ti->npoints+i);
            pi = *(ti->start_index+i);
            for (h=0;h<*(ti->npoints+i) * 3;h++)
            {
              int idx = *(ti->vertex_array + *(ti->start_index+i) * 3 + h);
              double coord1 = *(oneLayer->res_buf->vertex_array + 2 *  idx);
              double coord2 = *(oneLayer->res_buf->vertex_array + 2 *  idx + 1 );*/
        // DEBUG_PRINT(("idx : %d, %lf, %lf\n", idx, coord1, coord2));
        //     DEBUG_PRINT(("select 'point( %lf %lf)'::geometry geom union all\n",  coord1, coord2));
        //}
        //  int start =  * (ti->start_index+i);
        size_t index_offset = *(ti->start_index+i) * 3 *sizeof(GLushort) ;
        //  offset = i * 100;
        //int n = *(ti->npoints+i) * 3 ;
        glDrawElements(GL_TRIANGLES, *(ti->npoints+i) * 3,GL_UNSIGNED_SHORT,(GLvoid*) index_offset);

    }
    glDisableVertexAttribArray(oneLayer->attribute_coord2d);

    return 0;

}