#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "reflection.h"
#include "type_declare_macro.h"

class object_t : public reflection::reflection_class_t {
public:
    virtual ~object_t() {}
};
typedef std::shared_ptr<object_t> object_ref;

#endif