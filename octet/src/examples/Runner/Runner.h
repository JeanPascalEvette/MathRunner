////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//


#include <sstream>
namespace octet {
  /// Scene containing a box with octet.
	

  class Runner : public app {




	// Container class to store the different elements of an entity in octet
	class GameObject {
	public:
		GameObject() {}
		GameObject(ref<scene_node> node, btRigidBody* rigidBody, ref<mesh_instance> meshInstance, int _bonusType)
		{
			_node = node;
			_rigidBody = rigidBody;
			_meshInstance = meshInstance;
			bonusType = _bonusType;
		}

		ref<scene_node> getNode() { return _node; }
		btRigidBody* getRigidBody() { return _rigidBody; }
		ref<mesh_instance> getMeshInstance() { return _meshInstance; }
		int getBonusType() { return bonusType; }
		

	private:
		int bonusType;
		ref<scene_node> _node;
		btRigidBody* _rigidBody;
		ref<mesh_instance> _meshInstance;
	};


	//Container class used to store the different param_uniform that can be use to modify the properties of the background shader
	class Background {
	public:
		Background() {}
		Background(ref<scene_node> node, ref<material> mat, ref<param_uniform> zoom, ref<param_uniform> moveX, ref<param_uniform> moveY, ref<param_uniform> ChangeDiv, ref<param_uniform> CRe, ref<param_uniform> CIm, ref<param_uniform> divRGB)
		{
			_node = node;
			_mat = mat;
			_zoom = zoom;
			_moveX = moveX;
			_moveY = moveY;
			_ChangeDiv = ChangeDiv;
			_CRe = CRe;
			_CIm = CIm;
			_divRGB = divRGB;
		}

		ref<param_uniform> divRGB() { return _divRGB; }
		ref<param_uniform> moveX() { return _moveX; }
		ref<param_uniform> moveY() { return _moveY; }
		ref<param_uniform> ChangeDiv() { return _ChangeDiv; }
		ref<param_uniform> zoom() { return _zoom; }
		ref<param_uniform> real() { return _CRe; }
		ref<param_uniform> im() { return _CIm; }
		ref<material> mat() { return _mat; }
		ref<scene_node> node() { return _node; }
	private:
		ref<material> _mat;
		ref<param_uniform> _zoom;
		ref<param_uniform> _moveX;
		ref<param_uniform> _moveY;
		ref<param_uniform> _ChangeDiv;
		ref<param_uniform> _CRe;
		ref<param_uniform> _CIm;
		ref<param_uniform> _divRGB;
		ref<scene_node> _node;
	};
    // scene for drawing box
	
    ref<visual_scene> app_scene;
	Background background;
	bool freeCamera;
	bool isDebug;
	GameObject player;
	std::vector<GameObject> listGameObjects;
	ref<param_uniform> paramMinimapCRe;
	ref<param_uniform> paramMinimapCIm;
	ref<scene_node> minimapNode;
	ref<material> minimapMaterial;
	ref<text_overlay> myText;
	ref<mesh_text> myInfoText;
	

	int obstacleDrawDistance;
	int obstacleGap;
	int roadWidth;
	int playerSize;
	int backgroundDistance;
	int currentPickupType;
	float backgroundZoom;
	float backgroundMoveX;
	float backgroundMoveY;
	vec4 divRGB;
	float cRe;
	float cIm;
	time_t divisor_last_change;
	time_t divisor_change_step_time;
	int divisor_change;
	int max_divisor;
	int lastDist;
	int speed_type;
	float speedIm;
	float speedRe;
	float speedIncrease;
	float maxspeed;
	int currentDivRGB;
	std::vector<vec4> divRGBList;
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

	  srand(static_cast<unsigned int>(time(0)));

	  //Init text

	  aabb bb(vec3(64.5f, 305.0f, 0.0f), vec3(256, 64, 0));
	  myText = new text_overlay();
	  myInfoText = new mesh_text(myText->get_default_font(), "", &bb);
	  myText->add_mesh_text(myInfoText);


	  //Debug only - if TRUE, lets the user move the camera with the mouse
	  freeCamera = false;

