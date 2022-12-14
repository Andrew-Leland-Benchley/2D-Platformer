#include <vector>
#include <map>
#include <iostream>
#include <raylib.h>

int WIDTH = 2560;
int HEIGHT = 1440;

int DEATH_ZONE = 5000;

float STARTING_X = 1200;
float STARTING_Y = -1000;

typedef struct Player {

	Rectangle pos_size{ STARTING_X,STARTING_Y,50,100 };
	Rectangle old_pos{ pos_size.x,pos_size.y,pos_size.width,pos_size.height };

	Vector2 camera_coord = { 948,200 };
	Vector2 camera_vel = { 0,0 };
	float camera_acc = 5000;

	float x_vel = 0;
	float y_vel = 0;

	float speed = 600;
	float acc = 1970;
	float air_acc = 1200;
	float air_decc = 700;
	float decc = 1900;

	float grav = 3500;
	float term_vel = 10000;

	float jump = -1000;

	bool isJumping = false;
	bool isGrounded = false;
	bool onWallR = false;
	bool onWallL = false;

} Player;

enum Platform_Type {
	normal,
	wall_jump,
	ice,
	sand,
	jumpy,
};

enum Asset_Type {
	decor,
	interactable,
	entry,
};

typedef struct Platforms_Data {
	float x;
	float y;
	float width;
	float height;
	Platform_Type type;
};

typedef struct Assets_Data {
	float x;
	float y;
	float width;
	float height;
	Asset_Type type;
};

class Current_Level {
	private:

	public:
		std::vector<Platforms_Data> platforms = {
			{100, 700, 1900, 50, ice},
			{1200, 600, 50, 50, jumpy},
			{400, 400, 50,50, normal},
			{900, 600, 50, 100, normal},
			{650, 500, 50, 50, normal},
			{650, 300, 50, 50, normal},
			{400, 200, 50, 50, normal},
			{650, 100, 50, 50, normal},
			{900, 0, 50, 50, normal},
			{650, -100, 50, 50, normal},
			{0, -200, 400, 50, normal},
			{-2000, 800, 1900, 50, sand},
			{-1000, -300, 1000, 50, sand},
			{2000, 800, 50, 50, normal},
			{1500, 900, 1400, 100, ice},
			{2900, 0, 50, 900, wall_jump},
		};

		std::vector<Assets_Data> assets = {
			{100,600,50,50,decor},
			{300,550,50,50,decor},
			{1200, 0, 50, 600, decor},
		};

		//debug colors
		Color platform_color{ 255,255,255,255 };
		Color asset_color{ 30,30,30,255 };
};

