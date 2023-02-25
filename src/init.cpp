#include <editor/editor.h>

#include <entities/crate.h>
#include <entities/lamp.h>
#include <entities/staticworldobject.h>
//#include <moshkis.h>
//#include <pickup.h>

namespace Editor {
    void Init() {
        RegisterEntityType (dynamic_cast<Core::SerializedEntityData*>(new Core::Crate::Data));
        RegisterEntityType (dynamic_cast<Core::SerializedEntityData*>(new Core::Lamp::Data));
        RegisterEntityType (dynamic_cast<Core::SerializedEntityData*>(new Core::StaticWorldObject::Data));
    }
}
    
