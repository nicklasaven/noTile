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
 * Copyright (C) 2016 Nicklas Av�n
 *
 **********************************************************************/


#include "theclient.h"
#define START_MAX_N_VERTEX 1000
#define START_MAX_N_PA 100

//buffer_collection *res_buf;
GLESSTRUCT* init_res_buf()
{
    GLESSTRUCT *res_buf;
    size_t vertex_size = sizeof(float)*START_MAX_N_VERTEX; //in bytes

    res_buf = malloc(sizeof(GLESSTRUCT));
    res_buf->vertex_array=malloc(vertex_size); //Vi börjar med att göra plats för 1000 vertex-punkter så dubblar vi när behovet ökar
    res_buf->start_index = malloc(sizeof(uint32_t)*100); //Vi börjar med att göra plats för 100 point arrays
    res_buf->npoints= malloc(sizeof(uint32_t)*100); //Vi börjar med att göra plats för 100 point arrays
    res_buf->id= malloc(sizeof(uint32_t)*100); //Vi börjar med att göra plats för 100 point arrays
    res_buf->styleID= malloc(sizeof(uint32_t)*100); //Vi börjar med att göra plats för 100 point arrays
    res_buf->total_npoints = 0;
    res_buf->used_n_pa = 0;

    res_buf->max_pa=100 ;

    res_buf->polygon_offset = malloc(sizeof(size_t)*100); //Vi börjar med att göra plats för 100 point arrays
//    res_buf->npoints_polygon= malloc(sizeof(int)*100); //Vi börjar med att göra plats för 100 point arrays
    res_buf->used_n_polygon = 0;
    res_buf->max_polygon=100 ;

    res_buf->first_free=res_buf->vertex_array;
    res_buf->buffer_end = res_buf->vertex_array + START_MAX_N_VERTEX;

    //printf("buffer size = %ld\n", res_buf->vertex_array-res_buf->buffer_end);
    return res_buf;
}



float* get_start(uint32_t npoints, uint8_t ndims, GLESSTRUCT *res_buf)
{

    size_t new_size, used_n_values, old_size;
    int64_t needed_space = sizeof(float)*npoints*ndims;

    float *new_array;

    while (res_buf->buffer_end-res_buf->first_free < needed_space)
    {

        used_n_values = res_buf->first_free - res_buf->vertex_array; //number of floats
        old_size = (res_buf->buffer_end - res_buf->vertex_array); // number of floats
        new_size = old_size*2;					//number of floats
        DEBUG_PRINT(("Ok, increase space for vertex_array from  %d bytes\n",(int) old_size));
        new_array = realloc(res_buf->vertex_array,new_size*sizeof(float)); //In bytes

        if (!new_array)
            return NULL;
        res_buf->vertex_array = new_array;
        res_buf->first_free = res_buf->vertex_array + used_n_values;
        res_buf->buffer_end = res_buf->vertex_array + new_size;
        //printf("max_va increased to %ld from va_buf %p\n",(res_buf->buffer_end - res_buf->vertex_array),res_buf->vertex_array);
    }

    return res_buf->first_free;
}


int check_and_increase_max_pa(size_t needed, GLESSTRUCT *res_buf)
{
    while (res_buf->max_pa - res_buf->used_n_pa < needed)
    {

        res_buf->max_pa*=2;
        size_t new_size = res_buf->max_pa*sizeof(int);
        DEBUG_PRINT(("Ok, increase max_pa from %d points\n",res_buf->max_pa));
        res_buf->start_index = realloc(res_buf->start_index,new_size);
        if (!res_buf->start_index)
            return 1;
        DEBUG_PRINT(("res_buf->start_index - OK \n"));
        res_buf->npoints = realloc(res_buf->npoints,new_size);
        if (!res_buf->npoints)
            return 1;
        DEBUG_PRINT(("res_buf->npoints - OK \n"));
        res_buf->id = realloc(res_buf->id,new_size);
        if (!res_buf->id)
            return 1;
        DEBUG_PRINT(("res_buf->id - OK \n"));
        res_buf->styleID = realloc(res_buf->styleID,new_size);
        if (!res_buf->styleID)
            return 1;
        DEBUG_PRINT(("res_buf->start_index - OK \n"));
        //printf("max_pa increased to %d\n",res_buf->max_pa);
    }

    return 0;
}




