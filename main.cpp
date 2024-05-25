#include <cassert>
#include <iostream>
#include "raylib/src/raylib.h"
#include "raylib/src/raymath.h"
#include "ecs.h"


float window_width = 900.f;
float window_height = 900.f;
float width = 100;
const char* window_title = "ECS";

float speed = 100.f;
float enemy_speed = 100.f;

enum Component_name {
    POSITION = 1, VELOCITY = 2, BEHAVIOUR = 4
};

enum Component_index{
    POSITION_INDEX, VELOCITY_INDEX, BEHAVIOUR_INDEX, COMPONENT_INDEX_MAX
};

enum Entity_kind {
    PLAYER, ENEMY, 
};

enum Behaviour {
    PLAYER_CONTROL, SEEK,
};

//				POSITION	VELOCITY        BEHAVIOUR	
ECS ecs(COMPONENT_INDEX_MAX, sizeof(Vector2), sizeof(Vector2), sizeof(Behaviour));

void set_components(u64 entity_id, Vector2 position, Vector2 velocity, Behaviour behaviour) {
    ecs.write_component(entity_id, POSITION_INDEX, position);    
    ecs.write_component(entity_id, VELOCITY_INDEX, velocity);    
    ecs.write_component(entity_id, BEHAVIOUR_INDEX, behaviour);    
}

void add_enemy() {
    ecs.add_entity(POSITION | VELOCITY);
    Vector2 position = {(float)GetRandomValue(0, window_width), (float)GetRandomValue(0, window_height)};
    Vector2 velocity = {.0, .0}; 
    set_components(ecs.entity_count() - 1, position, velocity, SEEK);
}

void movement_system() {   
    for(int i = 0; i < ecs.entity_count(); ++i) {
	if (!ecs.check_components(i, POSITION | VELOCITY)) continue; 
	Vector2 pos = ecs.read_component<Vector2>(i, POSITION_INDEX);
	Vector2 vel = ecs.read_component<Vector2>(i, VELOCITY_INDEX);
	vel = Vector2Scale(vel, GetFrameTime());
	pos = Vector2Add(pos, vel);
	ecs.write_component(i, POSITION_INDEX, pos);
    } 
}

void control_system() {
    int key = GetKeyPressed();
    Vector2 vel = {0};
    if (IsKeyDown(KEY_LEFT)) {
	vel = Vector2Add(vel, {-speed, 0.f});
    }
    if (IsKeyDown(KEY_RIGHT)) {
	vel = Vector2Add(vel, {speed, 0.f});
    }
    if (IsKeyDown(KEY_UP)) {
	vel = Vector2Add(vel, {0.f, -speed});
    }
    if (IsKeyDown(KEY_DOWN)) {
	vel = Vector2Add(vel, {0.f, speed});
    }
    ecs.write_component(PLAYER, VELOCITY_INDEX, vel);
    for (int i = 1; i < ecs.entity_count(); ++i) {
	if (ecs.read_component<Behaviour>(i, BEHAVIOUR_INDEX) == SEEK) { 
	    Vector2 player_pos = ecs.read_component<Vector2>(PLAYER, POSITION_INDEX);
	    Vector2 pos = ecs.read_component<Vector2>(i, POSITION_INDEX);
	    Vector2 vel = Vector2Subtract(player_pos, pos);
	    ecs.write_component(i, VELOCITY_INDEX, vel); 
	}
    }

    if (IsKeyReleased(KEY_T)) {
	add_enemy();
    }
    if (IsKeyReleased(KEY_F)) {
	add_enemy();
    }
}

int main() {
    InitWindow(window_width, window_height, window_title);
    SetTargetFPS(60);

    //Player
    ecs.add_entity(POSITION | VELOCITY);
    set_components(PLAYER, {0, 0}, {0, 0}, PLAYER_CONTROL);

    while (!WindowShouldClose()) {
	BeginDrawing();
        ClearBackground(GRAY);     
	control_system();
	movement_system();
	DrawRectangleV(ecs.read_component<Vector2>(PLAYER, POSITION_INDEX), {100, 100}, BLUE);
	for (int i = 1; i < ecs.entity_count(); ++i) {
	    DrawCircleV(ecs.read_component<Vector2>(i, POSITION_INDEX), 10.f, RED);
	}
	EndDrawing();
    }

    CloseWindow();
    return 0;
}
