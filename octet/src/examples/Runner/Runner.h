////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "Julia.h"
namespace octet {
  /// Scene containing a box with octet.
	

  class Runner : public app {

    Julia_shader julia_shader_;

	class GameObject {
	public:
		GameObject() {}
		GameObject(ref<scene_node> node, btRigidBody* rigidBody, ref<mesh_instance> meshInstance)
		{
			_node = node;
			_rigidBody = rigidBody;
			_meshInstance = meshInstance;
		}

		ref<scene_node> getNode() { return _node; }
		btRigidBody* getRigidBody() { return _rigidBody; }
		ref<mesh_instance> getMeshInstance() { return _meshInstance; }

	private:
		ref<scene_node> _node;
		btRigidBody* _rigidBody;
		ref<mesh_instance> _meshInstance;
	};

	class Background {
	public:
		Background() {}
		Background(ref<scene_node> node, ref<material> mat, ref<param_uniform> zoom, ref<param_uniform> moveX, ref<param_uniform> moveY, ref<param_uniform> ChangeDiv)
		{
			_node = node;
			_mat = mat;
			_zoom = zoom;
			_moveX = moveX;
			_moveY = moveY;
			_ChangeDiv = ChangeDiv;
		}

		ref<param_uniform> moveX() { return _moveX; }
		ref<param_uniform> moveY() { return _moveY; }
		ref<param_uniform> ChangeDiv() { return _ChangeDiv; }
		ref<param_uniform> zoom() { return _zoom; }
		ref<material> mat() { return _mat; }
		ref<scene_node> node() { return _node; }
	private:
		ref<material> _mat;
		ref<param_uniform> _zoom;
		ref<param_uniform> _moveX;
		ref<param_uniform> _moveY;
		ref<param_uniform> _ChangeDiv;
		ref<scene_node> _node;
	};
    // scene for drawing box
	
    ref<visual_scene> app_scene;
	Background background;
	bool freeCamera;
	bool isBackgroundAuto;
	GameObject player;
	std::vector<GameObject> listGameObjects;
	std::vector<std::pair<vec3, time_t>> path;

	int obstacleDrawDistance;
	int obstacleGap;
	int roadWidth;
	int playerSize;
	int backgroundDistance;
	float backgroundZoom;
	float backgroundMoveX;
	float backgroundMoveY;
	time_t divisor_last_change;
	time_t divisor_change_step_time;
	int divisor_change;
	int lastDist;
	struct my_vertex {
		vec3p pos;
	};

  public:
    /// this is called when we construct the class before everything is initialised.
    Runner(int argc, char **argv) : app(argc, argv) {
    }
	// x = right
	// y = Up
	// -z = straight
    /// this is called once OpenGL is initialized
    void app_init() {

	  julia_shader_.init();

	  freeCamera = false;
	  isBackgroundAuto = true;
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
	  app_scene->get_camera_instance(0)->set_far_plane(1000.0f);
	  obstacleDrawDistance = 450;
	  obstacleGap = 50;
	  backgroundDistance = 500;
	  background = Background();
	  roadWidth = 5;
	  playerSize = 1;
	  lastDist = obstacleDrawDistance;
	  listGameObjects = std::vector<GameObject>();

	  backgroundZoom = 1.0f;
	  backgroundMoveX = 0.0f;
	  backgroundMoveY = 0.0f;
	  divisor_change = 20;
	  divisor_last_change = clock();
	  divisor_change_step_time = 100;
	  
	  material *red = new material(vec4(1, 0, 0, 1));
	  material *blue = new material(vec4(0, 0, 1, 1));
	  material *purple = new material(vec4(1, 0, 1, 1));


	  mat4t mat;
	  mat.translate(0, -10, 0);
	  listGameObjects.push_back(createGameObject(mat, new mesh_box(vec3(roadWidth, 0.1f, 100000)), red, false, 1.0f));



	  mat.translate(0, 2, 0);
	  player = createGameObject(mat, new mesh_box(vec3(playerSize)), purple, true, 10.0f);

	  //mat.translate(0, 0, -backgroundDistance);
	  //background = createGameObject(mat, new mesh_box(vec3(1000, 1000, 1)), blue, false, 1.0f);
      int vx = 0, vy = 0;
	  get_viewport_size(vx, vy);
	  drawBackground(vec3(0, 0, player.getNode()->get_position().z() - backgroundDistance), vx, vy);

	  for (int i = 0; i * obstacleGap < obstacleDrawDistance; i++)
	  {
		  createObstacle((i+1) * obstacleGap, new mesh_sphere(vec3(0), 1), blue);
	  }

	  //This is used to generate a path for the Mandlebrot's camera
	  //Format : 
	  // pair( vec3(MoveX, MoveY, Zoom), time (in ms) )
	  //Can add any number of entries > 1. Please note that the time is cumulative (i.e if the first one is 3000, and the second one 10000 the second one will trigger 13sec after start of run)
	  path = std::vector<std::pair<vec3, time_t>>(); 
	  path.push_back(std::pair<vec3, time_t>(vec3(backgroundMoveX, backgroundMoveY, backgroundZoom), 0));
	  path.push_back(std::pair<vec3, time_t>(vec3(-0.6f, -0.0f, 1.5f), 3000));
	  path.push_back(std::pair<vec3, time_t>(vec3(-0.7f, -0.15f, 13.75f), 10000));


    }

