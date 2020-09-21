#include "Mode.hpp"

#include "Scene.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	struct Present {
	    Scene::Transform *transform = nullptr;
	    glm::quat base_rotation;
	    glm::vec2 velocity = glm::vec2(0.f, 0.f);
	    glm::vec2 acc = glm::vec2(0.f, 0.f);
	    bool captured = false;
	};

	struct Snowman {
      Scene::Transform *transform = nullptr;
	};

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//hexapod leg to wobble:
	Scene::Transform *layer1 = nullptr;
	Scene::Transform *layer2 = nullptr;
	Scene::Transform *layer3 = nullptr;
	Scene::Transform *layer4 = nullptr;
	Scene::Transform *star = nullptr;
	Scene::Transform *trunk = nullptr;
	glm::quat layer1_base_rotation;
	glm::quat layer2_base_rotation;
	glm::quat layer3_base_rotation;
	glm::quat layer4_base_rotation;
	glm::quat star_base_rotation;
	glm::quat trunk_base_rotation;
	float wobble = 0.0f;

	float acc_step = 100000.f;
	float acc_max = 7.f;
	float vel_max = 3.f;
	float chg_dir_multiplier = 10000.f;
	glm::vec2 tree_velocity = glm::vec2(0.f, 0.f);
	glm::vec2 tree_acc = glm::vec2(0.f, 0.f);
	int acc_counter;
	float acc_threshold = acc_max - 0.1f;

	Present presents[7];
	Snowman snowmen[7];

	int present_count = 7;
	int ejected_count = 0;
	int captured_count = 0;
	bool space_pressed = false;
	bool printed_ending = false;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