int set_end(uint32_t npoints, uint8_t ndims,uint32_t id, uint32_t styleID, GLESSTRUCT *res_buf)
{

    //size_t used_n_values = res_buf->first_free - res_buf->vertex_array; //total number of floats
    *(res_buf->start_index + res_buf->used_n_pa) =res_buf->total_npoints; //preceeding number of points in array (gives startposition for VBO to OpenGL)
    *(res_buf->npoints + res_buf->used_n_pa) = npoints; // Number of points in current point array
    *(res_buf->id + res_buf->used_n_pa) = id; // Number of points in current point array
    *(res_buf->styleID + res_buf->used_n_pa) = styleID; // Number of points in current point array
    (res_buf->used_n_pa)++;	//Number of point arrays stored
    res_buf->first_free += npoints*ndims; //advance first free position for comming point arrays
    res_buf->total_npoints += npoints;  //add npoints to total number of points in whole VBO
    return 0;
}

int check_and_increase_max_polygon(uint32_t needed, GLESSTRUCT *res_buf)
{
    while (res_buf->max_polygon - res_buf->used_n_polygon < needed)
    {
        DEBUG_PRINT(("Ok, increase space triangel_indexes from %d \n",res_buf->max_polygon));
        res_buf->max_polygon*=2;

        res_buf->polygon_offset = realloc(res_buf->polygon_offset,res_buf->max_polygon*sizeof(uint32_t));
        if (!res_buf->polygon_offset)
            return 1;
        //printf("max_pa increased to %d\n",res_buf->max_pa);
    }

    return 0;
}


int set_end_polygon(GLESSTRUCT *res_buf)
{
    size_t used_n_values = res_buf->first_free - res_buf->vertex_array;

    *(res_buf->polygon_offset + res_buf->used_n_polygon) = used_n_values * sizeof(GLfloat);

    (res_buf->used_n_polygon)++;
    return 0;
}



void reset_buffer(GLESSTRUCT *res_buf)
{
    res_buf->used_n_pa = 0;
    res_buf->used_n_polygon = 0;
    res_buf->total_npoints = 0;
    res_buf->first_free=res_buf->vertex_array;
}

void destroy_buffer(GLESSTRUCT *res_buf)
{
    free(res_buf->vertex_array);
    free(res_buf->start_index);
    free(res_buf->npoints);
    free(res_buf->id);
    free(res_buf->polygon_offset);
    free(res_buf->styleID);
    free(res_buf);
    return;
}


ELEMENTSTRUCT* init_element_buf()
{
    ELEMENTSTRUCT *element_buf;
    size_t vertex_size = sizeof(GLushort)*START_MAX_N_VERTEX; //in bytes

    element_buf = malloc(sizeof(ELEMENTSTRUCT));
    element_buf->index_array=malloc(vertex_size); //Vi börjar med att göra plats för 1000 vertex-punkter så dubblar vi när behovet ökar
    element_buf->start_index = malloc(sizeof(int)*100); //Vi börjar med att göra plats för 100 point arrays
    element_buf->npoints= malloc(sizeof(int)*100); //Vi börjar med att göra plats för 100 point arrays
    element_buf->id= malloc(sizeof(int)*100); //Vi börjar med att göra plats för 100 point arrays
    element_buf->styleID= malloc(sizeof(int)*100); //Vi börjar med att göra plats för 100 point arrays
    element_buf->total_npoints = 0;
    element_buf->used_n_pa = 0;

    element_buf->max_pa=100 ;


    element_buf->first_free=element_buf->index_array;
    element_buf->buffer_end = element_buf->index_array + START_MAX_N_VERTEX;

    //printf("buffer size = %ld\n", res_buf->index_array-res_buf->buffer_end);
    return element_buf;
}



