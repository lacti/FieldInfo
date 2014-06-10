#include "reflection_macro.h"
#include "reset_type_macro.h"

#define STRUCT_BEGIN(class_name)        REFLECTION_REGISTER_BEGIN(class_name)
#define STRUCT_FIELD(type, field_name)  REFLECTION_REGISTER_FIELD(type, field_name)
#define STRUCT_END()                    REFLECTION_REGISTER_END()
