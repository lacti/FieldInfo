#ifndef __REFLECTION_H__
#define __REFLECTION_H__

namespace reflection { ;

class field_t;
class class_t;

template <typename _Ty>
class typeinfo {};

class reflection_base {
public:
    static reflection_base& instance();
    const class_t* get_class(std::string name) const;
    const field_t* get_field(std::string class_name, std::string field_name) const;

public:
    void add_class(std::string name, class_t* class_impl);
    void add_field(std::string class_name, std::string field_name, field_t* field_impl);

    void add_class_name(const std::type_info* typeinfo, std::string class_name);
    const char* class_name_from_typeinfo(const std::type_info* typeinfo) const;

private:
    reflection_base() {}
    reflection_base(const reflection_base&);
    reflection_base& operator= (const reflection_base&);

private:
    typedef std::map<const std::type_info*, std::string> class_name_map_t;
    typedef std::map<std::string, class_t*> class_map_t;

    class_name_map_t class_name_map;
    class_map_t class_map;
};

class class_t {
public:
    template <typename _Ty>
    class_t(const typeinfo<_Ty>&, std::string _name);
    const char* name() const { return class_name.c_str(); }

    const field_t* get_field(std::string name) const;
    void add_field(std::string name, field_t* field);

    template <typename _Func>
    void enumerate_fields(_Func&& func) const;

    template <typename _Class>
    _Class* new_instance() const;

    static const class_t* from_name(std::string name);

private:
    struct interface_t {
        virtual void* new_instance() const = 0;
    };
    template <typename _Ty>
    struct impl_t : public interface_t {
        virtual void* new_instance() const { return new _Ty; }
    };

private:
    std::shared_ptr<interface_t> inf;
    std::string class_name;

    typedef std::map<std::string, field_t*> field_map_t;
    field_map_t field_map;
};

class reflection_class_t {
public:
    virtual ~reflection_class_t() {}

    const char* type_name() const;
    const char* class_name() const;
    const class_t* get_class() const;
};

class field_t {
public:
    template <typename _ObjTy, typename _FieldTy>
    field_t(_FieldTy (_ObjTy::*Field), std::string _name);
    const char* name() const { return field_name.c_str(); }

    template <typename _FieldTy>
    _FieldTy& ref(void* obj) const;
    
private:
    struct interface_t {
        virtual void* ptr(void* obj_addr) const = 0;
        virtual const std::type_info& type() const = 0;
    };

    template <typename _ObjTy, typename _FieldTy>
    struct impl_t : public interface_t {
        impl_t(_FieldTy (_ObjTy::*Field)) : field(Field) {}
        virtual const std::type_info& type() const { return typeid(_FieldTy); }
        virtual void* ptr(void* obj_addr) const;

        _FieldTy (_ObjTy::*field);
    };

private:
    std::shared_ptr<interface_t> inf;
    std::string field_name;
};

// reflection_base
inline reflection_base& reflection_base::instance()
{
    static reflection_base impl;
    return impl;
}
    
inline const class_t* reflection_base::get_class(std::string name) const
{
    auto class_iter = class_map.find(name);
    if (class_iter == class_map.end())
        return nullptr;
    return class_iter->second;
}

inline const field_t* reflection_base::get_field(std::string class_name, std::string field_name) const
{
    return get_class(class_name)->get_field(field_name);
}

inline void reflection_base::add_class(std::string name, class_t* class_impl)
{
    auto result = class_map.insert(std::make_pair(name, class_impl));
    assert(result.second);
}

inline void reflection_base::add_field(std::string class_name, std::string field_name, field_t* field_impl)
{
    class_map[class_name]->add_field(field_name, field_impl);
}

inline void reflection_base::add_class_name(const std::type_info* typeinfo, std::string class_name)
{
    auto result = class_name_map.insert(std::make_pair(typeinfo, class_name));
    assert(result.second);
}

inline const char* reflection_base::class_name_from_typeinfo(const std::type_info* typeinfo) const
{
    auto name_iter = class_name_map.find(typeinfo);
    if (name_iter == class_name_map.end())
        return nullptr;
    return name_iter->second.c_str();
}

// class_t
template <typename _Ty>
inline class_t::class_t(const typeinfo<_Ty>&, std::string _name)
    : class_name(_name), inf(new impl_t<_Ty>)
{
}

inline const field_t* class_t::get_field(std::string name) const
{
    auto field_iter = field_map.find(name);
    if (field_iter == field_map.end())
        return nullptr;
    return field_iter->second;
}

template <typename _Class>
inline _Class* class_t::new_instance() const
{
    return reinterpret_cast<_Class*>(inf->new_instance());
}

inline const class_t* class_t::from_name(std::string name)
{
    return reflection_base::instance().get_class(name);
}

inline void class_t::add_field(std::string name, field_t* field)
{
    auto result = field_map.insert(std::make_pair(name, field));
    assert(result.second);
}

template <typename _Func>
inline void class_t::enumerate_fields(_Func&& func) const
{
    std::for_each(field_map.begin(), field_map.end(), [&] (field_map_t::value_type pair) {
        func(pair.second);
    });
}

// reflection_class_t
inline const char* reflection_class_t::type_name() const
{
    return typeid(*this).name();
}

inline const char* reflection_class_t::class_name() const
{
    return reflection_base::instance().class_name_from_typeinfo(&typeid(*this));
}

inline const class_t* reflection_class_t::get_class() const
{
    return class_t::from_name(class_name());
}

// field_t
template <typename _ObjTy, typename _FieldTy>
inline field_t::field_t(_FieldTy (_ObjTy::*Field), std::string _name)
    : field_name(_name), inf(new impl_t<_ObjTy, _FieldTy>(Field))
{
}

template <typename _FieldTy>
inline _FieldTy& field_t::ref(void* obj) const
{
    _FieldTy* address = reinterpret_cast<_FieldTy*>(inf->ptr(obj));
    return *address;
}

template <typename _ObjTy, typename _FieldTy>
inline void* field_t::impl_t<_ObjTy, _FieldTy>::ptr(void* obj_addr) const
{
    _ObjTy* obj = reinterpret_cast<_ObjTy*>(obj_addr);
    return &(obj->*field);
}

// register helper
template <typename _ObjTy>
struct reflection_register_helper_t {
    typedef _ObjTy target_type;

    static void register_class(std::string class_name);
    template <typename _FieldTy>
    static void register_field(_FieldTy (_ObjTy::*Field), std::string field_name);
};

template <typename _ObjTy>
inline void reflection_register_helper_t<_ObjTy>::register_class(std::string class_name)
{
    reflection_base::instance().add_class_name(&typeid(_ObjTy), class_name);
    reflection_base::instance().add_class(class_name, new class_t(typeinfo<_ObjTy>(), class_name));
}

template <typename _ObjTy>
template <typename _FieldTy>
inline void reflection_register_helper_t<_ObjTy>::register_field(_FieldTy (_ObjTy::*Field), std::string field_name)
{
    const char* class_name = reflection_base::instance().class_name_from_typeinfo(&typeid(_ObjTy));
    assert(class_name);

    reflection_base::instance().add_field(class_name, field_name, new field_t(Field, field_name));
}

}

#endif