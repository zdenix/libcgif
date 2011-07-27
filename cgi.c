//
//  cgi.c
//  Ray
//
//  Created by Denis Zavalishin on 12.06.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "cgi.h"
//#include "work.h"


cgi_g* init_cgi(FCGX_Stream *fcgi_in, FCGX_Stream *fcgi_out, FCGX_ParamArray fcgi_envp) {
    
    
    cgi_g*          destination;
    cgi_arg*        arguments;
    
    
    char*           content_length_s;
    unsigned long   content_length;
    char*           content_type;
    char*           request_uri;
    char*           request_uri_fcgx;
    int             request_uri_length;
    char*           request_uri_argstring;
    
    int             arguments_count;
    
    
    int             method_type;
    
    
    
    
    
    destination = (cgi_g*) calloc(1, sizeof(cgi_g));
    request_uri_fcgx = FCGX_GetParam("REQUEST_URI", fcgi_envp);
    request_uri = NULL;
    
    if(request_uri_fcgx == NULL) { return NULL; } 
    
    arguments_count = 0;
    arguments = NULL;
    
    
    
    content_length_s = FCGX_GetParam("CONTENT_LENGTH", fcgi_envp);
    if(content_length_s==NULL) {
        method_type = M_GET;
    }
    else {
        content_type = FCGX_GetParam("CONTENT_TYPE", fcgi_envp);
        if(content_type!=NULL) { 
            if(strstr(content_type, "multipart")==NULL) {
                method_type = M_POST;
            }
            else {
                method_type = M_POST_MULTIPART;
            }
        }
        else {
            method_type = M_POST;
        }        
        content_length = atoi(content_length_s);
    } 
    
    if(method_type == M_POST && content_length>0)
    {
        request_uri = calloc(strlen(request_uri_fcgx)+content_length+3, sizeof(char));
        
        char* post;
        post = calloc(content_length+1, sizeof(char));
        FCGX_GetStr(post, (int)content_length, fcgi_in);
        
        if(strstr(request_uri_fcgx, "?")==NULL) {
            sprintf(request_uri, "%s\?%s", request_uri_fcgx, post);                
        } else {
            sprintf(request_uri, "%s&%s", request_uri_fcgx, post);    
        }
        
    }
    else {
        request_uri = calloc(strlen(request_uri_fcgx)+1, sizeof(char));
        sprintf(request_uri, "%s", request_uri_fcgx);    
    }
    
    
    /*  Разбор аргументов из REQUEST_URI.
     *  Если REQUEST_URI не определён, то пропускается.
     *  От метода не зависит.
     *  Сработает если POST стандартный.
     */
    
    if(request_uri != NULL) {
        request_uri_length = (int) strlen(request_uri);
        request_uri_argstring = strstr(request_uri, "?");
        if(request_uri_argstring != NULL) request_uri_argstring = request_uri_argstring+1;
    }
    
    if(request_uri != NULL && request_uri_length > 3 && request_uri_argstring != NULL && strstr(request_uri_argstring, "=") != NULL) {
        
        char* a_name;
        char* a_value;
        
        
        if(strstr(request_uri_argstring, "&") == NULL && (a_value = strstr(request_uri_argstring, "=")) != NULL) {
            
            int name_length;
            name_length = (int) (strlen(request_uri_argstring)-strlen(a_value));
            a_name = calloc(name_length+1, sizeof(char));
            sprintf(a_name, "%.*s", name_length, request_uri_argstring);
            a_value = a_value + 1;
            
            arguments = calloc(2, sizeof(cgi_arg));
            arguments[0].name = (char*) calloc(strlen(a_name)+1, sizeof(char));
            arguments[0].value = (unsigned char*) calloc(strlen(a_value)+1, sizeof(unsigned char));
            memset(&arguments[1], 0, sizeof(cgi_arg));
            sprintf(arguments[0].name, "%s", a_name);
            sprintf((char*) arguments[0].value, "%s", a_value);
            
        }
        
        else if(strstr(request_uri_argstring, "&") != NULL &&  strstr(request_uri_argstring, "=") != NULL) {
            
            do {
                a_value = strstr(request_uri_argstring, "&"); 
                a_name  = strstr(request_uri_argstring, "="); 
                
                if(a_name == NULL) continue;
                
                if(arguments == NULL) {
                    arguments = (cgi_arg*) calloc(2, sizeof(cgi_arg));
                    memset(&arguments[1], 0, sizeof(cgi_arg));
                }
                else {
                    arguments = (cgi_arg*) realloc(arguments, (arguments_count+2)*sizeof(cgi_arg));
                    memset(&arguments[arguments_count+1], 0, sizeof(cgi_arg));
                }
                
                int name_length;
                int value_length;
                
                name_length = (int) (strlen(request_uri_argstring)-strlen(a_name));
                
                if(a_value != NULL) { value_length = (int)strlen(request_uri_argstring)-name_length-(int)strlen(a_value)-1; }
                else { value_length=(int)strlen(request_uri_argstring)-name_length-1; }
                
                if(value_length<1) continue;
                
                arguments[arguments_count].name = (char*) calloc(name_length+1, sizeof(char));
                arguments[arguments_count].value = (unsigned char*) calloc(value_length+1, sizeof(unsigned char));
                
                sprintf(arguments[arguments_count].name, "%.*s",name_length, request_uri_argstring);
                
                if(a_value != NULL) {
                    sprintf((char*) arguments[arguments_count].value, "%.*s", value_length,  a_value-value_length);
                } else {
                    sprintf((char*) arguments[arguments_count].value, "%.*s", value_length,  request_uri_argstring+name_length+1);
                }
                
                request_uri_argstring = request_uri_argstring+name_length+value_length+2;
                
                arguments_count++;   
                
            } while(a_value != NULL);
            
        }
        
    }  
    
    
    /*  Разбор аргументов и файлов из POST.
     *  CONTENT_LENNGTH не должен быть больше 5 мбайт.
     *  В ином случае — нужно использовать модули
     */  
    
    if(method_type == M_POST_MULTIPART && content_length>0 && content_length<5242880) {
        char* post;
        char* boundary;
        int   boundary_len;
        char* boundary_end;
        char* post_progress;
        char  post_temp[3072];
        
        post = calloc(content_length+1, sizeof(char));
        
        boundary = strstr(FCGX_GetParam("CONTENT_TYPE", fcgi_envp), "boundary=")+9;
        boundary_len = (int) strlen(boundary);
        boundary_end = (char*) calloc(strlen(boundary)+3, sizeof(char));
        
        sprintf(boundary_end, "%s--", boundary);
        
        long step;
        
        for(step = 0; step<content_length; step=step+3072) {
            FCGX_GetStr(post_temp, 3072, fcgi_in);
            strcpy(post, post_temp);
        } 
        
        post_progress = post;
        
        
        
        do {
            
            int    a_name_len;
            int    a_value_len;
            
            if(arguments == NULL) {
                arguments = (cgi_arg*) calloc(2, sizeof(cgi_arg));
                memset(&arguments[1], 0, sizeof(cgi_arg));
            }
            else {
                arguments = (cgi_arg*) realloc(arguments, (arguments_count+2)*sizeof(cgi_arg));
                memset(&arguments[arguments_count+1], 0, sizeof(cgi_arg));
            }
            
            
            post_progress = strstr(post_progress, boundary)+boundary_len;
            
            a_name_len = (int) (strlen(strstr(post_progress, "name=\""))-6-strlen(strstr(strstr(post_progress, "name=\"")+6, "\"")));
            arguments[arguments_count].name = (char*) calloc(a_name_len+1, sizeof(char));
            
            
            sprintf(arguments[arguments_count].name, "%.*s", a_name_len, strstr(post_progress, "name=\"")+6);
            
            a_value_len = (int) (strlen(strstr(post_progress, "\r\n\r\n")) - 4 - (strlen(strstr(strstr(post_progress, "\r\n\r\n")+4, boundary))+2))-2;
            arguments[arguments_count].value = (unsigned char*) calloc(a_value_len+1, sizeof(char));
            sprintf((char*)arguments[arguments_count].value, "%.*s", a_value_len, strstr(post_progress, "\r\n\r\n")+4);
            
            
            arguments_count++;
            
        } while(strlen(strstr(post_progress, boundary))!=strlen(strstr(post_progress, boundary_end)));
        
        
        
        free(post);
    }
    
    /* ========================================== */
    
    destination->arg = arguments;
    
    destination->cf_in = fcgi_in;
    destination->cf_out = fcgi_out;
    destination->cf_envp = fcgi_envp;
    
    
    
    if(request_uri != NULL) free(request_uri);
    return destination;
}

