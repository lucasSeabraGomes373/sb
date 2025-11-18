#ifndef ATTRIBUTE_STRUCTS_H
#define ATTRIBUTE_STRUCTS_H

#include "byteTypes.h"
#include "constantPool.h"

// -------------------- Estruturas de atributos genéricos --------------------

typedef struct attribute_info {
    byte2 attribute_name_index;
    byte4 attribute_length;
    void *info;
} attribute_info;
typedef struct same_frame_extended {
    byte2 offset_delta;
} same_frame_extended;


// -------------------- Campos e métodos --------------------

typedef struct field_info {
    byte2 access_flags;
    byte2 name_index;
    byte2 descriptor_index;
    byte2 attributes_count;
    attribute_info **attributes;
} field_info;

typedef struct method_info {
    byte2 access_flags;
    byte2 name_index;
    byte2 descriptor_index;
    byte2 attributes_count;
    attribute_info **attributes;
} method_info;

// -------------------- Code e exceções --------------------

typedef struct exception_table {
    byte2 start_pc;
    byte2 end_pc;
    byte2 handler_pc;
    byte2 catch_type;
} exception_table;

typedef struct code_attribute {
    byte2 max_stack;
    byte2 max_locals;
    byte4 code_length;
    byte1 *code;
    byte2 exception_table_length;
    exception_table *table;
    byte2 attributes_count;
    attribute_info **attributes;
} code_attribute;

// -------------------- LineNumberTable --------------------

typedef struct line_number_tableInfo {
    byte2 start_pc;
    byte2 line_number;
} line_number_tableInfo;

typedef struct line_number_table {
    byte2 line_number_table_length;
    line_number_tableInfo *info;
} line_number_table;

// -------------------- ConstantValue --------------------

typedef struct constantValue_attribute {
    byte2 constantvalue_index;
} constantValue_attribute;

// -------------------- Signature --------------------

typedef struct signature_attribute {
    byte2 signature_index;
} signature_attribute;

// -------------------- SourceFile --------------------

typedef struct source_file_attribute {
    byte2 source_file_index;
} source_file_attribute;

// -------------------- InnerClasses --------------------

typedef struct classes {
    byte2 inner_class_info_index;
    byte2 outer_class_info_index;
    byte2 inner_name_index;
    byte2 inner_class_access_flags;
} classes;

typedef struct innerClasses_attribute {
    byte2 number_of_classes;
    classes **classes_vector;
} innerClasses_attribute;

// -------------------- Exceptions --------------------

typedef struct exceptions_attribute {
    byte2 number_of_exceptions;
    byte2 *exception_index_table;
} exceptions_attribute;

// -------------------- StackMapTable --------------------

typedef struct object_variable_info {
    byte1 tag;
    byte2 cpool_index;
} object_variable_info;

typedef struct uninitialized_variable_info {
    byte1 tag;
    byte2 offset;
} uninitialized_variable_info;

typedef union {
    object_variable_info object_variable_info;
    uninitialized_variable_info uninitialized_variable_info;
} verification_type_info_union;

typedef struct verification_type_info {
    byte1 tag;
    verification_type_info_union type_info;
} verification_type_info;

typedef struct append_frame {
    byte2 offset_delta;
    verification_type_info **locals;
    byte1 number_of_locals;
} append_frame;

typedef struct full_frame {
    byte2 offset_delta;
    byte1 number_of_locals;
    verification_type_info **locals;
    byte1 number_of_stack_items;
    verification_type_info **stack;
} full_frame;

typedef struct same_locals_1_stack_item_frame {
    byte2 offset_delta;
    verification_type_info **stack;
} same_locals_1_stack_item_frame;

typedef struct same_locals_1_stack_item_frame_extended {
    byte2 offset_delta;
    verification_type_info **stack;
} same_locals_1_stack_item_frame_extended;

typedef struct chop_frame {
    byte2 offset_delta;
} chop_frame;

typedef union {
    same_locals_1_stack_item_frame same_locals_1_stack_item_frame;
    same_locals_1_stack_item_frame_extended same_locals_1_stack_item_frame_extended;
    chop_frame chop_frame;
    same_frame_extended same_frame_extended;
    append_frame append_frame;
    full_frame full_frame;
} map_frame_type_union;


typedef struct stack_map_frame {
    byte1 frame_type;
    map_frame_type_union map_frame_type;
} stack_map_frame;

typedef struct stackMapTable_attribute {
    byte2 number_of_entries;
    stack_map_frame **entries; // ← deve ser ponteiro para ponteiro
} stackMapTable_attribute;



#endif // ATTRIBUTE_STRUCTS_H

