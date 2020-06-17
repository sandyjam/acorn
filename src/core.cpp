#include "core.h"
#include "model.h"
#include "texture.h"
#include <iostream>

#define NUM_MODELS 3
// TODO: move data, preferably to a memory arena
static Model models[NUM_MODELS] = {};

static Core core_local;
Core *core = &core_local;

void Core::run() {
    info("Initializing...");

    init();

    info("Initialized successfully");

    while (true) {
        m_platform.update();

        // TODO: remove temporary update
        {
            if (game_state.camera.is_orbiting) {
                f32 t = glfwGetTime() * 0.25f;
                game_state.camera.position = glm::vec3(cos(t) * 2, 0, sin(t) * 2);
                game_state.camera.look_at = glm::vec3(0, 0, 0);
            }

            Model *spheres = &models[0];
            Model *rifle = &models[1];
            Model *rock = &models[2];

            m_renderer.queueRenderable(Renderable{
                    Transform{
                            glm::vec3(0, 0.15, 0),
                            glm::identity<glm::quat>(),
                            glm::vec3(1.0f)
                    },
                    spheres
            });

            m_renderer.queueRenderable(Renderable{
                    Transform{
                            glm::vec3(0, -0.45, -0.35),
                            glm::vec3(0, glm::half_pi<f32>(), 0),
                            glm::vec3(0.01f)
                    },
                    rifle
            });

            m_renderer.queueRenderable(Renderable{
                    Transform{
                            glm::vec3(0, -0.45, 0.35),
                            glm::vec3(0, glm::half_pi<f32>(), 0),
                            glm::vec3(1.0f)
                    },
                    rock
            });

            m_renderer.queueRenderable(Renderable{
                    Transform{
                            glm::vec3(0, -0.45, 0.35),
                            glm::vec3(0, glm::half_pi<f32>(), 0),
                            glm::vec3(1.0f)
                    },
                    rock
            });
        }

        // draw frame
        m_renderer.render();

//        debug_gui_draw(&game_state);
    }
}

void Core::quit() {
    info("Quitting normally");
    cleanup();
    exit(0);
}

void Core::info(const std::string &msg) {
    std::cout << "[msg] " << msg << std::endl;
}

void Core::warn(const std::string &msg) {
    std::cerr << "[warn] " << msg << std::endl;
}

void Core::fatal(const std::string &msg) {
    std::cerr << "[fatal] " << msg << std::endl;
    cleanup();
    exit(1);
}

void Core::init() {
    game_state.render_options.width = 1024;
    game_state.render_options.height = 768;
    game_state.camera.position = glm::vec3(1, 0, 0);
    game_state.camera.look_at = glm::vec3(0, 0, -1);
    game_state.camera.fov_radians = glm::quarter_pi<f32>();
    game_state.sun_direction = glm::normalize(glm::vec3(-1, 1, 1));

    m_platform.init();
    m_renderer.init();
    if (!textures_init()) core->fatal("Failed to init textures");
//    if (!debug_gui_init()) return false;

    // TODO: remove temp model loading
    models[0] = model_load_from_obj("../assets/spheres/spheres.obj", "../assets/spheres");
    models[1] = model_load_from_obj("../assets/stylized-rifle/Stylized_rifle_final.obj", "../assets/stylized-rifle");
    models[2] = model_load_from_obj("../assets/rock03/3DRock003_16K.obj", "../assets/rock03");
}

void Core::cleanup() {
    for (u32 i = 0; i < NUM_MODELS; ++i) {
        model_free(&models[i]);
    }

//    debug_gui_shutdown();
    textures_shutdown();
    m_renderer.destroy();
    m_platform.destroy();
}