	  //Debug only - if TRUE, lets the user use the Zoom In/Out hotkeys and move through the Mandelbrot through the WASD keys
	  isDebug = false;
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
	  app_scene->get_camera_instance(0)->set_far_plane(1000.0f);
	  obstacleDrawDistance = 450;
	  obstacleGap = 100;
	  backgroundDistance = 500;
	  background = Background();
	  roadWidth = 8;
	  playerSize = 1;
	  lastDist = obstacleDrawDistance;
	  listGameObjects = std::vector<GameObject>();
	  currentPickupType = 0;
	  currentDivRGB = 0;
	  max_divisor = 500;

	  //Generate list of color sets
	  divRGBList = std::vector<vec4>();
	  divRGBList.push_back(vec4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f));
	  divRGBList.push_back(vec4(1.0 / 3.0, 2.0 / 3.0, 1.0, 3.0));
	  divRGBList.push_back(vec4(1.0 / 2.0, 1.0 / 3.0, 1.0, 3.0));
	  divRGBList.push_back(vec4(1.0 / 3.0, 1.0 / 3.0, 1.0, 3.0));
	  divRGBList.push_back(vec4(1.0 / 2.0, 2.0 / 1.0, 1.0, 3.0));
	  divRGBList.push_back(vec4(1.0 / 2.0, 2.0 / 1.0, 1.0, 1.0));
	  divRGB = divRGBList[currentDivRGB];

	  //Initial position in Julia Set
	  backgroundZoom = 0.2f;
	  backgroundMoveX = 0.0f;
	  backgroundMoveY = 0.0f;

	  //Initial position in Mandelbrot set
	  cRe = 0.0f;
	  cIm = 0.0f;


	  divisor_change = 150;
	  divisor_last_change = clock();
	  divisor_change_step_time = 100;
	  speed_type = 1;
	  speedIm = 0.0f;
	  speedRe = 0.0f;
	  maxspeed = 0.005f;
	  speedIncrease = 0.0001f;


	  material *red = new material(vec4(1, 0, 0, 1));
	  material *blue = new material(vec4(0, 0, 1, 1));
	  material *purple = new material(vec4(1, 0, 1, 1));


	  mat4t mat;

	  mat.translate(0, -8, 0);
	  player = createGameObject(mat, new mesh_box(vec3(playerSize)), purple, true, 10.0f);

