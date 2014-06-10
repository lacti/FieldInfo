// FieldInfo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "object.h"
#include "user.h"

using namespace reflection;

#include "type_register_macro.h"
#include "user.h"

int _tmain(int argc, _TCHAR* argv[])
{
    const class_t* clazz = class_t::from_name("user_t");
    std::cout << clazz->name() << std::endl;

    clazz->enumerate_fields([&] (const field_t* field) {
        std::cout << "\t" << field->name() << std::endl;
    });

    object_ref ptr(clazz->new_instance<object_t>());
    assert(clazz == ptr->get_class());

    user_t_ref user = std::dynamic_pointer_cast<user_t>(ptr);
    clazz->get_field("index")->ref<int>(ptr.get()) = 100;
    clazz->get_field("name")->ref<std::string>(ptr.get()) = "hello";
    assert(user->index == 100);
    assert(user->name == "hello");

	return 0;
}

