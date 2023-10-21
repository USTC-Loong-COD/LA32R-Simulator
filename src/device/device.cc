#include <cpu.h>
#include <common.h>
#include <SDL2/SDL.h>
#include <mmio.h>

extern void send_key(uint8_t, bool);
extern void vga_update_screen();

bool quit = false;
#define TIMER_HZ 60

void device_update() {
    vga_update_screen();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                cpu.state = SIM_STOP;
            break;
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                uint8_t k = event.key.keysym.scancode;
                bool is_keydown = (event.key.type == SDL_KEYDOWN);
                send_key(k, is_keydown);
                break;
            }
            default: break;
        }
    }
}

int thread_update(void* ptr) {
    while (!quit) {
        device_update();
    }
    return 0;
}

extern void init_serial();
extern void init_rtc();
extern void init_keyboard();
extern void init_vga();

SDL_Thread * thread = NULL;
extern std::map<std::pair<paddr_t, paddr_t>, io_map_t> io_space;
void free_device(){
    quit = true;
    SDL_WaitThread(thread, NULL);
    SDL_Quit();

    for(auto iter = io_space.begin(); iter != io_space.end(); iter++){
        if(iter->second.base != NULL) delete [] (uint8_t*)(iter->second.base);
    }

}
void init_device() {
    init_serial();
    init_rtc();
    init_keyboard();
    init_vga();
    thread = SDL_CreateThread(thread_update, "device_update", NULL);
}

