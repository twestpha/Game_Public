#ifndef Jsonable_h
#define Jsonable_h

class Jsonable {
public:
    Jsonable() {;}
    virtual string asJsonString() = 0;
};

#endif
