#ifndef PLAYINGSTATE_H
#define PLAYINGSTATE_H




#include <unordered_map>

#include "gamestate.h"

class GameObjectArray;
class GraphicsManager;
class Room;
class GameObject;
class Player;
class MoveProcessor;

class UndoStack;
class DeltaFrame;
class DoorMoveDelta;

struct Point3;

class Door;
struct DoorTravellingObj;

class PlayingState: public GameState {
public:
    PlayingState();
    virtual ~PlayingState();
    void main_loop();
    void handle_input();
	Room* active_room();
	bool activate_room(Room*);
    bool activate_room(const std::string&);
    bool load_room(const std::string&);

    bool can_use_door(Door*, std::vector<DoorTravellingObj>&, Room**);

	void snap_camera_to_player();
	void init_player(Point3);

protected:
    std::unordered_map<std::string, std::unique_ptr<Room>> loaded_rooms_;
    Room* room_;
    Player* player_;

private:
    std::unique_ptr<GameObjectArray> objs_;
    std::unique_ptr<MoveProcessor> move_processor_;
    std::unique_ptr<UndoStack> undo_stack_;
    std::unique_ptr<DeltaFrame> delta_frame_;

    friend DoorMoveDelta;
};

#endif // PLAYINGSTATE_H
