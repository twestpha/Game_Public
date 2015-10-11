#ifndef UnitHolder_h
#define UnitHolder_h

#include "playable.hpp"

using namespace std;

class UnitHolder {
public:
    UnitHolder();

    void addUnit(Playable& unit);

    vector<Playable>& getUnits();

    void populate(ResourceLoader& resource_loader);

private:
    vector<Playable> units;

};

#endif
