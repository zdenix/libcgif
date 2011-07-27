//
//  hex.h
//  Ray
//
//  Created by Denis Zavalishin on 12.06.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef HEX_H
#define HEX_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


unsigned char* hexDecodeValue(char* value);
unsigned char* valueEncodeHex (unsigned char* value);

#endif