	void createObstacle(float distanceFromPlayer, mesh *msh, material *mtl)
	{
		float xCoord = rand() % 5 - 2.5f;
		vec3 relativePos = vec3(xCoord, 0, -distanceFromPlayer);
		mat4t mat;
		mat.translate(0, player.getNode()->get_position().y(),player.getNode()->get_position().z());
		mat.translate(relativePos);
		listGameObjects.push_back(createGameObject(mat, msh, mtl, true, 99999.0f));
	}

	GameObject createGameObject(mat4t_in mat, mesh *msh, material *mtl, bool is_dynamic = false, float mass = 1)
	{
		btRigidBody* rigidBody = NULL;
		scene_node* node = NULL;
		ref<mesh_instance> mi = app_scene->add_shape(mat, msh, mtl, is_dynamic, mass, &rigidBody, &node);
		return GameObject(node, rigidBody, mi);
	}


	void cameraFollowMouse()
	{
		float sensitivity = 100.0f / 360.0f;
		int xPos, yPos;
		get_absolute_mouse_movement(xPos, yPos);
		set_mouse_pos(0, 0);

		mat4t &camera_to_world = app_scene->get_camera_instance(0)->get_node()->access_nodeToParent();

		camera_to_world.x() = vec4(1, 0, 0, 0);
		camera_to_world.y() = vec4(0, 1, 0, 0);
		camera_to_world.z() = vec4(0, 0, 1, 0);

		float angle_x = (float)(app_scene->get_camera_instance(0)->get_node()->get_position().x()-xPos) * sensitivity;
		float angle_y = (float)(app_scene->get_camera_instance(0)->get_node()->get_position().y() -yPos) * sensitivity;
		angle_y = std::max(-90.0f, std::min(angle_y, 90.0f));
		camera_to_world.rotateY(angle_x);
		camera_to_world.rotateX(angle_y);
	}


	void updateCamera()
	{
		if (freeCamera)
		{
			cameraFollowMouse();
		}
		vec3 cameraLoc = player.getNode()->get_position() - vec3(0, -5, -17);
		app_scene->get_camera_instance(0)->get_node()->translate(-app_scene->get_camera_instance(0)->get_node()->get_position());
		app_scene->get_camera_instance(0)->get_node()->translate(cameraLoc);
	}


	void handleMovement()
	{
		int max_divisor = 500;

		player.getNode()->translate(vec3(0, 0, -3.0f));

		float movement = 0.0f;
		if (is_key_down(key_left))
			movement -= 0.5f;
		if (is_key_down(key_right))
			movement += 0.5f;

		
		if (isBackgroundAuto && path.size() > 1)
		{
			time_t currentTime = clock();
			time_t cumul = 0;
			for (int i = 0; i < path.size(); i++)
			{
				cumul += path[i].second;
				if (currentTime < cumul)
				{
					//The factor calculates how much of the current path has already been traveled
					float factor = (float(currentTime - (cumul - path[i].second)) / float(path[i].second));

					//Then the factor is applied to each property
					backgroundMoveX = (path[i - 1].first.x() + (path[i].first.x() - path[i - 1].first.x()) * factor);
					backgroundMoveY = (path[i - 1].first.y() + (path[i].first.y() - path[i - 1].first.y()) * factor);
					backgroundZoom  = (path[i - 1].first.z() + (path[i].first.z() - path[i - 1].first.z()) * factor);
					break;
				}
			}

			float posX = player.getNode()->get_position().x();
			if (clock() > divisor_last_change + divisor_change_step_time &&
				(player.getNode()->get_position().x() > 0 && divisor_change <= max_divisor ||
				player.getNode()->get_position().x() < 0 && divisor_change > 1))
			{
				divisor_change += player.getNode()->get_position().x();
				divisor_last_change = clock();
			}
		}
		else // debug option - if isBackgroundAuto is false then move using WASD and zoom using QE
		{
			float moveSpeed = 0.1f / backgroundZoom;
			float zoomSpeed = 0.5f * backgroundZoom / 3;
			if (is_key_down(65))
				backgroundMoveX -= moveSpeed;
			if (is_key_down(68))
				backgroundMoveX += moveSpeed;
			if (is_key_down(87))
				backgroundMoveY -= moveSpeed;
			if (is_key_down(83))
				backgroundMoveY += moveSpeed;
			if (is_key_down(81) && backgroundZoom > 1.0f)
			{
				backgroundZoom -= (zoomSpeed);
			}
			if (is_key_down(69))
			{
				backgroundZoom += (zoomSpeed);
			}
			//Code added to change color palette of Mandelbrot
			if (is_key_down(key_alt) && divisor_change <= max_divisor)
			{
				divisor_change += 1;
			}

			if (is_key_down(key_backspace) && divisor_change > 1)
			{
				divisor_change -= 1;
			}

		}


		float newX = abs(player.getNode()->get_position().x() + movement);
		if (newX < roadWidth - playerSize)
		{
			player.getNode()->translate(vec3(movement, 0, 0));
		}
		if (background.node() == nullptr)
			return;


		background.node()->translate(-background.node()->get_position());
		background.node()->translate(vec3(0, player.getNode()->get_position().y(), player.getNode()->get_position().z() - backgroundDistance));
		
		
		background.mat()->set_uniform(background.zoom(), &backgroundZoom, sizeof(backgroundZoom));
		background.mat()->set_uniform(background.moveX(), &backgroundMoveX, sizeof(backgroundMoveX));
		background.mat()->set_uniform(background.moveY(), &backgroundMoveY, sizeof(backgroundMoveY));
		background.mat()->set_uniform(background.ChangeDiv(), &divisor_change, sizeof(divisor_change));
	}

