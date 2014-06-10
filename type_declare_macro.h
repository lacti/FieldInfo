#include "reset_type_macro.h"

#ifndef __DECLARE_TYPE_MACRO_DEFINED__
#define __DECLARE_TYPE_MACRO_DEFINED__
#define DECLARE_BEGIN(class_name) \
    struct class_name; \
    typedef std::shared_ptr<class_name> class_name##_ref; \
    struct class_name : public object_t {

#define DECLARE_FIELD(type, field_name) \
        type field_name;

#define DECLARE_END()   \
    };
#endif

#define STRUCT_BEGIN(class_name)        DECLARE_BEGIN(class_name)
#define STRUCT_FIELD(type, field_name)  DECLARE_FIELD(type, field_name)
#define STRUCT_END()                    DECLARE_END()
