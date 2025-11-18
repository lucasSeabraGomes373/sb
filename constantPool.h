#ifndef CONSTANT_POOL_H
#define CONSTANT_POOL_H

#include "byteTypes.h"

// Constant Pool structure and tags
typedef struct cp_info {
    byte1 tag;
    union {
        struct {
            byte2 name_index;
        } CONSTANT_Class;

        struct {
            byte2 class_index;
            byte2 name_and_type_index;
        } CONSTANT_Fieldref, CONSTANT_Methodref, CONSTANT_InterfaceMethodref;

        struct {
            byte4 bytes;
        } CONSTANT_Integer, CONSTANT_Float;

        struct {
            byte4 high_bytes;
            byte4 low_bytes;
        } CONSTANT_Long, CONSTANT_Double;

        struct {
            byte2 string_index;
        } CONSTANT_String;

        struct {
            byte2 name_index;
            byte2 descriptor_index;
        } CONSTANT_NameAndType;

        struct {
            byte1 reference_kind;
            byte2 reference_index;
        } CONSTANT_MethodHandle;

        struct {
            byte2 descriptor_index;
        } CONSTANT_MethodType;

        struct {
            byte2 bootstrap_method_attr_index;
            byte2 name_and_type_index;
        } CONSTANT_InvokeDynamicInfo;

        struct {
            byte2 length;
            byte1 *bytes;
        } CONSTANT_UTF8;
    } UnionCP;
} cp_info;

// Constant pool tags
enum constant_pool_tags {
    CONSTANT_Class = 7,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_String = 8,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_NameAndType = 12,
    CONSTANT_Utf8 = 1,
    CONSTANT_MethodHandle = 15,
    CONSTANT_MethodType = 16,
    CONSTANT_InvokeDynamic = 18
};

#endif // CONSTANT_POOL_H