#include "stdafx.h"
#include "delta.h"

#include "gameobject.h"
#include "room.h"
#include "roommap.h"

#include "car.h"
#include "snakeblock.h"
#include "player.h"
#include "switch.h"
#include "switchable.h"
#include "signaler.h"
#include "playingstate.h"
#include "gatebody.h"

Delta::~Delta() {}

DeltaFrame::DeltaFrame(): deltas_ {}, changed_ {false} {}

DeltaFrame::~DeltaFrame() {}

void DeltaFrame::revert() {
    for (auto it = deltas_.rbegin(); it != deltas_.rend(); ++it) {
        (**it).revert();
    }
}

void DeltaFrame::push(std::unique_ptr<Delta> delta) {
    deltas_.push_back(std::move(delta));
    changed_ = true;
}

bool DeltaFrame::trivial() {
    return deltas_.empty();
}

void DeltaFrame::reset_changed() {
    changed_ = false;
}

bool DeltaFrame::changed() {
    return changed_;
}


UndoStack::UndoStack(unsigned int max_depth): frames_ {}, max_depth_ {max_depth}, size_ {0} {}

UndoStack::~UndoStack() {}

void UndoStack::push(std::unique_ptr<DeltaFrame> delta_frame) {
    if (!delta_frame->trivial()) {
        if (size_ == max_depth_) {
            frames_.pop_front();
        } else {
            ++size_;
        }
        frames_.push_back(std::move(delta_frame));
    }
}

bool UndoStack::non_empty() {
    return size_ > 0;
}

void UndoStack::pop() {
    frames_.back()->revert();
    frames_.pop_back();
    --size_;
}

void UndoStack::reset() {
    frames_.clear();
    size_ = 0;
}


CreationDelta::CreationDelta(GameObject* obj, RoomMap* room_map): obj_ {obj}, map_ {room_map} {}

CreationDelta::~CreationDelta() {}

void CreationDelta::revert() {
    map_->uncreate(obj_);
}


DeletionDelta::DeletionDelta(GameObject* obj, RoomMap* room_map):
obj_ {obj}, map_ {room_map} {}

DeletionDelta::~DeletionDelta() {}

void DeletionDelta::revert() {
    map_->undestroy(obj_);
}


AbstractCreationDelta::AbstractCreationDelta(GameObject* obj, RoomMap* room_map): obj_ {obj}, map_ {room_map} {}

AbstractCreationDelta::~AbstractCreationDelta() {}

void AbstractCreationDelta::revert() {
    map_->uncreate_abstract(obj_);
}


PutDelta::PutDelta(GameObject* obj, RoomMap* room_map):
obj_ {obj}, map_ {room_map} {}

PutDelta::~PutDelta() {}

void PutDelta::revert() {
    map_->take_real(obj_, nullptr);
}


// NOTE: A Taken (intangible) object won't have its position updated
// until it has been Put back into the map, so there's no need
// to record its old position in the delta.
TakeDelta::TakeDelta(GameObject* obj, RoomMap* room_map):
obj_ {obj}, map_ {room_map} {}

TakeDelta::~TakeDelta() {}

void TakeDelta::revert() {
    map_->put_real(obj_, nullptr);
}


MotionDelta::MotionDelta(GameObject* obj, Point3 dpos, RoomMap* room_map):
obj_ {obj}, dpos_ {dpos}, map_ {room_map} {}

MotionDelta::~MotionDelta() {}

void MotionDelta::revert() {
    map_->just_shift(obj_, -dpos_);
}


BatchMotionDelta::BatchMotionDelta(std::vector<GameObject*> objs, Point3 dpos, RoomMap* room_map):
objs_ {objs}, dpos_ {dpos}, map_ {room_map} {}

BatchMotionDelta::~BatchMotionDelta() {}

void BatchMotionDelta::revert() {
    map_->just_batch_shift(objs_, -dpos_);
}


AbstractShiftDelta::AbstractShiftDelta(GameObject* obj, Point3 dpos):
obj_ {obj}, dpos_ {dpos} {}

AbstractShiftDelta::~AbstractShiftDelta() {}

void AbstractShiftDelta::revert() {
    obj_->pos_ -= dpos_;
}


AbstractPutDelta::AbstractPutDelta(GameObject* obj, Point3 pos) :
	obj_{ obj }, pos_{ pos } {}

AbstractPutDelta::~AbstractPutDelta() {}

void AbstractPutDelta::revert() {
	obj_->pos_ = pos_;
}


AddLinkDelta::AddLinkDelta(SnakeBlock* a, SnakeBlock* b): a_ {a}, b_ {b} {}

AddLinkDelta::~AddLinkDelta() {}

void AddLinkDelta::revert() {
    a_->remove_link_quiet(b_);
}


RemoveLinkDelta::RemoveLinkDelta(SnakeBlock* a, SnakeBlock* b): a_ {a}, b_ {b} {}

RemoveLinkDelta::~RemoveLinkDelta() {}

void RemoveLinkDelta::revert() {
    a_->add_link_quiet(b_);
}


RoomChangeDelta::RoomChangeDelta(PlayingState* state, Room* room):
state_ {state}, room_ {room} {}

RoomChangeDelta::~RoomChangeDelta() {}

//TODO: keep some local state in the DeltaFrame? If so, this also changes *that* Room
void RoomChangeDelta::revert() {
    state_->activate_room(room_);
}


SwitchableDelta::SwitchableDelta(Switchable* obj, int count, bool active, bool waiting):
	obj_{ obj }, count_{ count }, active_{ active }, waiting_{ waiting } {}

SwitchableDelta::~SwitchableDelta() {}

void SwitchableDelta::revert() {
	obj_->count_ = count_;
    obj_->active_ = active_;
    obj_->waiting_ = waiting_;
}


SwitchToggleDelta::SwitchToggleDelta(Switch* obj): obj_ {obj} {}

SwitchToggleDelta::~SwitchToggleDelta() {}

void SwitchToggleDelta::revert() {
    obj_->toggle(false);
}


SignalerCountDelta::SignalerCountDelta(Signaler* sig, int count) : sig_{ sig }, count_{ count } {}

SignalerCountDelta::~SignalerCountDelta() {}

void SignalerCountDelta::revert() {
    sig_->reset_count(count_);
}


RidingStateDelta::RidingStateDelta(Player* player, Car* car, RidingState state):
	player_{ player }, car_{ car }, state_{ state } {}

RidingStateDelta::~RidingStateDelta() {}

void RidingStateDelta::revert() {
    player_->state_ = state_;
	player_->car_ = car_;
}


ColorChangeDelta::ColorChangeDelta(Car* car, bool undo): car_ {car}, undo_ {undo} {}

ColorChangeDelta::~ColorChangeDelta() {}

void ColorChangeDelta::revert() {
    car_->cycle_color(undo_);
}


GatePosDelta::GatePosDelta(GateBody* gate_body, Point3 dpos):
gate_body_ {gate_body}, dpos_ {dpos} {}

GatePosDelta::~GatePosDelta() {}

void GatePosDelta::revert() {
    gate_body_->gate_pos_ -= dpos_;
}
