#include <SDL2/SDL.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "math.h"

#include <unistd.h>

#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
using namespace std;

//#define FPS 50.0f //PAL
#define FPS 60.0f //NTSC
//#define FPS 120.0f //PC-CUSTOM
#define FPS_UPDATE 60.0f

void Draw_Grid()
{																	
	for(float i = -500; i <= 500; i += 5)
	{
		glBegin(GL_LINES);
			glColor3ub(150, 190, 150);							
			glVertex3f(-500, 0, i);									
			glVertex3f(500, 0, i);

			glVertex3f(i, 0, -500);								
			glVertex3f(i, 0, 500);
		glEnd();
	}
}


enum move_dir
{   move_null = 0, 
    move_up = 4, 
    move_down = 8, 
    move_left = 16,
    move_right = 32
};

int current_move = move_null;
int check_moved(bool up,bool down,bool left,bool right)
{
    if(current_move == move_null)
    {
        if(up)
        {
            //list_move.push_back(move_up);
            current_move = move_up;
            return current_move;
        }
        if(down)
        {
            //list_move.push_back(move_down);
            current_move = move_down;
            return current_move;
        }
        if(left)
        {
            //list_move.push_back(move_left);
            current_move = move_left;
            return current_move;
        }
        if(right)
        {
            //list_move.push_back(move_right);
            current_move = move_right;
            return current_move;
        }
    }

//un-sure for optimize performance
    if(!left && !right && !up && !down)
    {
        //list_move.clear();
        current_move = move_null;
    }


    //if(!list_move.empty())
    if(current_move != move_null)
    {
        //current_move = list_move[0];
        if(up)
        {
            if(current_move == move_up)
            {
                return current_move;
            }
        }
        if(down)
        {
            if(current_move == move_down)
            {
                return current_move;
            }
        }
        if(left)
        {
            if(current_move == move_left)
            {
                return current_move;
            }
        }
        if(right)
        {
            if(current_move == move_right)
            {
                return current_move;
            }
        }

        //list_move.clear();
        current_move = move_null;
        return check_moved(up,down,left,right);
    }

    return current_move;
}

vector<int> list_move;
int check_moved_2(bool up,bool down,bool left,bool right)
{
    if(current_move == move_null)
    {
        if(up)
        {
            list_move.push_back(move_up);
        }
        if(down)
        {
            list_move.push_back(move_down);
        }
        if(left)
        {
            list_move.push_back(move_left);
        }
        if(right)
        {
            list_move.push_back(move_right);
        }
    }

//un-sure for optimize performance
    if(!left && !right && !up && !down)
    {
        list_move.clear();
        current_move = move_null;
    }

    if(!list_move.empty())
    {
        current_move = list_move[0];
        if(up)
        {
            if(move_up == current_move)
            {
                return current_move;
            }
        }
        if(down)
        {
            if(move_down == current_move)
            {
                return current_move;
            }
        }
        if(left)
        {
            if(move_left == current_move)
            {
                return current_move;
            }
        }
        if(right)
        {
            if(move_right == current_move)
            {
                return current_move;
            }
        }

        list_move.clear();
        current_move = move_null;
        return check_moved_2(up,down,left,right);
    }

    return current_move;
}


// i get this idea from COS4102 (Cohen–Sutherland algorithm) bring bits only
typedef int Encode;
Encode encode_current;
/*
const int NONE = 0;     // 0000
const int LEFT = 1;     // 0001
const int RIGHT = 2;    // 0010
const int DOWN = 4;     // 0100
const int UP = 8;       // 1000
*/

enum encode_dir
{   NONE = 0, 
    LEFT = 1, 
    RIGHT = 2, 
    DOWN = 4,
    UP = 8
};

Encode ComputeEncode(bool up,bool down,bool left,bool right)
{
	Encode code = NONE;

    if(up)
    {
        code |= UP;
    }
    if(down)
    {
        code |= DOWN;
    }
    if(left)
    {
        code |= LEFT;
    }
    if(right)
    {
        code |= RIGHT;
    }

	return code;
}

#include <bitset>

int check_moved_3(Encode encode)
{
    if(encode)
    {
        if(!encode_current)
        {
            encode_current = encode;
        }
        else
        {
            Encode encode_last = encode & encode_current;
            
            if (encode_last & UP)
            {
                return UP;
            }
            if (encode_last & DOWN)
            {
                return DOWN;
            }
            if (encode_last & LEFT)
            {
                return LEFT;
            }
            if (encode_last & RIGHT)
            {
                return RIGHT;
            }

            encode_current = NONE;
            return check_moved_3(encode);
        }
    }

    return NONE;
}

// from https://gist.github.com/therathatter/1bc149bc292f48a0c5511df84de2f3dc {Pump It Up 60 FPS locker}
static uint64_t get_time_ns(void)
{
   struct timespec ts;
   clock_gettime(CLOCK_MONOTONIC, &ts);
   return (uint64_t)ts.tv_sec * (uint64_t)1e9 + (uint64_t)ts.tv_nsec;
}

