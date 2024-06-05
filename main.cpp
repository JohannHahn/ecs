#include <cassert>
#include <cstring>
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

enum Component_Flag{
    POSITION_FLAG = 1, VELOCITY_FLAG = 2, BEHAVIOUR_FLAG = 4
};

enum Component_Index{
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

void remove_enemy(u64 entity_id = 0) {
    if (ecs.entity_count() == 1) {
	std::cout << "nothing to delete\n";
	return;
    }
    if (entity_id == PLAYER) {
	assert(0 && "trying to delete player in remove_enemy");
    }
    std::cout << "entity_id = " << entity_id << "\n";
    ecs.remove_entity(entity_id);
}

void add_enemy() {
    u64 entity_id = ecs.add_entity(POSITION_FLAG | VELOCITY_FLAG);
    std::cout << "added entity with id = " << entity_id << "\n";
    Vector2 position = {entity_id * 100.f, window_height / 2.f};
    Vector2 velocity = {.0, .0}; 
    set_components(entity_id, position, velocity, SEEK);
}

void movement_system() {   
    for(int i = 0; i < ecs.entity_count(); ++i) {
	if (!ecs.check_components(i, POSITION_FLAG | VELOCITY_FLAG)) continue; 
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
	    //ecs.write_component(i, VELOCITY_INDEX, vel); 
	}
    }

    if (IsKeyReleased(KEY_T)) {
	add_enemy();
    }
    if (IsKeyReleased(KEY_F)) {
	remove_enemy(1);
    }
}

void draw() {
    Vector2 player_pos = ecs.read_component<Vector2>(PLAYER, POSITION_INDEX);
    DrawRectangleV(Vector2AddValue(player_pos, -0.5 * width), {width, width}, BLUE);
    for (int i = 1; i < ecs.entity_count(); ++i) {
	DrawCircleV(ecs.read_component<Vector2>(i, POSITION_INDEX), 10.f, RED);
    }
}

void resize() {
    window_width = GetScreenWidth();
    window_width = GetScreenHeight();
}

int main() {
    InitWindow(window_width, window_height, window_title);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    //Player
    ecs.add_entity(POSITION_FLAG | VELOCITY_FLAG);
    set_components(PLAYER, {0, 0}, {0, 0}, PLAYER_CONTROL);

    while (!WindowShouldClose()) {
	BeginDrawing();
        ClearBackground(GRAY);     
	control_system();
	movement_system();
	draw();
	EndDrawing();

	if (IsWindowResized()) resize();
    }

    CloseWindow();
    return 0;
}