void do_physics(Player* player) {

	float deltaTime = GetFrameTime();

	player->old_pos = player->pos_size;

	if (player->pos_size.y > DEATH_ZONE) {
		player->pos_size.x = STARTING_X;
		player->pos_size.y = STARTING_Y;
		player->y_vel = 0;
		player->x_vel = 0;
	}

	if (player->isGrounded || player->onWallR || player->onWallL) {
		if (IsKeyPressed(KEY_SPACE)) {
			player->y_vel = player->jump;
			if (!player->isGrounded) {
				if (player->onWallR) {
					player->x_vel = 200;
				}
				else if (player->onWallL) {
					player->x_vel = -200;
				}
			}
		}
		if (IsKeyDown(KEY_A) && player->x_vel > -player->speed) {
			player->x_vel -= player->acc * deltaTime;
		}
		if (IsKeyDown(KEY_D) && player->x_vel < player->speed) {
			player->x_vel += player->acc * deltaTime;
		}

		if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
			if (player->x_vel > 0) {
				if (player->x_vel - (player->decc * deltaTime) < 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel -= player->decc * deltaTime;
				}
			}
			if (player->x_vel < 0) {
				if (player->x_vel + (player->decc * deltaTime) > 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel += player->decc * deltaTime;
				}
			}
		}

		// if I'm going over the allowed speed when transitioning to different blocks
		if (abs(player->x_vel) > player->speed) {
			if (player->x_vel > 0) {
				if (player->x_vel - (player->decc * deltaTime) < 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel -= player->decc * deltaTime;
				}
			}
			if (player->x_vel < 0) {
				if (player->x_vel + (player->decc * deltaTime) > 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel += player->decc * deltaTime;
				}
			}
		}

		if (abs(player->y_vel) > player->term_vel) {
			if (player->y_vel > 0) {
				if (player->y_vel - (player->decc * deltaTime) < 0) {
					player->y_vel = 0;
				}
				else {
					player->x_vel -= player->decc * deltaTime;
				}
			}
			if (player->y_vel < 0) {
				if (player->y_vel + (player->decc * deltaTime) > 0) {
					player->y_vel = 0;
				}
				else {
					player->y_vel += player->decc * deltaTime;
				}
			}
		}
	}
	else {
		if (IsKeyDown(KEY_A) && player->x_vel > -player->speed) {
			player->x_vel -= player->air_acc * deltaTime;
		}
		if (IsKeyDown(KEY_D) && player->x_vel < player->speed) {
			player->x_vel += player->air_acc * deltaTime;
		}

		if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
			if (player->x_vel > 0) {
				if (player->x_vel - (player->air_decc * deltaTime) < 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel -= player->air_decc * deltaTime;
				}
			}
			if (player->x_vel < 0) {
				if (player->x_vel + (player->air_decc * deltaTime) > 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel += player->air_decc * deltaTime;
				}
			}
		}
	}
	player->pos_size.x += player->x_vel * deltaTime;

	if (player->y_vel < player->term_vel) {
		player->y_vel += player->grav * deltaTime;
	}

	player->pos_size.y += player->y_vel * deltaTime;
}

void collision_detection(Player* player, std::vector<Platforms_Data> platforms) {

	//std::printf("%.2f %.2f\n", player->pos_size.x, player->pos_size.y);
	//std::printf("%.2f\n", player->y_vel);

	float deltaTime = 0.01;

	for (int i = 0; i < platforms.size(); i++) {
		if (CheckCollisionRecs(player->pos_size, Rectangle{ platforms[i].x, platforms[i].y, platforms[i].width, platforms[i].height })) {
			//If my old position is above the platform
			if (player->old_pos.y + player->old_pos.height < platforms[i].y) {
				float y_recoil = player->y_vel * 0.85;
				player->y_vel = 0;
				player->isGrounded = true;
				player->pos_size.y = platforms[i].y - player->pos_size.height - deltaTime;

				//Platform type results
				if (platforms[i].type == ice) {
					player->acc = 1970;
					player->decc = 450;
					player->speed = 800;
					player->jump = -1000;
				}
				else if (platforms[i].type == sand) {
					player->acc = 1100;
					player->decc = 5000;
					player->speed = 300;
					player->jump = -600;
				}
				else if (platforms[i].type == jumpy) {
					player->y_vel = -y_recoil;
					//player->jump = -3000;
					//player->speed = 600;
					//player->acc = 1970;
					//player->decc = 1900;
				}
				else if (platforms[i].type == normal) { // enum -> normal
					player->speed = 600;

					player->acc = 1970;
					player->air_acc = 1200;

					player->decc = 1900;
					player->air_decc = 700;

					player->grav = 3500;
					player->term_vel = 10000;

					player->jump = -1000;
				}
				break;
			}
			//If my old position is below the platform
			if (player->old_pos.y > platforms[i].y + platforms[i].height) {
				player->y_vel = 0;
				player->pos_size.y = platforms[i].y + platforms[i].height + deltaTime;
				break;
			}
		}
		else {
			player->isGrounded = false;
		}
	}
	for (int i = 0; i < platforms.size(); i++) {
		if (CheckCollisionRecs(player->pos_size, Rectangle{ platforms[i].x, platforms[i].y, platforms[i].width, platforms[i].height })) {
			//If my old position is to the left of the platform
			if (player->old_pos.x + player->old_pos.width < platforms[i].x) {
				player->x_vel = 0;
				player->pos_size.x = platforms[i].x - player->pos_size.width - deltaTime;
				if (platforms[i].type == wall_jump) {
					player->onWallL = true;
					player->onWallR = false;
					player->term_vel = 200;
				}
				break;
			}
			//If my old position is to the right of the platform
			if (player->old_pos.x > platforms[i].x + platforms[i].width) {
				player->x_vel = 0;
				player->pos_size.x = platforms[i].x + platforms[i].width + deltaTime;
				if (platforms[i].type == wall_jump) {
					player->onWallR = true;
					player->onWallL = true;
					player->term_vel = 200;
				}
				break;
			}
		}
		else {
			player->grav = 3500;
			player->term_vel = 10000;
			player->onWallR = false;
			player->onWallL = false;
		}
	}
}

