#ifndef __MACRO_HELPER_H__
#define __MACRO_HELPER_H__

#define __CONCAT(x, y)      x##y
#define __CONCAT2(x, y)     __CONCAT(x, y)
#define __AUTO_NAME         __CONCAT2(__auto_name, __COUNTER__)

#endif