	  //Draw Julia & Mandelbrot sets
	  drawBackground(vec3(0, 0, player.getNode()->get_position().z() - backgroundDistance), 800, 550);
	  drawMinimap();


    }

	//This function creates a pickup at a set distance from the player
	void createObstacle(float distanceFromPlayer, mesh *msh, material *mtl, int bonusType)
	{
		float xCoord = (rand() % 8) - 4.0f;
		vec3 relativePos = vec3(xCoord, 0, -distanceFromPlayer);
		mat4t mat;
		mat.translate(0, player.getNode()->get_position().y(),player.getNode()->get_position().z()); //Translate to player position
		mat.translate(relativePos); //Translate to pickup's position
		listGameObjects.push_back(createGameObject(mat, msh, mtl, bonusType, true, 99999.0f));
	}

	//This function creates a new shape in the world based on the parameters, then returns a GameObject object to store the node, rigidbody and mesh instance
	GameObject createGameObject(mat4t_in mat, mesh *msh, material *mtl, int bonusType, bool is_dynamic = false, float mass = 1)
	{
		btRigidBody* rigidBody = NULL;
		scene_node* node = NULL;
		ref<mesh_instance> mi = app_scene->add_shape(mat, msh, mtl, is_dynamic, mass, &rigidBody, &node);
		return GameObject(node, rigidBody, mi, bonusType);
	}

	//This allow the player to move the camera with the mouse. Debug only. Will not run if the freeCamera boolean is false.
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

	//This function update the camera location based on the location of the player.
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


	
	void handleInputs()
	{

		//Move the player forward automatically. 
		player.getNode()->translate(vec3(0, 0, -5.0f));

		//Player movement Left/Right
		float movement = 0.0f;
		if (is_key_down(key_left) && !is_key_down(key_shift))
			movement -= 0.5f;
		if (is_key_down(key_right) && !is_key_down(key_shift))
			movement += 0.5f;
		float newX = abs(player.getNode()->get_position().x() + movement);
		if (newX < roadWidth - playerSize)
		{
			player.getNode()->translate(vec3(movement, 0, 0));
		}

		
		if (isDebug) // debug option - if isDebug is true then move using WASD and zoom using QE
		{
			//Move in Mandelbrot set using WASD
			float moveSpeed = 0.0005f;
			if (is_key_down(65))
				cRe -= moveSpeed;
			if (is_key_down(68))
				cRe += moveSpeed;
			if (is_key_down(87))
				cIm -= moveSpeed;
			if (is_key_down(83))
				cIm += moveSpeed;


			//Zoom In/Out using Q and E keys
			//Zoom speed is depending on current zoom level
			float zoomSpeed = 0.5f * backgroundZoom / 3;
			if (is_key_down(81) && backgroundZoom > 1.0f)
			{
				backgroundZoom -= (zoomSpeed);
			}
			if (is_key_down(69))
			{
				backgroundZoom += (zoomSpeed);
			}
			

		}// Rest of the code will run whether or not debug mode is enabled


		//Arrow keys can be used to control the location in the Julia set if the SHIFT key is pressed
		float moveSpeedNew = 0.1f / backgroundZoom;
		if (is_key_down(key_left) && is_key_down(key_shift))
			backgroundMoveX -= moveSpeedNew;
		if (is_key_down(key_right) && is_key_down(key_shift))
			backgroundMoveX += moveSpeedNew;
		if (is_key_down(key_down) && is_key_down(key_shift))
			backgroundMoveY += moveSpeedNew;
		if (is_key_down(key_up) && is_key_down(key_shift))
			backgroundMoveY -= moveSpeedNew;


		//Change the number of colors used ot represent the Julia set using the Backspace and Ctrl keys
		if (is_key_down(key_ctrl) && divisor_change <= max_divisor)
		{
			divisor_change += 1;
		}

		if (is_key_down(key_backspace) && divisor_change > 1)
		{
			divisor_change -= 1;
		}

		//Zoom out slowly using the space key
		if (is_key_down(key_space) && (backgroundZoom>0.231f))
		{
			backgroundZoom -= 0.03f;
		}



		if (background.node() == nullptr)
			return;


		//If F1 is pressed - iterate through the list of color sets
		if (is_key_going_down(key_f1))
		{
			divRGB = divRGBList[++currentDivRGB % divRGBList.size()];
		}
	}


	//This function goes through every Pickup and applies their effects if the player is in collision with it.
	void handleCollisions()
	{
		for (int i = 0; i < listGameObjects.size(); ++i)
		{
			//If Player is at < than 1.5f from the pickup in the X and Z direction.
			//Y direction is much higher because we want the player to be able to "keep" pickups by holding them above the character.
			if ((abs((player.getNode()->get_position().x()) - (listGameObjects[i].getNode()->get_position().x())) < 1.5f)
				&& ((abs((player.getNode()->get_position().z()) - (listGameObjects[i].getNode()->get_position().z())) < 1.5f))
				&& ((abs((player.getNode()->get_position().y()) - (listGameObjects[i].getNode()->get_position().y())) < 20.0f)))
			{
				//Depending on the bonus type, apply a different effect on the Imaginary and Real velocities
				//After applying the effect, also move the bonus above the player's character and make it move at the same velocity as the player in the Z direction
				//This means that the pickup will stay above the player until the player moves in such a way that the collision doesn't register anymore.
				//Also, if the player collides with a new pickup, it will replace the current one.
				switch (listGameObjects[i].getBonusType()) {
					case 1: 
						if (speedIm < maxspeed) // If the player already has the max speed - get rid of the pickup
						{
							speedIm += speedIncrease;
							listGameObjects[i].getNode()->translate(vec3(0.0f, 0.0f, -5.0f));
							if ((player.getNode()->get_position().y()) == (listGameObjects[i].getNode()->get_position().y()))
							{
								remove_current_bonus();
								listGameObjects[i].getNode()->translate(vec3(0.0f, 2.0f, 0.0f));
							}
						}
						else 
						{ 
							listGameObjects[i].getNode()->translate(vec3(30.0f, 0.0f, 0.0f)); 
						}
							break;
					case 2: 
						if (-speedIm < maxspeed)
						{
							speedIm -= speedIncrease;
							listGameObjects[i].getNode()->translate(vec3(0.0f, 0.0f, -5.0f));
							if ((player.getNode()->get_position().y()) == (listGameObjects[i].getNode()->get_position().y()))
							{
								remove_current_bonus();
								listGameObjects[i].getNode()->translate(vec3(0.0f, 2.0f, 0.0f));
							}
						}
						else 
						{ 
							listGameObjects[i].getNode()->translate(vec3(30.0f, 0.0f, 0.0f)); 
						}

							break;
					case 3: 
						if (speedRe < maxspeed)
						{
							speedRe += speedIncrease;
							listGameObjects[i].getNode()->translate(vec3(0.0f, 0.0f, -5.0f));
							if ((player.getNode()->get_position().y()) == (listGameObjects[i].getNode()->get_position().y()))
							{
								remove_current_bonus();
								listGameObjects[i].getNode()->translate(vec3(0.0f, 2.0f, 0.0f));
							}
						}
						else 
						{ 
							listGameObjects[i].getNode()->translate(vec3(30.0f, 0.0f, 0.0f)); 
						}
							break;
					case 4: 
						if (-speedRe < maxspeed)
						{
							speedRe -= speedIncrease;

							listGameObjects[i].getNode()->translate(vec3(0.0f, 0.0f, -5.0f));
							if ((player.getNode()->get_position().y()) == (listGameObjects[i].getNode()->get_position().y()))
							{
								remove_current_bonus();
								listGameObjects[i].getNode()->translate(vec3(0.0f, 2.0f, 0.0f));
							}
						}
						else 
						{ 
							listGameObjects[i].getNode()->translate(vec3(30.0f, 0.0f, 0.0f)); 
						}
							break;
					case 5:
						speedRe = 0.0f;
						speedIm = 0.0f;
						listGameObjects[i].getNode()->translate(vec3(30.0f, 0.0f, 0.0f));
						remove_current_bonus();
						break;
				}
			}
		}

		//If the player has gone outside of the Mandelbrot set, move him back at the initial location, with the initial velocities and zoom
		if (cRe>0.50f || cRe<-1.7f || abs(cIm)>1.1f)
		{
			speedIm = 0.0f;
			speedRe = 0.0f;

			cRe = 0.0f;
			cIm = 0.0f;

			backgroundZoom = 0.2f;
		}
	}

	//This function iterates through the pickups and removes any it finds above the player.
	//This is called only when the player collides with a pick up to remove the previous pick up
	void remove_current_bonus() {
		if (listGameObjects.size() == 0) return;
		std::vector<GameObject>::iterator it;
		for (it = listGameObjects.begin(); it != listGameObjects.end(); it++)
		{
			if ((*it).getNode()->get_position().y() > -8.0f)
				it->getNode()->translate(vec3(30.0f, 0.0f, 0.0f));
		}
	}


	//This function is called to create the shader for the Julia Set that is displayed in the background of the game
	void drawBackground(const vec3 &position, const float &width, const float &height)
	{
		param_shader *shader = new param_shader("shaders/default.vs", "shaders/julia.fs");



		material* backgroundMaterial = new material(vec4(0, 0, 0, 1), shader); //to attach the material to
		backgroundMaterial->add_uniform(&width, app_utils::get_atom("width"), GL_FLOAT, 1, param::stage_fragment); // to pass parameters to shader
		backgroundMaterial->add_uniform(&height, app_utils::get_atom("height"), GL_FLOAT, 1, param::stage_fragment);
		param_uniform* paramZoom = backgroundMaterial->add_uniform(&backgroundZoom, app_utils::get_atom("zoom"), GL_FLOAT, 1, param::stage_fragment);
		param_uniform* paramMoveX = backgroundMaterial->add_uniform(&backgroundMoveX, app_utils::get_atom("moveX"), GL_FLOAT, 1, param::stage_fragment);
		param_uniform* paramMoveY = backgroundMaterial->add_uniform(&backgroundMoveY, app_utils::get_atom("moveY"), GL_FLOAT, 1, param::stage_fragment);
		param_uniform* paramChange_div = backgroundMaterial->add_uniform(&divisor_change, app_utils::get_atom("divisor"), GL_INT, 1, param::stage_fragment);
		param_uniform* paramCRe = backgroundMaterial->add_uniform(&cRe, app_utils::get_atom("cRe"), GL_FLOAT, 1, param::stage_fragment);
		param_uniform* paramCIm = backgroundMaterial->add_uniform(&cIm, app_utils::get_atom("cIm"), GL_FLOAT, 1, param::stage_fragment);
		param_uniform* paramDivRGB = backgroundMaterial->add_uniform(&divRGB, app_utils::get_atom("divRGB"), GL_FLOAT_VEC4, 1, param::stage_fragment);


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


		//Add the node to the app_scene
		scene_node *node = new scene_node();
		app_scene->add_child(node);
		app_scene->add_mesh_instance(new mesh_instance(node, bg, backgroundMaterial));

		//Move the node to the required location, then add it to the list of holes.
		node->translate(position);

		//Store the uniforms in an accessible object
		background = Background(node, backgroundMaterial, paramZoom, paramMoveX, paramMoveY, paramChange_div, paramCRe, paramCIm, paramDivRGB);

	}


	//This function is called to create the shader for the Mandelbrot Set that is displayed in the minimap
	void drawMinimap()
	{
		param_shader *shader = new param_shader("shaders/default.vs", "shaders/mandelbrot.fs");

		float width = 5.0f;
		float height = 5.0f;

		minimapMaterial = new material(vec4(0, 0, 0, 1), shader); //to attach the material to
		minimapMaterial->add_uniform(&width, app_utils::get_atom("width"), GL_FLOAT, 1, param::stage_fragment); // to pass parameters to shader
		minimapMaterial->add_uniform(&height, app_utils::get_atom("height"), GL_FLOAT, 1, param::stage_fragment);
		paramMinimapCRe = minimapMaterial->add_uniform(&cRe, app_utils::get_atom("cRe"), GL_FLOAT, 1, param::stage_fragment);
		paramMinimapCIm = minimapMaterial->add_uniform(&cIm, app_utils::get_atom("cIm"), GL_FLOAT, 1, param::stage_fragment);


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
			vtx->pos = vec3p(-width / 2, -height / 2, 0);
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


		//Add the node to the app_scene
		scene_node *node = new scene_node();
		app_scene->add_child(node);
		app_scene->add_mesh_instance(new mesh_instance(node, bg, minimapMaterial));

		//Move the node to the required location, then add it to the list of holes.
		node->translate(vec3(0,0,player.getNode()->get_position().z()));

		//Store the node in a field.
		minimapNode = node;
	}

	
	//This function iterates through the pickups and deletes any which is behind the player - keeps the number of active pickups between 4 and 7
	void deleteObstacles()
	{
		if (listGameObjects.size() == 0) return;
		std::vector<GameObject>::iterator it;

		for (it = listGameObjects.begin(); it != listGameObjects.end(); it = listGameObjects.begin())
		{
			if ((*it).getNode()->get_position().z() > player.getNode()->get_position().z())
			{   
				app_scene->delete_mesh_instance((*it).getMeshInstance());
				app_scene->getWorld()->removeRigidBody((*it).getRigidBody());
				listGameObjects.erase(it);
				if (listGameObjects.size() == 0) return;
			}
			else
				break;
		}
		listGameObjects.shrink_to_fit();
	}

	//This function will update the text display at the top of the screen.
	void updateText(int vx, int vy)
	{
		myInfoText->clear();
		// write some text to the overlay

		std::stringstream text;
		float factor = 1.0f / speedIncrease;
		text << "Real Speed : " << std::to_string(int(speedRe * factor))  << " Real Value : " << std::to_string(cRe) << "\nImaginary Speed : " << std::to_string(int(speedIm * factor)).substr(0, 7) << " Imaginary Value : " << std::to_string(cIm).substr(0, 7) << "\nZoom Value : " << std::to_string(backgroundZoom).substr(0, 7);

		myInfoText->format(text.str().c_str());



		// convert it to a mesh.
		myInfoText->update();
		// draw the text overlay

		myText->render(vx, vy);
	}

	//This function updates the data that is sent to the shaders to update the Mandelbrot and Julia sets
	void updateShaders()
	{
		//Update Shaders position
		background.node()->translate(-background.node()->get_position());
		background.node()->translate(vec3(player.getNode()->get_position().x(), player.getNode()->get_position().y(), player.getNode()->get_position().z() - backgroundDistance));
		minimapNode->translate(-minimapNode->get_position());
		minimapNode->translate(vec3(6.8f + app_scene->get_camera_instance(0)->get_node()->get_position().x(), 3.5f, player.getNode()->get_position().z()));

		//Update Shaders uniforms
		background.mat()->set_uniform(background.zoom(), &backgroundZoom, sizeof(backgroundZoom));
		background.mat()->set_uniform(background.moveX(), &backgroundMoveX, sizeof(backgroundMoveX));
		background.mat()->set_uniform(background.moveY(), &backgroundMoveY, sizeof(backgroundMoveY));
		background.mat()->set_uniform(background.im(), &cIm, sizeof(cIm));
		background.mat()->set_uniform(background.real(), &cRe, sizeof(cRe));
		background.mat()->set_uniform(background.ChangeDiv(), &divisor_change, sizeof(divisor_change));
		background.mat()->set_uniform(background.divRGB(), &divRGB, sizeof(divRGB));
		minimapMaterial->set_uniform(paramMinimapCIm, &cIm, sizeof(cIm));
		minimapMaterial->set_uniform(paramMinimapCRe, &cRe, sizeof(cRe));
	}

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) { 
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);
	  
	  cIm += speedIm;
	  cRe += speedRe;

	  if (backgroundZoom < 8.0f)
	  {
		  backgroundZoom += 0.00035f;
	  }
	  
      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);
	  updateCamera();

	  handleInputs();
	  handleCollisions();
	  updateShaders();

	  deleteObstacles();



	  //Creates a new pick up if the gap with the previous one is big enough
		if (-player.getNode()->get_position().z() + obstacleDrawDistance > lastDist + obstacleGap)
		{
			mesh* myMesh = nullptr;
			material* myMaterial = nullptr;


			if (currentPickupType == 0) {
				myMesh = new mesh_sphere(vec3(0), 1);
				myMaterial = new material(vec4(0, 1, 0, 1));
				  
			}
			else if (currentPickupType == 1) {
				myMesh = new mesh_sphere(vec3(0), 1);
				myMaterial = new material(vec4(0, 0, 1, 1));
				 
			}

			else if (currentPickupType == 2) {
				myMesh = new mesh_box(vec3(1.0f));
				myMaterial = new material(vec4(0, 1, 0, 1));
				  
			}

			else if (currentPickupType == 3) {
				myMesh = new mesh_box(vec3(1.0f));
				myMaterial = new material(vec4(0, 0, 1, 1));
				  
			}

			else {
				myMesh = new mesh_sphere(vec3(0), 1);
				myMaterial = new material(vec4(1, 1, 1, 1));
				 
			}

			createObstacle(obstacleDrawDistance, myMesh, myMaterial, currentPickupType + 1);
			lastDist = -player.getNode()->get_position().z() + obstacleDrawDistance;

			if (currentPickupType >= 4)
			{
				currentPickupType = 0;
			}
			else { currentPickupType++; }

		}


	  
      // draw the scene
      app_scene->render((float)vx / vy);


	  //draw text
	  updateText(vx, vy);

    }
  };
}