void camera_movement(Player * player) {
	float deltaTime = GetFrameTime();
	player->camera_vel.x = (6.7 * (player->pos_size.x - player->camera_coord.x));
	player->camera_vel.y = (6.7 * (player->pos_size.y - player->camera_coord.y));
	player->camera_coord.x += player->camera_vel.x * deltaTime;
	player->camera_coord.y += player->camera_vel.y * deltaTime;
}

void draw_screen(Current_Level level, Player* player, Camera2D camera) {
	BeginDrawing();

		ClearBackground(BLACK);

		BeginMode2D(camera);



		//debug

		for (int i = 0; i < level.platforms.size(); i++) {
			if (level.platforms[i].type == ice) {
				DrawRectangle(level.platforms[i].x, level.platforms[i].y, level.platforms[i].width, level.platforms[i].height, Color { 100,180,255,255});
			}
			else if (level.platforms[i].type == sand) {
				DrawRectangle(level.platforms[i].x, level.platforms[i].y, level.platforms[i].width, level.platforms[i].height, Color{ 245,245,220,255 });
			}
			else if (level.platforms[i].type == jumpy) {
				DrawRectangle(level.platforms[i].x, level.platforms[i].y, level.platforms[i].width, level.platforms[i].height, Color{ 245,100,220,255 });
			}
			else {
				DrawRectangle(level.platforms[i].x, level.platforms[i].y, level.platforms[i].width, level.platforms[i].height, level.platform_color);
			}
		}
		for (int i = 0; i < level.assets.size(); i++) {
			DrawRectangle(level.assets[i].x, level.assets[i].y, level.assets[i].width, level.assets[i].height, level.asset_color);
		}
		DrawRectangleLines(player->pos_size.x, player->pos_size.y, player->pos_size.width, player->pos_size.height, { 255, 0, 0, 255 });
		//DrawPixel(player->pos_size.x, player->pos_size.y, Color{ 0,255,0,255 });
		//DrawPixel(player->camera_coord.x, player->camera_coord.y, Color{ 255,0,0,255 });
		//

		EndMode2D();

		DrawFPS(GetScreenWidth() - 200, GetScreenHeight() - 80);

	EndDrawing();
}

int main(void) 
{	
	InitWindow(WIDTH, HEIGHT, "Game");

	//ToggleFullscreen();

	Player player;
	Current_Level level;

	Camera2D camera;
	camera.zoom = 3;
	camera.offset = { (float) (WIDTH / 2) - (player.pos_size.width + 25), (float) (HEIGHT / 2) - (player.pos_size.height + 50)};
	camera.rotation = 0;

	//if fps is > 1000 or < 12, you fall through the platform
	SetTargetFPS(165);

	while (!WindowShouldClose()) {

		do_physics(&player);

		collision_detection(&player, level.platforms);

		camera_movement(&player);
		camera.target = { player.camera_coord.x, player.camera_coord.y };

		draw_screen(level, &player, camera);
	}

	CloseWindow();
}