	void drawBackground(const vec3 &position, const float &width, const float &height)
	{
		param_shader *shader = new param_shader("shaders/default.vs", "shaders/mandelbrot.fs");



		material* backgroundMaterial = new material(vec4(0, 0, 0, 1), shader); //to attach the material to
		backgroundMaterial->add_uniform(&width, app_utils::get_atom("width"), GL_FLOAT, 1, param::stage_fragment); // to pass parameters to shader
		backgroundMaterial->add_uniform(&height, app_utils::get_atom("height"), GL_FLOAT, 1, param::stage_fragment);
		param_uniform* paramZoom = backgroundMaterial->add_uniform(&backgroundZoom, app_utils::get_atom("zoom"), GL_FLOAT, 1, param::stage_fragment);
		param_uniform* paramMoveX = backgroundMaterial->add_uniform(&backgroundMoveX, app_utils::get_atom("moveX"), GL_FLOAT, 1, param::stage_fragment);
		param_uniform* paramMoveY = backgroundMaterial->add_uniform(&backgroundMoveY, app_utils::get_atom("moveY"), GL_FLOAT, 1, param::stage_fragment);
		param_uniform* paramChange_div = backgroundMaterial->add_uniform(&divisor_change, app_utils::get_atom("divisor"), GL_INT, 1, param::stage_fragment);


		mesh *bg = new mesh(); // attach properties, octet stuff


		size_t num_vertices = 4; // creating four vertices for the 2 triangles creating the rectangle
		size_t num_indices = 6;
		bg->allocate(sizeof(my_vertex) * num_vertices, sizeof(uint32_t) * num_indices);
		bg->set_params(sizeof(my_vertex), num_indices, num_vertices, GL_TRIANGLES, GL_UNSIGNED_INT);

		// describe the structure of my_vertex to OpenGL
		bg->add_attribute(attribute_pos, 3, GL_FLOAT, 0);


		{
			// these write-only locks give access to the vertices and indices.
			// they will be released at the next } (the end of the scope)
			gl_resource::wolock vl(bg->get_vertices());
			my_vertex *vtx = (my_vertex *)vl.u8();
			gl_resource::wolock il(bg->get_indices());
			uint32_t *idx = il.u32();
			
			vtx->pos = vec3p(-width / 2, height / 2, 0);
			vtx++;
			vtx->pos = vec3p(-width /2, -height / 2, 0);
			vtx++;
			vtx->pos = vec3p(width / 2, height / 2, 0);
			vtx++;
			vtx->pos = vec3p(width / 2, -height / 2, 0);
			// make the triangles
			uint32_t vn = 0;
				// 0--2
				// | \|
				// 1--3
			idx[0] = 0;
			idx[1] = 1;
			idx[2] = 3;
			idx[3] = 0;
			idx[4] = 3;
			idx[5] = 2;
		}


		//Add the hole to the app_scene
		scene_node *node = new scene_node();
		app_scene->add_child(node);
		app_scene->add_mesh_instance(new mesh_instance(node, bg, backgroundMaterial));

		//Move the hole to the required location, then add it to the list of holes.
		node->translate(position);

		background = Background(node, backgroundMaterial, paramZoom, paramMoveX, paramMoveY, paramChange_div);

	}

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) { 
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);
	  
      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);
	  updateCamera();

	  handleMovement();

	  if (-player.getNode()->get_position().z() + obstacleDrawDistance > lastDist)
	  {
		  createObstacle(-player.getNode()->get_position().z() + obstacleDrawDistance + obstacleGap, new mesh_sphere(vec3(0), 1), new material(vec4(0, 0, 1, 1)));
		  lastDist = -player.getNode()->get_position().z() + obstacleDrawDistance + obstacleGap;
	  }

	  //std::cout << "Player Position : ("<< player.getNode()->get_position().x() << "," << player.getNode()->get_position().y() << "," << player.getNode()->get_position().z() << ")\n";
	  std::cout << "MoveX : " << backgroundMoveX << " MoveY : " << backgroundMoveY << " Zoom : " << backgroundZoom << "\n";



      // draw the scene
      app_scene->render((float)vx / vy);


    }
  };
}