// from https://github.com/facebook/folly/blob/c83bd7eb9851256657e58585c6906d0589fb58c4/folly/portability/SysResource.cpp#L80
// setpriority for windows
#ifdef _WIN32
// https://github.com/RT-Thread/rt-thread/blob/db5bdb1ffadced28e40b97cd62e95f87ee248db9/components/lwp/lwp_syscall.h#L44
#define	PRIO_PROCESS 0
int setpriority(int which, int who, int value)
{
  if (which != PRIO_PROCESS || who != 0) {
    errno = EINVAL;
    return -1;
  }

  auto newClass = [value] {
    if (value >= 39) {
      return IDLE_PRIORITY_CLASS;
    } else if (value >= 30) {
      return BELOW_NORMAL_PRIORITY_CLASS;
    } else if (value >= 20) {
      return NORMAL_PRIORITY_CLASS;
    } else if (value >= 10) {
      return ABOVE_NORMAL_PRIORITY_CLASS;
    } else {
      return HIGH_PRIORITY_CLASS;
    }
  }();

  if (!SetPriorityClass(GetCurrentProcess(), newClass)) {
    errno = EACCES;
    return -1;
  }
  return 0;
}
#endif

int main(int argv,char** argc)
{
    setpriority(PRIO_PROCESS, 0, 0);

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );

    SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("Story of Seasons ~ After Story ♪ [walk-run-speed]", 
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
        800, 600,
		//SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_ALLOW_HIGHDPI |
    	SDL_WINDOW_OPENGL);

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	//SDL_GL_SetSwapInterval(SDL_TRUE);

	glEnable(GL_DEPTH_TEST);

printf("%s",glGetString(GL_RENDERER));

    vec3 player_pos = vec3(0,0,0);

        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;
        bool a_key = false;

        vec3 dir = vec3(0,0,0);
    

double time         = 0.0;
		double accumulator  = 0.0;
        const constexpr double dt = 1000.0 / FPS_UPDATE;

auto current_time = get_time_ns()*0.001*0.001;

    bool running = true;
    while(running)
    {

vec3 cam_norm;

//from https://gafferongames.com/post/fix_your_timestep/ {Fix Your Timestep!}
auto new_time     = get_time_ns()*0.001*0.001;
double frame_time   = new_time - current_time;
current_time = new_time;

accumulator += frame_time;

while (accumulator >= dt)
    {

/// event
	    SDL_Event event;
	    while (SDL_PollEvent(&event))
	    {
	    	if (event.type == SDL_QUIT)
	    		running = false;
	    	if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
	    		running = false;

            switch (event.type)
            {
                case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_a:  a_key = true; break;
                    case SDLK_LEFT:  left = true; break;
                    case SDLK_RIGHT: right = true; break;
                    case SDLK_UP:    up = true; break;
                    case SDLK_DOWN:  down = true; break;
                }
                break;
                case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                    case SDLK_a:  a_key = false; break;
                    case SDLK_LEFT: left = false; break;
                    case SDLK_RIGHT: right = false; break;
                    case SDLK_UP:    up = false; break;
                    case SDLK_DOWN:  down = false; break;
                }
                break;
            }

	    }

        Encode encode = ComputeEncode(up,down,left,right);
        int test_dir = check_moved_3(encode); //new design

/// update
    vec3 cam_pos(22.50,22.50,31.82);// spherical coordinates (45º,45º,45º)
    //vec3 cam_norm = normalize(cam_pos);
    cam_norm = normalize(cam_pos);
    cam_norm = cam_norm*20.0f;

    float speed_multipier = 1.0f;

    if(a_key)
    {
        speed_multipier = speed_multipier * 2.0f;
    }

    float speed_walk = 0.25f;

    if(test_dir == UP)
    {
        dir.z -= speed_walk;
    }
    if(test_dir == DOWN)
    {
        dir.z += speed_walk;
    }
    if(test_dir == LEFT)
    {
        dir.x -= speed_walk;
    }
    if(test_dir == RIGHT)
    {
        dir.x += speed_walk;
    }
    
    player_pos += dir*speed_multipier;

    dir = vec3(0,0,0);

        accumulator -= dt;
        time += dt;

    }

/// draw
	    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	    glClearDepth(1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    glViewport(0,0,800,600);

	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	    gluPerspective(45.0f,(GLfloat)800/(GLfloat)600,0.1f,1000.0f);
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();			

        gluLookAt(  player_pos.x+cam_norm.x,player_pos.y+cam_norm.y,player_pos.z+cam_norm.z,
                    player_pos.x,player_pos.y,player_pos.z,
                    0,1,0   );
                
        glPushMatrix();
        {
            GLUquadricObj *pObj = gluNewQuadric();
            gluQuadricDrawStyle(pObj, GLU_FILL);

            glTranslatef(player_pos.x,player_pos.y,player_pos.z);
            glColor3f(1.0f,0.0f,1.0f);
            gluSphere(pObj, 1.0f, 10, 8);
        }
        glPopMatrix();

        glPushMatrix();
        {
            Draw_Grid();
        }
        glPopMatrix();

        SDL_GL_SwapWindow(window);

// from https://gist.github.com/therathatter/1bc149bc292f48a0c5511df84de2f3dc {Pump It Up 60 FPS locker}
   if (FPS > 0)
   {
      static uint64_t last_time;
      const double interval = 1e9 / FPS;
      const useconds_t step = interval / 1e6;
      while (last_time > 0 && get_time_ns() - last_time < interval)
      {
        
// from https://www.geeksforgeeks.org/petersons-algorithm-for-mutual-exclusion-set-2-cpu-cycles-and-memory-fence/ {Peterson’s Algorithm for Mutual Exclusion | Set 2 (CPU Cycles and Memory Fence)}
    // Memory fence to prevent the reordering
    // of instructions beyond this barrier.
    __sync_synchronize();

         sched_yield();
         usleep(step);
      }
      last_time = get_time_ns();
   }
    }

    return 0;
}