GLushort* element_get_start(uint32_t npoints, uint8_t ndims, ELEMENTSTRUCT *element_buf)
{

    size_t new_size, used_n_values, old_size;
    int64_t needed_space = sizeof(GLushort)*npoints*ndims;

    GLushort *new_array;

    while (element_buf->buffer_end-element_buf->first_free < needed_space)
    {
        used_n_values = element_buf->first_free - element_buf->index_array; //number of floats
        old_size = (element_buf->buffer_end - element_buf->index_array); // number of floats
        new_size = old_size*2;					//number of floats
        DEBUG_PRINT(("Ok, increase space for tri_indexes from  %d bytes\n",(int) old_size));
        new_array = realloc(element_buf->index_array,new_size*sizeof(GLushort)); //In bytes

        if (!new_array)
            return NULL;
        element_buf->index_array = new_array;
        element_buf->first_free = element_buf->index_array + used_n_values;
        element_buf->buffer_end = element_buf->index_array + new_size;
    }

    return element_buf->first_free;
}


int element_check_and_increase_max_pa(size_t needed, ELEMENTSTRUCT *element_buf)
{
    while (element_buf->max_pa - element_buf->used_n_pa < needed)
    {
        DEBUG_PRINT(("Ok, increase space for number of tri_indexes from   %d \n",element_buf->max_pa));
        element_buf->max_pa*=2;
        element_buf->start_index = realloc(element_buf->start_index,element_buf->max_pa*sizeof(uint32_t));
        if (!element_buf->start_index)
            return 1;
        element_buf->npoints = realloc(element_buf->npoints,element_buf->max_pa*sizeof(uint32_t));
        if (!element_buf->npoints)
            return 1;
        element_buf->id = realloc(element_buf->id,element_buf->max_pa*sizeof(uint32_t));
        if (!element_buf->id)
            return 1;
        element_buf->styleID = realloc(element_buf->styleID,element_buf->max_pa*sizeof(uint32_t));
        if (!element_buf->styleID)
            return 1;
        //printf("max_pa increased to %d\n",res_buf->max_pa);
    }

    return 0;
}




int element_set_end(uint32_t npoints, uint8_t ndims,uint32_t styleID, ELEMENTSTRUCT *element_buf)
{

    //size_t used_n_values = res_buf->first_free - res_buf->index_array; //total number of floats
    *(element_buf->start_index + element_buf->used_n_pa) =element_buf->total_npoints; //preceeding number of points in array (gives startposition for VBO to OpenGL)
    *(element_buf->npoints + element_buf->used_n_pa) = npoints; // Number of points in current point array
    *(element_buf->styleID + element_buf->used_n_pa) = styleID; // Number of points in current point array
    (element_buf->used_n_pa)++;	//Number of point arrays stored
    element_buf->first_free += npoints*ndims; //advance first free position for comming point arrays
    element_buf->total_npoints += npoints;  //add npoints to total number of points in whole VBO
    return 0;
}

void element_reset_buffer(ELEMENTSTRUCT *element_buf)
{
    element_buf->used_n_pa = 0;
    element_buf->total_npoints = 0;
    element_buf->first_free=element_buf->index_array;
}

void element_destroy_buffer(ELEMENTSTRUCT *element_buf)
{
    free(element_buf->index_array);
    free(element_buf->start_index);
    free(element_buf->npoints);
    free(element_buf->id);
    free(element_buf->styleID);
    free(element_buf);
    return;
}


long int getReadPos(TWKB_BUF *tb)
{
    return tb->BufOffsetFromBof + tb->read_pos - tb->start_pos;
}




