// All of the includes required to use the jsoncpp parser

#if defined __APPLE__ && __MACH__
    #include <json/json.h>
    #include <json/value.h>
    #include <json/reader.h>

#elif defined __gnu_linux__
    #include <jsoncpp/json/json.h>
    #include <jsoncpp/json/value.h>
    #include <jsoncpp/json/reader.h>
#endif
