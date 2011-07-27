//
//  cgi.h
//  Ray
//
//  Created by Denis Zavalishin on 12.06.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef CGI_H
#define CGI_H

#define M_GET               0
#define M_POST              1
#define M_POST_MULTIPART    2

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcgi_stdio.h>
#include <fcgiapp.h>

#include "hex.h"

typedef struct {
    
    char* name;
    unsigned char* value;
    time_t expires;
    
} cgi_cookie;


typedef struct {
    
    char* name;
    unsigned char* value;
    
} cgi_arg;

typedef struct {
    
    char* name;
    char* tmp_path;
    
} cgi_file;


typedef struct {
    
    char* content_type;
    unsigned long content_length;
    char* language;
    char* additional_header;
    cgi_cookie* cookie;
    
} cgi_out;

typedef union {
    
    cgi_cookie*     cookie;
    cgi_file*       file;
    cgi_arg*        arg;
    cgi_out*        outcgi;
    
    FCGX_Stream *cf_in, *cf_out;
    FCGX_ParamArray cf_envp;
    
} cgi_g;



cgi_g* init_cgi(FCGX_Stream *fcgi_in, FCGX_Stream *fcgi_out, FCGX_ParamArray fcgi_envp);
void   free_cgi(cgi_g* cgig);

unsigned char*  get_arg(cgi_g* cgig, const char* name);
unsigned char*  get_cookie(cgi_g* cgig, const char* name);
int             set_cookie(cgi_g* cgig, const char* name, const char* value, time_t expires);

int             set_content_type(cgi_g* cgig, const char* content_type);
int             set_content_length(cgi_g* cgig, unsigned long content_length);
int             add_aditional_header(cgi_g* cgig, const char* header);

void            print_header(cgi_g* cgig);


#endif