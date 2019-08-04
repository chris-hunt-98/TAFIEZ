#pragma once

#ifndef EDITORSTATE_H
#define EDITORSTATE_H

#include "editorbasestate.h"
#include "editortab.h"
#include "room.h"
#include "gameobjectarray.h"

class RoomMap;

struct EditorRoom {
    std::unique_ptr<Room> room;
    Point3 start_pos;
    Point3 cam_pos;
    bool changed;
    EditorRoom(std::unique_ptr<Room>, Point3);

    RoomMap* map();
    std::string name();
};

class EditorState: public EditorBaseState {
public:
    EditorState(GraphicsManager* gfx);
    virtual ~EditorState();
    void main_loop();

	bool handle_keyboard_input_main_state();
    void set_active_room(std::string name);
    int get_room_names(const char* room_names[]);
    EditorRoom* get_room(std::string name);

    void new_room(std::string name, int width, int height, int depth);
    bool load_room(std::string name, bool from_main);
	void load_room_from_path(std::filesystem::path path);
    void save_room(EditorRoom* eroom, bool commit);
	void set_player_state(Player* player, RoomMap* room_map);
    EditorRoom* reload(EditorRoom* eroom);
	void load_save_cycle();
    void unload_current_room();
    void commit_current_room();
    void commit_all();

	void set_active_tab_by_index(int i);
    void begin_test();

    EditorRoom* active_room_;
    EditorTab* active_tab_;

private:
    std::map<std::string, std::unique_ptr<EditorRoom>> rooms_;
    std::vector<std::pair<std::string, std::unique_ptr<EditorTab>>> tabs_;

    std::unique_ptr<GameObjectArray> objs_;

    void handle_left_click(Point3);
    void handle_right_click(Point3);
};

#endif // EDITORSTATE_H