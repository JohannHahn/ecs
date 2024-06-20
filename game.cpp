#include <cassert>
#include <iostream>
#include "raylib/src/raylib.h"
#include "raylib/src/raymath.h"
#include "ecs.h"

#define MIDDLE_POINT(v, size) {v.x + size.x / 2.f, v.y + size.y / 2.f} 
#define MIDDLE_POINT_REC(rec) {rec.x + rec.w / 2.f, rec.y + rec.h / 2.f} 

// collectables
// - sword
// - projectile
// enemies with behaviours
// weapons
//
//
enum component_flag{
    POSITION_FLAG = 1, VELOCITY_FLAG = 2, TYPE_FLAG = 4, BEHAVIOUR_FLAG = 8
};

enum component_index{
    POSITION_INDEX, VELOCITY_INDEX, TYPE_INDEX, BEHAVIOUR_INDEX, COMPONENT_INDEX_MAX
};

enum entity_type {
    PLAYER, ENEMY, ITEM, ITEM_SWORD, ITEM_LAST 
};

enum behaviour {
    PLAYER_CONTROL, SEEK, NOTHING
};

enum weapon {
    NO_WEAPON, SWORD,
};

struct Player {
    u64 entity_id;
    float speed;
    Vector2 size;
    Vector2 hit_dir;
    int face_dir = 1;
    float collect_distance_base;
    weapon weapon;
};


class Game {
public:
    Game(u64 component_count, 
	 u64 component_sizes[], float width = 0.f, float height = 0.f, const char *title = "") 
    : window_width(width), window_height(height), window_title(title), ecs(component_count, component_sizes) {
	for (int i = 0; i < component_count; ++i) {
	    this->component_sizes.push_back(component_sizes[i]);
	}
	item_size = {10.f, 10.f};
	enemy_speed = 100.f;
    };
  float window_width;
  float window_height;
  const char *window_title;
  ECS ecs;
  Player player;
  Vector2 item_size;
  float enemy_speed;
  std::vector<u64> component_sizes;

  bool is_item(u64 entity_id);
  void collect(u64 entity_id);
  void set_components(u64 entity_id, Vector2 position, Vector2 velocity,
                      entity_type type, behaviour behaviour);
  void add_collectable(Vector2 position, entity_type type);
  void remove_enemy(u64 entity_id);
  void add_enemy();
  void movement_system();
  void control_system();
  void draw();
  void resize();
};

bool Game::is_item(u64 entity_id) {
    entity_type type = ecs.read_component<entity_type>(entity_id, TYPE_INDEX);
    return type >= ITEM && type <= ITEM_LAST;
}

void Game::collect(u64 entity_id) {
    assert(entity_id > player.entity_id);
    entity_type type = ecs.read_component<entity_type>(entity_id, TYPE_INDEX);
    if (type == ITEM_SWORD) {
	player.weapon = SWORD;
    }
    ecs.remove_entity(entity_id);
}

void Game::set_components(u64 entity_id, Vector2 position, Vector2 velocity, entity_type type, behaviour behaviour) {
    ecs.write_component(entity_id, POSITION_INDEX, position);    
    ecs.write_component(entity_id, VELOCITY_INDEX, velocity);    
    ecs.write_component(entity_id, TYPE_INDEX, type);    
    ecs.write_component(entity_id, BEHAVIOUR_INDEX, behaviour);    
}

void Game::remove_enemy(u64 entity_id) {
    if (ecs.entity_count() == 1) {
	return;
    }
    if (entity_id == player.entity_id) {
	assert(0 && "trying to delete player in remove_enemy");
    }
    ecs.remove_entity(entity_id);
}

void Game::add_collectable(Vector2 position, entity_type type) {
    assert(type >= ITEM && type <= ITEM_LAST);
    u64 entity_id = ecs.add_entity(POSITION_FLAG | TYPE_FLAG);
    set_components(entity_id, position, {0,0}, type, NOTHING);
}
void Game::add_enemy() {
    u64 entity_id = ecs.add_entity(POSITION_FLAG | VELOCITY_FLAG | TYPE_FLAG);
    Vector2 position = {entity_id * 100.f, window_height / 2.f};
    Vector2 velocity = {.0, .0}; 
    set_components(entity_id, position, velocity, ENEMY, SEEK);
}

void Game::movement_system() {   
    for(int i = 0; i < ecs.entity_count(); ++i) {
	if (!ecs.check_components(i, POSITION_FLAG | VELOCITY_FLAG)) continue; 
	Vector2 pos = ecs.read_component<Vector2>(i, POSITION_INDEX);
	Vector2 vel = ecs.read_component<Vector2>(i, VELOCITY_INDEX);
	vel = Vector2Scale(vel, GetFrameTime());
	pos = Vector2Add(pos, vel);
	ecs.write_component(i, POSITION_INDEX, pos);
    } 
}

