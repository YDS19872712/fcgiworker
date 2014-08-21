#ifndef FASTCGI_UTILS_INCLUDED
#define FASTCGI_UTILS_INCLUDED

#include <string>

#define DELETE_COPY(Class)\
    Class(const Class&) = delete;\
    Class& operator =(const Class&) = delete

#define DELETE_MOVE(Class)\
    Class(Class&&) = delete;\
    Class& operator =(Class&&) = delete
    
namespace fastcgi {
namespace utils {

    std::string escape(const std::string& value);
    
    std::string unescape(const std::string& value);
}}

#endif // FASTCGI_UTILS_INCLUDED
