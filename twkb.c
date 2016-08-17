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


int more2Come=1;
sqlite3_stmt *prepared_statement;
pthread_mutex_t mutex;


static int get_blob(TWKB_BUF *tb,sqlite3_stmt *res, int icol)
{

    /*twkb-buffer*/
    uint8_t *buf;
    size_t buf_len;
    const sqlite3_blob *db_blob;

    db_blob = sqlite3_column_blob(res, icol);

    buf_len = sqlite3_column_bytes(res, icol);
    //   DEBUG_PRINT(("blob size;%d\n", buf_len));
    buf = malloc(buf_len);
    memcpy(buf, db_blob,buf_len);



    tb->start_pos = tb->read_pos=buf;
    tb->end_pos=buf+buf_len;
    //printf("allocate buffer at %p\n",tb->start_pos);
    return 0;


}
//~ void *get_and_decode(void *ts)
//~ {

//~ DEBUG_PRINT(("Entering get_and_decode\n"));
//~ int err;
//~ uint8_t *buf;
//~ size_t buf_len;
//~ const sqlite3_blob *db_blob;

//~ TWKB_PARSE_STATE *ts_l = (TWKB_PARSE_STATE*) ts;
//~ TWKB_HEADER_INFO thi;
//~ TWKB_BUF tb;
//~ BBOX bbox;

//~ ts_l->thi = &thi;
//~ ts_l->thi->bbox=&bbox;
//~ DEBUG_PRINT(("Starting reading from prepared statement %p\n",prepared_statement));
//~ while (sqlite3_step(prepared_statement))
//~ {
//~ // pthread_mutex_lock(&mutex);

//~ //more2Come=sqlite3_step(prepared_statement)==SQLITE_ROW;


//~ db_blob = sqlite3_column_blob(prepared_statement, 0);
//~ //err = sqlite3_errcode(db);
//~ //if(err)
//~ //   fprintf(stderr,"problem, %d\n",err);

//~ buf_len = sqlite3_column_bytes(prepared_statement, 0);

//~ buf = malloc(buf_len);
//~ memcpy(buf, db_blob,buf_len);

//~ //pthread_mutex_unlock(&mutex);

//~ tb.start_pos = tb.read_pos=buf;
//~ tb.end_pos=buf+buf_len;


//~ //	ts.thi->id =  sqlite3_column_int(prepared_statement, 1);


//~ tb.handled_buffer = 0;
//~ ts_l->tb=&tb;
//~ while (ts_l->tb->read_pos<ts_l->tb->end_pos)
//~ {
//~ decode_twkb(ts_l, ts_l->res_buf);


//~ }
//~ free(tb.start_pos);
//~ n++;
//~ }//while(!sqlite3_blob_reopen(db_res, n));


//~ pthread_exit(NULL);
//~ }



/*Get the twkb-buffer from SQLite and output as geoJSON*/
void *twkb_fromSQLiteBBOX(void *theL)
{
    DEBUG_PRINT(("Entering twkb_fromSQLiteBBOX\n"));
    /*twkb structures*/
    TWKB_HEADER_INFO thi;
    TWKB_PARSE_STATE ts;
    TWKB_BUF tb;
    sqlite3_stmt *prepared_statement;

    GLfloat *ext;
    BBOX bbox;
    ts.thi = &thi;
    ts.thi->bbox=&bbox;
    LAYER_RUNTIME *theLayer = (LAYER_RUNTIME *) theL;


//DEBUG_PRINT(("sqlite_error? %d\n",sqlite3_config(SQLITE_CONFIG_SERIALIZED )));


    prepared_statement = theLayer->preparedStatement;
    ext = theLayer->BBOX;
    //rc = sqlite3_exec(db, sql, callback, 0, &err_msg);


    int err = sqlite3_errcode(projectDB);
    if(err)
        fprintf(stderr,"sqlite problem, %d\n",err);

    sqlite3_bind_double(prepared_statement, 1,(float) ext[2]); //maxX
    sqlite3_bind_double(prepared_statement, 2,(float) ext[0]); //minX
    sqlite3_bind_double(prepared_statement, 3,(float) ext[3]); //maxY
    sqlite3_bind_double(prepared_statement, 4,(float) ext[1]); //minY


    DEBUG_PRINT(("1 = %f, 2 = %f, 3 = %f, 4 = %f\n", ext[2],ext[0],ext[3],ext[1]));
    //pthread_mutex_init(&mutex, NULL);
    err = sqlite3_errcode(projectDB);
    if(err)
        fprintf(stderr,"sqlite problem 2, %d\n",err);

//	 ts.res_buf = theLayer->res_buf;
    while (sqlite3_step(prepared_statement)==SQLITE_ROW)
    {
        ts.id = sqlite3_column_int(prepared_statement, 2);
        ts.styleID = sqlite3_column_int(prepared_statement, 3);
        if(get_blob(&tb,prepared_statement,0))
        {
            fprintf(stderr, "Failed to select data\n");

            sqlite3_close(projectDB);
            return NULL;
        }
        ts.tb=&tb;

        while (ts.tb->read_pos<ts.tb->end_pos)
        {
            decode_twkb(&ts, theLayer->res_buf);
        }
//printf("start free %p, n_pa = %d\n",tb.start_pos, res_buf->used_n_pa);
        free(tb.start_pos);
        if(theLayer->geometryType == POLYGONTYPE)
        {
            if(get_blob(&tb,prepared_statement,1))
            {
                fprintf(stderr, "Failed to select data\n");

                sqlite3_close(projectDB);
                return NULL;
            }
            ts.tb=&tb;

            while (ts.tb->read_pos<ts.tb->end_pos)
            {
                decode_element_array(&ts, theLayer->tri_index);
            }
//printf("start free %p, n_pa = %d\n",tb.start_pos, res_buf->used_n_pa);
            free(tb.start_pos);



        }

    }
    sqlite3_clear_bindings(prepared_statement);
    sqlite3_reset(prepared_statement);

//   sqlite3_finalize(prepared_statement);

    more2Come=1;
//pthread_exit(NULL);

    pthread_exit(NULL);
}