void Game::control_system() {
    int key = GetKeyPressed();
    Vector2 vel = {0};
    if (IsKeyDown(KEY_LEFT)) {
	vel = Vector2Add(vel, {-player.speed, 0.f});
	player.hit_dir.x = -1;
	player.face_dir = -1;
    }
    if (IsKeyDown(KEY_RIGHT)) {
	vel = Vector2Add(vel, {player.speed, 0.f});
	player.hit_dir.x = 1;
	player.face_dir = 1;
    }
    if (IsKeyDown(KEY_UP)) {
	vel = Vector2Add(vel, {0.f, -player.speed});
	player.hit_dir.y = -1;
    }
    if (IsKeyDown(KEY_DOWN)) {
	vel = Vector2Add(vel, {0.f, player.speed});
	player.hit_dir.y = 1;
    }
    if (IsKeyReleased(KEY_UP) || IsKeyReleased(KEY_DOWN)) {
	player.hit_dir.y = 0;
    }
    if (IsKeyReleased(KEY_LEFT) || IsKeyReleased(KEY_RIGHT)) {
	player.hit_dir.x = 0;
    }
    ecs.write_component(player.entity_id, VELOCITY_INDEX, vel);
    Vector2 player_pos = ecs.read_component<Vector2>(player.entity_id, POSITION_INDEX);
    for (int i = 1; i < ecs.entity_count(); ++i) {
	if (is_item(i)) {
	    Vector2 item_pos = ecs.read_component<Vector2>(i, POSITION_INDEX);
	    Vector2 player_mid = MIDDLE_POINT(player_pos, player.size);
	    Vector2 item_mid = MIDDLE_POINT(item_pos, item_size);
	    float distance = Vector2Length(Vector2Subtract(player_mid, item_mid));
	    if (distance < player.collect_distance_base) {
		collect(i);
		i--;
		// important, collect deletes the entity, 
		// need to continue to 
		continue;
	    }
	}
	if (ecs.read_component<behaviour>(i, BEHAVIOUR_INDEX) == SEEK) { 
	    Vector2 pos = ecs.read_component<Vector2>(i, POSITION_INDEX);
	    Vector2 vel = Vector2Subtract(player_pos, pos);
	    ecs.write_component(i, VELOCITY_INDEX, vel); 
	}
    }

    if (IsKeyPressed(KEY_T)) {
	add_collectable({(float)GetRandomValue(0, window_width), (float)GetRandomValue(0, window_height)}, 
		 (entity_type)GetRandomValue(ITEM, ITEM_LAST));
    }
}

void Game::draw() {
    Vector2 player_pos = ecs.read_component<Vector2>(PLAYER, POSITION_INDEX);
    DrawRectangleV(player_pos, player.size, BLUE);
    for (int i = 1; i < ecs.entity_count(); ++i) {
	entity_type type = ecs.read_component<entity_type>(i, TYPE_INDEX);
	Vector2 position = ecs.read_component<Vector2>(i, POSITION_INDEX);
	if (type == ENEMY) DrawCircleV(position, 10.f, RED);
	else if (type == ITEM) DrawRectangleV(position, item_size, GREEN);
	else if (type == ITEM_SWORD) DrawRectangleV(position, {item_size.x*3.f, item_size.y}, GREEN);
    }
    if (player.weapon == SWORD && IsKeyDown(KEY_SPACE)) {
	Vector2 player_mid = MIDDLE_POINT(player_pos, player.size);
	Vector2 hit_dir = player.hit_dir;
	if (player.hit_dir.x == 0 && player.hit_dir.y == 0) {
	    hit_dir = {(float)player.face_dir, 0.f};
	}
	Vector2 end = Vector2Add(player_mid, Vector2Scale(hit_dir, 50.f));
	DrawLineEx(player_mid, end, 10.f, GREEN);
    }
}

void Game::resize() {
    window_width = GetScreenWidth();
    window_width = GetScreenHeight();
}

int main() {
    u64 component_sizes[] = {sizeof(Vector2), sizeof(Vector2), sizeof(entity_type), sizeof(behaviour)};
    Game game = Game(4, component_sizes, 900.f, 600.f, "Survive the vampires");
    game.player = {
	.entity_id = 0,
	.speed = 500.f,
	.size = {50.f, 100.f},
	.hit_dir = {0.f, 0.f},
	.collect_distance_base = 50.f,
	.weapon = NO_WEAPON
    };
    InitWindow(game.window_width, game.window_height, game.window_title);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    //Player
    game.ecs.add_entity(POSITION_FLAG | VELOCITY_FLAG | TYPE_FLAG);
    game.set_components(PLAYER, {0, 0}, {0, 0}, PLAYER, PLAYER_CONTROL);
    game.add_collectable({game.window_width / 2.f, game.window_height / 2.f}, ITEM_SWORD);
    while (!WindowShouldClose()) {
	BeginDrawing();
        ClearBackground(GRAY);     
	game.control_system();
	game.movement_system();
	game.draw();
	DrawFPS(0, 0);
	EndDrawing();
	if (IsWindowResized()) game.resize();
    }

    CloseWindow();
    return 0;
}
