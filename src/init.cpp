#include <editor/editor.h>

#include <entities/crate.h>
#include <entities/marker.h>
#include <entities/lamp.h>
#include <entities/staticworldobject.h>
//#include <moshkis.h>
//#include <pickup.h>
#include <door.h>
#include <trigger.h>
#include <pickup.h>
#include <crab.h>
#include <frog.h>

namespace Editor {
    void Init() {
        RegisterEntityType (dynamic_cast<tram::SerializedEntityData*>(new tram::Crate::Data));
        RegisterEntityType (dynamic_cast<tram::SerializedEntityData*>(new tram::Marker::Data));
        RegisterEntityType (dynamic_cast<tram::SerializedEntityData*>(new tram::Lamp::Data));
        RegisterEntityType (dynamic_cast<tram::SerializedEntityData*>(new tram::StaticWorldObject::Data));
        
        RegisterEntityType (dynamic_cast<tram::SerializedEntityData*>(new Door::Data));
        RegisterEntityType (dynamic_cast<tram::SerializedEntityData*>(new Trigger::Data));
        RegisterEntityType (dynamic_cast<tram::SerializedEntityData*>(new Pickup::Data));
        RegisterEntityType (dynamic_cast<tram::SerializedEntityData*>(new Crab::Data));
        RegisterEntityType (dynamic_cast<tram::SerializedEntityData*>(new Frog::Data));
    }
}
    
