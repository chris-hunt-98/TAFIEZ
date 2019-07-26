#ifndef AUTOBLOCK_H
#define AUTOBLOCK_H

#include "objectmodifier.h"

class AutoBlock: public ObjectModifier {
public:
    AutoBlock(GameObject* parent, RoomMap* room_map);
    virtual ~AutoBlock();

    std::string name();
    ModCode mod_code();
    void serialize(MapFileO& file);
    static void deserialize(MapFileI&, RoomMap*, GameObject*);

	BlockTexture texture();

	void setup_on_put(RoomMap* room_map);
	void cleanup_on_take(RoomMap* room_map);

    std::unique_ptr<ObjectModifier> duplicate(GameObject*, RoomMap*, DeltaFrame*);

private:
    // Auto needs to be able to alert the map when it is duplicated/destroyed
    RoomMap* map_;
};

#endif // AUTOBLOCK_H