void free_cgi(cgi_g* cgig) {
    
    int arg_count;
    arg_count = 0;
    
    if(cgig->arg!=NULL) { 
        while(cgig->arg[arg_count].name) {
            free(cgig->arg[arg_count].name);
            free(cgig->arg[arg_count].value);
            arg_count++;
        }
        free(cgig->arg);
    }
}



unsigned char*  get_arg(cgi_g* cgig, const char* name) {
    
    int arg_count;
    arg_count = 0;
    
    if(cgig->arg!=NULL) { 
        while(cgig->arg[arg_count].name) {
            if(strcmp(cgig->arg[arg_count].name, name)==0) {
                return cgig->arg[arg_count].value;
            }
            arg_count++;
        }
        free(cgig->arg);
    }
    
    return NULL;
}



unsigned char* get_cookie(cgi_g* cgig, const char* name) {
    
    
    return NULL;
}

int set_cookie(cgi_g* cgig, const char* name, const char* value, time_t expires) {
    
    
    return 1;
}

int set_content_type(cgi_g* cgig, const char* content_type) {
    
    
    return 1;
}


int set_content_length(cgi_g* cgi, unsigned long content_length) {
    
    
    return 1;
}

int add_aditional_header(cgi_g* cgig, const char* header) {
    
    
    return 1;
}

void            print_header(cgi_g* cgig) {
    
    
    
}