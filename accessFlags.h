#ifndef ACCESS_FLAGS_H
#define ACCESS_FLAGS_H

enum access_flags {
    ACC_PUBLIC = 1,             
    ACC_PRIVATE = 2,            
    ACC_PROTECTED = 4,          
    ACC_STATIC = 8,             
    ACC_FINAL = 16,             
    ACC_SYNCHRONIZED = 32,      
    ACC_BRIDGE = 64,            
    ACC_VARARGS = 128,          
    ACC_ABSTRACT = 1024,        
    ACC_STRICT = 2048,          
    ACC_SYNTHETIC = 4096,       
    ACC_ENUM = 16384,           
    ACC_VOLATILE = 64,          
    ACC_TRANSIENT = 128         
};

#endif 