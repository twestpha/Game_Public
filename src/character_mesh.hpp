#ifndef CharacterMesh_h
#define CharacterMesh_h

#include "flat_mesh.hpp"
#include "font_sheet.hpp"

class CharacterMesh : public FlatMesh {
public:

    static CharacterMesh* getInstance();

private:

    CharacterMesh();
    static CharacterMesh* instance;
};

#endif
