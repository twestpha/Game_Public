#ifndef Debug_h
#define Debug_h

#include <cstdarg>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <queue>

class DebugConsole;

class Debug{
public:
    static void info(const char*, ...);
    static void error(const char*, ...);
    static void warning(const char*, ...);

    static std::string popMessage();
    static bool hasMessages();

    static bool is_on;
private:
    static const char* INFO;
    static const char* ERROR;
    static const char* WARNING;
    static const char* NORMAL;

    static void print(const char*, ...);

    static std::queue<std::string> messages;

};


#endif
