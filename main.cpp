#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdint>
#include <windows.h>

using namespace std;

const int MAP_SIZE = 16;

// Theta is cw, 0 is on +x (straight to the right)
int map[MAP_SIZE][MAP_SIZE] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };

// Variables to hold the players position
float posx = 14.0, posy = 1.0;

// Direction the player is looking in
float theta = 1.6;

// Step size for the movement
const float step_size = 0.25;

// Step size for the change in theta
const float theta_step_size = 0.1;

// Step size for the ray casting
const float ray_step_size = 0.1;

const float fov = (3.14159f * 0.75) / 4.0f;

const int number_of_rays = 80; // Screen width
const int screen_height = 24;   // Screen height

char screen[screen_height][number_of_rays];
uint8_t screen_brightness[screen_height][number_of_rays];

float rays_distance[number_of_rays];

const char brightness[] = ".:--+*#%@"; //" .\'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
const int number_of_characters = sizeof(brightness);

void drawPlayerOnMap()
{

    int map_copy[MAP_SIZE][MAP_SIZE];

    // Copy the values from the global map to the temporary map_copy array
    for (int i = 0; i < MAP_SIZE; ++i)
    {
        for (int j = 0; j < MAP_SIZE; ++j)
        {
            map_copy[i][j] = map[i][j];
        }
    }
    map_copy[int(posy)][int(posx)] = 2;

    // Display the 2D array
    for (int i = 0; i < MAP_SIZE; ++i)
    {
        for (int j = 0; j < MAP_SIZE; ++j)
        {
            char character = *"";
            if (map_copy[i][j] == 1) {
                character = '#';
            } else if (map_copy[i][j] == 0) {
                character = ' ';
            } else if (map_copy[i][j] == 2) {
                character = '+';
            } else {
                // Handle other cases if needed
                character = '?';
            }

            // Output the character followed by a space
            std::cout << character << " ";
        }
        cout << "\n";
    }
}


float castSingleRay(float angle)
{
    float ray_x = posx;
    float ray_y = posy;
    float ray_distance = 0;

    // Incrementally move along the ray per step
    while (map[int(ray_y)][int(ray_x)] != 1)
    {
        ray_x += ray_step_size * cos(angle);
        ray_y += ray_step_size * sin(angle);
    }

    // Calculate the distance using the Pythagorean theorem
    float distance = sqrt(pow(ray_x - posx, 2) + pow(ray_y - posy, 2));

    // Distance correction for the fishbowl effect
    distance = distance * cos(theta - angle);

    return distance;
}

/*
//Using DDA
float castSingleRay(float angle)
{
    float ray_x = posx;
    float ray_y = posy;

    // Direction of the ray
    float ray_dx = cos(angle);
    float ray_dy = sin(angle);

    // Initialize step variables
    int step_x = (ray_dx > 0) ? 1 : -1;
    int step_y = (ray_dy > 0) ? 1 : -1;

    // Calculate initial distance to the next vertical and horizontal wall
    float delta_dist_x = step_x / ray_dx;
    float delta_dist_y = step_y / ray_dy;

    // Initialize the next intersection point
    float next_x = ray_x + (int(ray_x) + (step_x > 0 ? 1 : 0) - ray_x) * delta_dist_x;
    float next_y = ray_y + (int(ray_y) + (step_y > 0 ? 1 : 0) - ray_y) * delta_dist_y;

    // Incrementally move along the ray using DDA
    while (map[int(ray_y)][int(ray_x)] != 1)
    {
        if (next_x < next_y)
        {
            next_x += delta_dist_x;
            ray_x += step_x;
        } else
        {
            next_y += delta_dist_y;
            ray_y += step_y;
        }
    }

    // Calculate the distance using the Pythagorean theorem
    float distance = sqrt(pow(ray_x - posx, 2) + pow(ray_y - posy, 2));

    // Distance correction for the fishbowl effect
    distance = distance * cos(theta - angle);

    return distance;
}
*/
void rayCast()
{
    // Calculate the angle step size based on the FOV and number of rays
    float angle_step = fov / (number_of_rays - 1);

    // Calculate the starting angle as the middle of the FOV
    float start_angle = theta - (fov / 2);

    // Cast rays for each angle in the FOV
    for (int i = 0; i < number_of_rays; ++i)
    {
        // Calculate the current angle
        float current_angle = start_angle + i * angle_step;

        // Cast a ray and print the result
        float distance = castSingleRay(current_angle);
        rays_distance[i] = max(distance, 1.5f);
        //cout << rays_distance[i] << " ";
    }
    // cout << "\n";
}

