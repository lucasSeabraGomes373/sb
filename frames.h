

#ifndef FRAME_H
#define FRAME_H

#include "byteTypes.h"
#include "constantPool.h"

typedef struct {
    byte4 pc;                  
    byte2 max_stack;           
    byte2 max_locals;          
    byte4 code_length;         
    byte1 *code;               
    int *operand_stack;        
    int *local_variables;      
    int sp;                    
} Frame;

typedef struct {
    Frame *current_frame;      
} Thread;

#endif 