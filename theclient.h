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
 **********************************************************************

 ************************************************************************
*This is the main header file where all exposed fuctions is declared
*************************************************************************/
#ifndef _theclient_H
#define _theclient_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <time.h>
#include<pthread.h>
#include "ext/sqlite/sqlite3.h"


/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
/* Using SDL2 for the base window and OpenGL context init */
#include "SDL.h"

#define INIT_WIDTH 1000
#define INIT_HEIGHT 500

/*Maximum number of dimmensions that a twkb geoemtry
can hold according to the specification*/
#define TWKB_IN_MAXCOORDS 4

/*twkb types*/
#define	POINTTYPE			1
#define	LINETYPE			2
#define	POLYGONTYPE		3
#define	MULTIPOINTTYPE	4
#define	MULTILINETYPE		5
#define	MULTIPOLYGONTYPE	6
#define	COLLECTIONTYPE		7
#define DEFAULT_TEXT_BUF 1024

#define DEBUG 0

#if DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

#define INIT_PS_POOL_SIZE 10



/**

This is a struct keeping track of the arrays holdng vertices and vertice information
*/
typedef struct
{
    float *vertex_array; // A long list of vertices
    uint32_t *start_index; //a list of offsets where each point array starts in "vertex_array" above
    uint32_t *npoints; //a list of npoints in corresponding point array
    uint32_t total_npoints; // total npoints in all point arrays
    uint32_t used_n_pa; //used number of point arrays
    uint32_t max_pa; //max number of point arrays that we have allocated memory for in "npoints" and "start_index" lists
    float *buffer_end; //where "vertex_array" ends
    float *first_free;    //first free position in "vertex_array"
    uint32_t *polygon_offset; //a polygon can have several point arrays (if holes) so we have to keep track of where the polygon starts
    uint32_t used_n_polygon; //number of polygons that we have registered
    uint32_t max_polygon; //max number of polygons that we have allocated memory for in "used_n_polygon" and "polygon_offset" lists
    int *id; //lista med id till korresponderande pointarrays
    uint32_t *styleID; //array of styleID
}
GLESSTRUCT;

/**

This is a structsimilar to the above, but instead keeping track of arrays of indexes constructing triangels from vertex_array
*/
typedef struct
{
    GLushort *index_array; // a list of indeces for triangels
    uint32_t *start_index; //list where each point array starts (not each triangel)
    uint32_t *npoints; //number of values in "index_array"
    uint32_t total_npoints;
    uint32_t max_pa; //max number of point arrays we have allocated memory for storing info about
    uint32_t used_n_pa;
    GLushort *buffer_end;
    GLushort *first_free;
    int *id; //list if id to corresponding point array
    uint32_t *styleID; //array of styleID
}
ELEMENTSTRUCT;


/**

In runtime an array of the structure below holds all styles involved in the rendering
*/
typedef struct
{
    GLuint layerID;
    GLuint styleID;
    GLfloat color[4];
    GLfloat outlinecolor[4];
    GLfloat lineWidth;
} STYLES_RUNTIME;

/**

Information about all the layers in the project is loaded in an array of this structure at start.
*/
typedef struct
{
    char* name[63];
    uint8_t visible;
    sqlite3_stmt *preparedStatement;
    GLuint program;
    /*Placeholders in shaders*/
    GLint attribute_coord2d;
    GLint uniform_theMatrix;
    GLint uniform_color;
    /*Buffers*/
    GLuint vbo;
    GLuint ebo;
    /*Values for shaders*/
    GLfloat theMatrix[16];
    /*values for what and how to render*/
    GLfloat *BBOX;
    uint8_t geometryType;
    GLint minScale;
    GLint maxScale;
    GLESSTRUCT *res_buf;
    ELEMENTSTRUCT *tri_index;
}
LAYER_RUNTIME;



/***************************************************************
			DECODING TWKB						*/
/*Holds a buffer with the twkb-data during read*/
typedef struct
{
    uint8_t handled_buffer; /*Indicates if this program is resposible for freeing*/
    uint64_t BufOffsetFromBof;	//Only osed when reading from file
    uint8_t *start_pos;
    uint8_t *read_pos;
    uint8_t *end_pos;
    uint8_t *max_end_pos;

} TWKB_BUF;

typedef struct
{
    float bbox_min[TWKB_IN_MAXCOORDS];
    float bbox_max[TWKB_IN_MAXCOORDS];
} BBOX;


typedef struct
{
    uint8_t has_bbox;
    uint8_t has_size;
    uint8_t has_idlist;
    uint8_t has_z;
    uint8_t has_m;
    uint8_t is_empty;
    uint8_t type;

    /* Precision factors to convert ints to double */
    uint8_t n_decimals[TWKB_IN_MAXCOORDS];
    /* Precision factors to convert ints to double */
    double factors[TWKB_IN_MAXCOORDS];

    uint32_t ndims; /* Number of dimensions */
    /* An array to keep delta values from 4 dimensions */
    int64_t coords[TWKB_IN_MAXCOORDS];

    BBOX *bbox;
    size_t next_offset;
    int32_t id;
} TWKB_HEADER_INFO;