void projectDistanceTo3D()
{
    int bar_height = 32;
    for (int i = 0; i < screen_height; i++)
    {
        for (int j = 0; j < number_of_rays; j++)
        {

            screen_brightness[i][j] = int(((float)i / (float)screen_height) * 210.0);

            if (i < screen_height / 2)
            {
                screen[i][j] = *" ";
            } else if (i < screen_height * 3 / 4) {
                //screen_brightness[i][j] = 100;
                screen[i][j] = *".";

            } else if (i < screen_height * 7 / 8) {
                //screen_brightness[i][j] = 175;
                screen[i][j] = *".";

            } else {
                // Assign a default value or leave it as is.
                screen[i][j] = *" ";
            }

        }
    }

    for (int j = 0; j < number_of_rays; j++)
    {
        int bar_height = screen_height - (((float)rays_distance[j] / (14.0)) * (float)screen_height);
        // int free_space = (float)screen_height - (float)bar_height;
        int top_free_space = ((float)screen_height - (float)bar_height) / 2;
        // int bottom_free_space = ((float)screen_height - (float)bar_height) / 2.0;

        for (int i = 0; i < top_free_space; i++)
        {
            screen[i][j] = *" ";
        }

        for (int i = top_free_space; i < bar_height; i++)
        {
            screen[i][j] = brightness[int((1 - ((float)rays_distance[j] / (14.0))) * (float)number_of_characters)];
            screen_brightness[i][j] = int((1 - ((float)rays_distance[j] / (14.0))) * 255.0);
        }
        /*
        for (int i = top_free_space + bar_height; i < screen_height; i++)
        {

             cout << i << " ";
             screen[i][j] = *".";
             if (i > screen_height * 0.5)
            {
                 screen[i][j] = *":";
             }
             if (i > screen_height * 0.75)
            {
                 screen[i][j] = *"-";
             }
             else
            {

             screen[i][j] = *i.str();
             }
        }
        */
    }

    // Add Minimap to the screen
    if (0) {
        for (int i = number_of_rays - MAP_SIZE; i < number_of_rays; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                if (map[j][i - number_of_rays] == 0) {
                    screen[j][i] = *" ";
                }

                if (map[j][i - number_of_rays] == 1) {
                    screen[j][i] = *"#";
                }

                if ((j == int(posy)) && ((i - number_of_rays) == int(posx))) {
                    screen[j][i] = *"+";
                }
            }
        }

        //Draw player on minimap
        screen[int(posy)][number_of_rays - (MAP_SIZE - int(posx))] = *"+";
    }
}

void drawScreen()
{
    system("cls");
    for (int j = 0; j < screen_height; j++)
    {
        for (int i = 0; i < number_of_rays; i++)
        {
            // Set the brightness level (0 to 255)
            int brightnessLevel = screen_brightness[j][i];

            // Output a character with adjusted brightness (using true color escape sequence)
            std::cout << "\x1b[38;2;" << brightnessLevel << ";" << brightnessLevel << ";" << brightnessLevel << "m" << screen[j][i] << "\x1b[0m";

            //cout << screen[j][i];
        }
        cout << "\n";
    }
}

void stepPlayerLocation(bool dir)
{
    float posx_temp = posx;
    float posy_temp = posy;

    if (dir == 1)
    {
        // Calculate the new position based on the bearing (theta) and step size
        posx += (float)step_size * cos(theta);
        posy += (float)step_size * sin(theta);
    } else
    {
        // Calculate the new position based on the bearing (theta) and step size
        posx -= (float)step_size * cos(theta);
        posy -= (float)step_size * sin(theta);
    }
    if (map[int(posy)][int(posx)] == 1)
    {
        posx = posx_temp;
        posy = posy_temp;
    }
}

void getKeyPress()
{

    if (GetAsyncKeyState('W') & 0x8000) // 'W' key is pressed
    {
        stepPlayerLocation(1);
    }
    if (GetAsyncKeyState('S') & 0x8000) // 'S' key is pressed
    {
        stepPlayerLocation(0);
    }
    if (GetAsyncKeyState('D') & 0x8000) // 'D' key is pressed
    {
        theta += theta_step_size;
    }
    if (GetAsyncKeyState('A') & 0x8000) // 'A' key is pressed
    {
        theta -= theta_step_size;
    }

    // You may want to introduce a small delay here to avoid rapid key detection
    Sleep(100);
}

int main()
{
    std::ios::sync_with_stdio(false);
    cout << setprecision(9);
    // Main loop
    while (true)
    {
        getKeyPress();
        rayCast();
        projectDistanceTo3D();
        drawScreen();
        //drawPlayerOnMap(); // Show a minimap after the screen is drawn
        cout << posx << " " << posy << " " << theta << endl;
    }

    return 0;
}
