//
//  hex.c
//  Ray
//
//  Created by Denis Zavalishin on 12.06.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "hex.h"


unsigned char* hexDecodeValue(char* value) {
 	
 	if(value==NULL) { return NULL; }
 	
 	unsigned char* destination;
 	destination = calloc(strlen(value), sizeof(unsigned char));
 	
 	int currentPosition;
 	
 	for(currentPosition=0; currentPosition<strlen(value); currentPosition++) {
 		if(value[currentPosition]=='+') {
 			sprintf((char*)destination, "%s ", destination);
 		}
 		else if(value[currentPosition]=='%') {
 			
 			char fhex[5];
 			fhex[0] = '0';
 			fhex[1] = 'x';
 			fhex[2] = value[currentPosition+1];
 			fhex[3] = value[currentPosition+2];
 			fhex[4] = 0;
 			currentPosition=currentPosition+2;
 			sprintf((char*)destination, "%s%c", destination, (char) strtol(fhex, NULL, 16)); 				
 			
 		}	
 		else {
 			sprintf((char*)destination,"%s%c", destination, value[currentPosition]);
 		}
 		
 	}
	if(strlen((const char*)destination)==0) {
		free(destination);
		return NULL;	
	}
	
	//destination = realloc(destination, strlen(destination)); 	
    
    return destination;	
}


unsigned char* valueEncodeHex(unsigned char* value) {
	
	unsigned char* destination;
	destination = (unsigned char*) calloc(strlen((const char*)value)*3, sizeof(char));
	strcpy((char*)destination, "");
    
	int currentPosition;
	
	for(currentPosition=0; currentPosition<strlen((const char*)value); currentPosition++) {
        
		if((value[currentPosition]>='0' && value[currentPosition]<='9')  ||
           (value[currentPosition]>='A' && value[currentPosition]<='Z') ||
           (value[currentPosition]>='a' && value[currentPosition]<='z'))
        {
            
            sprintf((char*)destination, "%s%c", destination, value[currentPosition]);
		   	
        }
	 	else {
            
            sprintf((char*)destination, "%s%c%02X",destination, '%', value[currentPosition]);
        } 
        
	}
	
	//destination = realloc(destination, strlen((const char*)destination));
    
	return destination;
}