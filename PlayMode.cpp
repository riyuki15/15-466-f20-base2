#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint tree_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > tree_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("tree.pnct"));
	tree_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > tree_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("tree.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = tree_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = tree_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

PlayMode::PlayMode() : scene(*tree_scene) {
	//get pointers to leg for convenience:
	for (auto &transform : scene.transforms) {
		if (transform.name == "layer1") layer1 = &transform;
		else if (transform.name == "layer2") layer2 = &transform;
		else if (transform.name == "layer3") layer3 = &transform;
		else if (transform.name == "layer4") layer4 = &transform;
		else if (transform.name == "star") star = &transform;
		else if (transform.name == "trunk") trunk = &transform;
		else if (transform.name == "present") presents[0].transform = &transform;
    else if (transform.name == "present.001") presents[1].transform = &transform;
    else if (transform.name == "present.002") presents[2].transform = &transform;
    else if (transform.name == "present.003") presents[3].transform = &transform;
    else if (transform.name == "present.004") presents[4].transform = &transform;
    else if (transform.name == "present.005") presents[5].transform = &transform;
    else if (transform.name == "present.006") presents[6].transform = &transform;
    else if (transform.name == "snowman") snowmen[0].transform = &transform;
    else if (transform.name == "snowman.001") snowmen[1].transform = &transform;
    else if (transform.name == "snowman.002") snowmen[2].transform = &transform;
    else if (transform.name == "snowman.003") snowmen[3].transform = &transform;
    else if (transform.name == "snowman.004") snowmen[4].transform = &transform;
    else if (transform.name == "snowman.005") snowmen[5].transform = &transform;
    else if (transform.name == "snowman.006") snowmen[6].transform = &transform;
	}
	if (layer1 == nullptr) throw std::runtime_error("layer1 not found.");
	if (layer2 == nullptr) throw std::runtime_error("layer2 not found.");
	if (layer3 == nullptr) throw std::runtime_error("layer3 not found.");
	if (layer4 == nullptr) throw std::runtime_error("layer4 not found.");
	if (star == nullptr) throw std::runtime_error("star not found.");
	if (trunk == nullptr) throw std::runtime_error("trunk not found.");
	for (int i = 0; i < present_count; i++) {
	  if (presents[i].transform == nullptr) {
	    throw std::runtime_error("present " + std::to_string(i) + " not found.");
	  }
    if (snowmen[i].transform == nullptr) {
      throw std::runtime_error("snowman " + std::to_string(i) + " not found.");
    }
	}

	layer1_base_rotation = layer1->rotation;
	layer2_base_rotation = layer2->rotation;
	layer3_base_rotation = layer3->rotation;
	layer4_base_rotation = layer4->rotation;
	star_base_rotation = star->rotation;
	trunk_base_rotation = trunk->rotation;
  for (int i = 0; i < present_count; i++) {
    presents[i].base_rotation = presents[i].transform->rotation;
  }

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
      down.downs += 1;
      down.pressed = true;
      return true;
    } else if (evt.key.keysym.sym == SDLK_LEFT) {
		  if (tree_acc.x > 0) tree_acc.x -= chg_dir_multiplier * acc_step;
		  else tree_acc.x -= acc_step;
		  return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
		  if (tree_acc.x < 0) tree_acc.x += chg_dir_multiplier * acc_step;
		  else tree_acc.x += acc_step;
		  return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
		  if (tree_acc.y < 0) tree_acc.y += chg_dir_multiplier * acc_step;
      else tree_acc.y += acc_step;
      return true;
    } else if (evt.key.keysym.sym == SDLK_DOWN) {
		  if (tree_acc.y > 0) tree_acc.y -= chg_dir_multiplier * acc_step;
		  else tree_acc.y -= acc_step;
		  return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
		  space_pressed = true;
		  return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	wobble += elapsed / 10.0f;
	wobble -= std::floor(wobble);

	tree_acc.x = std::clamp(tree_acc.x, -acc_max, acc_max);
	tree_acc.y = std::clamp(tree_acc.y, -acc_max, acc_max);

  tree_velocity.x += elapsed * tree_acc.x;
  tree_velocity.y += elapsed * tree_acc.y;
  tree_velocity.x = std::clamp(tree_velocity.x, -vel_max, vel_max);
  tree_velocity.y = std::clamp(tree_velocity.y, -vel_max, vel_max);
	float dx = elapsed * (tree_velocity.x + 0.5f * tree_acc.x * elapsed);
	float dy = elapsed * (tree_velocity.y + 0.5f * tree_acc.y * elapsed);

	trunk->position += glm::vec3(dx, dy, 0.f);
	if (trunk->position.x > 3.5f) trunk->position.x = 3.5f;
	else if (trunk->position.x < -3.5f) trunk->position.x = -3.5f;
	if (trunk->position.y > 5.f) trunk->position.y = 5.f;
	else if (trunk->position.y < -3.f) trunk->position.y = -3.f;

	glm::vec3 rotate_axis = glm::vec3(0.f, 0.f, 1.f);

	layer1->rotation = glm::angleAxis(
	        glm::radians(tree_velocity.x * 2 * std::sin(wobble * 2.0f * float(M_PI))),
	        rotate_axis);
	layer2->rotation = glm::angleAxis(
          glm::radians(tree_velocity.x * 2 * std::sin(wobble * 2.0f * float(M_PI))),
          rotate_axis);
  layer3->rotation = glm::angleAxis(
          glm::radians(tree_velocity.x * 1 * std::sin(wobble * 2.0f * float(M_PI))),
          rotate_axis);
  layer4->rotation = glm::angleAxis(
          glm::radians(tree_velocity.x * 1 * std::sin(wobble * 2.0f * float(M_PI))),
          rotate_axis);

  if (space_pressed && ejected_count < present_count) {
    presents[ejected_count].acc = glm::vec2(-50 * tree_acc.x, -50 * tree_acc.y);
    presents[ejected_count].velocity = glm::vec2(3 * tree_velocity.x, 3 * tree_velocity.y);
    presents[ejected_count].transform->parent = nullptr;
    presents[ejected_count].transform->rotation = trunk_base_rotation;
    presents[ejected_count].transform->position = trunk->position;
    ejected_count += 1;
    space_pressed = false;
  }

  for (int i = 0; i < ejected_count; i++) {
    dx = elapsed * (presents[i].velocity.x + 0.5f * presents[i].acc.x * elapsed);
    dy = elapsed * (presents[i].velocity.y + 0.5f * presents[i].acc.y * elapsed);
    presents[i].transform->position += glm::vec3(dx, dy, 0.f);
    for (int j = 0; j < present_count; j++) {
      glm::vec3 snowman_position = snowmen[j].transform->position;
      if (std::abs(presents[i].transform->position.x - snowman_position.x) < 0.5f &&
          std::abs(presents[i].transform->position.y - snowman_position.y) < 0.5f) {
        presents[i].velocity = glm::vec2(0.f, 0.f);
        presents[i].acc = glm::vec2(0.f, 0.f);
        presents[i].captured = true;
      }
    }
  }

  if (ejected_count == present_count && !printed_ending) {
    std::cout << "\n";
    int captured_count = 0;
    for (int i = 0; i < present_count; i++) {
      if (presents[i].captured)
        captured_count += 1;
    }
    if (captured_count == 0)
      std::cout << "None of the gifts were received!\n" << std::endl;
    else if (captured_count < 3)
      std::cout << "Only " + std::to_string(captured_count) +"/7 gifts were received.\n" << std::endl;
    else if (captured_count < present_count)
      std::cout << "You sent " + std::to_string(captured_count) + "/7 gifts successfully. Good job!\n" << std::endl;
    else if (captured_count == present_count)
      std::cout << "All of the gifts were received! The snowmen will have a happy new year.\n" << std::endl;
    printed_ending = true;
  }

	//move camera:
	{

		//combine inputs into a move:
		constexpr float PlayerSpeed = 30.0f;
		glm::vec2 move = glm::vec2(0.0f);
		if (left.pressed && !right.pressed) move.x =-1.0f;
		if (!left.pressed && right.pressed) move.x = 1.0f;
		if (down.pressed && !up.pressed) move.y =-1.0f;
		if (!down.pressed && up.pressed) move.y = 1.0f;

		//make it so that moving diagonally doesn't go faster:
		if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 right = frame[0];
		//glm::vec3 up = frame[1];
		glm::vec3 forward = -frame[2];

		camera->transform->position += move.x * right + move.y * forward;
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	GL_ERRORS();
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	GL_ERRORS();
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	GL_ERRORS();
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
}