/* Used for passing the parse state between the parsing functions.*/
typedef struct
{
    TWKB_BUF *tb; /* Points to start of TWKB */
    //~ buffer_collection *rb;
    TWKB_HEADER_INFO *thi;
    GLESSTRUCT *res_buf;
    sqlite3_stmt *prepared_statement;
    uint32_t id;  //the current styleID
    uint32_t styleID;  //the current styleID
} TWKB_PARSE_STATE;



/*************Memory handling***********/


GLESSTRUCT* init_res_buf();
void destroy_buffer(GLESSTRUCT *res_buf);
int check_and_increase_max_pa(size_t needed, GLESSTRUCT *res_buf);
float* get_start(uint32_t npoints, uint8_t ndims, GLESSTRUCT *res_buf);
int set_end(uint32_t npoints, uint8_t ndims,uint32_t id, uint32_t styleID, GLESSTRUCT *res_buf);

int check_and_increase_max_polygon(uint32_t needed, GLESSTRUCT *res_buf);
int set_end_polygon( GLESSTRUCT *res_buf);


ELEMENTSTRUCT* init_element_buf();
void element_destroy_buffer(ELEMENTSTRUCT *element_buf);
int element_check_and_increase_max_pa(size_t needed, ELEMENTSTRUCT *element_buf);
GLushort* element_get_start(uint32_t npoints, uint8_t ndims, ELEMENTSTRUCT *element_buf);
int element_set_end(uint32_t npoints, uint8_t ndims,uint32_t styleID, ELEMENTSTRUCT *element_buf);

//int id; //just for debugging, remove later

/* Functions for decoding twkb*/
int read_header (TWKB_PARSE_STATE *ts);
int decode_twkb_start(uint8_t *buf, size_t buf_len);
int decode_twkb(TWKB_PARSE_STATE *old_ts,GLESSTRUCT *res_buf);
int* decode_element_array(TWKB_PARSE_STATE *old_ts, ELEMENTSTRUCT *index_buf);

/*a type holding pointers to our parsing functions*/
typedef int (*parseFunctions_p)(TWKB_PARSE_STATE*,GLESSTRUCT *res_buf);

/*Functions for decoding varInt*/
int64_t unzigzag64(uint64_t val);
uint64_t buffer_read_uvarint(TWKB_BUF *tb);
int64_t buffer_read_svarint(TWKB_BUF *tb);
uint8_t buffer_read_byte(TWKB_BUF *tb);
void buffer_jump_varint(TWKB_BUF *tb,int n);


long int getReadPos(TWKB_BUF *tb);

/*resetting GLESSTRUCT buffer*/
void reset_buffer();

/*resetting ELEMENTSTRUCT buffer*/
void element_reset_buffer();

/*Functions exposed to other programs*/
void *twkb_fromSQLiteBBOX( void *theL);
GLuint create_shader(const char* source, GLenum type);
void print_log(GLuint object);
int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave);

/*utils*/
void copyNew2CurrentBBOX(GLfloat *newBBOX,GLfloat *currentBBOX);
int multiply_matrices(GLfloat *matrix1,GLfloat *matrix2, GLfloat *theMatrix);

/*This is functions for manipulating bbox, translations and zoom*/
int px2m(GLfloat *bbox,GLint px_x,GLint px_y,GLfloat *w_x,GLfloat *w_y);
int calc_translate(GLfloat w_x,GLfloat w_y, GLfloat *transl);
int calc_scale(GLfloat *bbox, GLfloat zoom, GLfloat *scale);
void windowResize(int newWidth,int newHeight,GLfloat *currentBBOX, GLfloat *newBBOX);
void initialBBOX(GLfloat x, GLfloat y, GLfloat width, GLfloat *newBBOX);

/*event handling*/
int matrixFromBboxPointZoom(GLfloat *currentBBOX,GLfloat *newBBOX,GLfloat px_x_clicked,GLfloat px_y_clicked, GLfloat zoom, GLfloat *theMatrix);
int matrixFromDeltaMouse(GLfloat *currentBBOX,GLfloat *newBBOX,GLfloat mouse_down_x,GLfloat mouse_down_y,GLfloat mouse_up_x,GLfloat mouse_up_y, GLfloat *theMatrix);

LAYER_RUNTIME* init_layer_runtime(int n);
int  matrixFromBBOX(GLfloat *newBBOX, GLfloat *theMatrix );
int get_data(SDL_Window* window,GLfloat *bbox,GLfloat *theMatrix);

int loadPoint(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int  renderPoint(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int loadLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int  renderLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix, int outline);

int loadPolygon(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int  renderPolygon(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int render_data(SDL_Window* window,GLfloat *bbox,GLfloat *theMatrix);

/*********************** Global variables*******************************/


sqlite3 *projectDB;

int nLayers;


LAYER_RUNTIME *layerRuntime;


STYLES_RUNTIME *global_styles;
size_t length_global_styles;

int CURR_WIDTH;
int CURR_HEIGHT;
#endif