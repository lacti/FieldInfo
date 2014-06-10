#ifndef __REFLECTION_MACRO_H__
#define __REFLECTION_MACRO_H__

#include "reflection.h"
#include "macro_helper.h"

#define REFLECTION_REGISTER_BEGIN(class_name)  \
    static struct _register_##class_name : public reflection::reflection_register_helper_t<class_name> { \
       _register_##class_name() \
        { \
            register_class(#class_name);

#define REFLECTION_REGISTER_FIELD(type, field_name) \
            register_field<type>(&target_type::field_name, #field_name);

#define REFLECTION_REGISTER_END()  \
        } \
    } __AUTO_NAME;

#endif