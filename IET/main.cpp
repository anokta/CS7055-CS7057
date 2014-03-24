#include <time.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glui.h>

#include "EntityManager.h"

#include "Camera.h"
#include "Skybox.h"

#include "RigidBodyModel.h"
#include "Box.h"
#include "Plane.h"
#include "Terrain.h"
#include "ModelBody.h"

#include "MeshLoader.h"

#include "HelperFunctions.h"

using namespace std;
using namespace glm;

int main_window;

enum GUI_CONTROLLER_TYPE { CAMERA_TRANSLATION, TARGET_CHANGE, BODY_ROTATION, LIGHT_ROTATION, XTOON_TEXTURE, XTOON_DISTANCE };
GLUI_RadioButton * xtoonTextureButtons[4];

// Update interval
const int FrameRate = 40;
const float DELTA_TIME = 1.0f/FrameRate;

// Shading properties
vector<GenericShader*> shaders;
int currentShaderIndex;

vec3 directionalLightDirection;
float lightDirectionRotation[16];

float ambientLightIntensity;
float directionalLightIntensity;
float specularIntensity;

float roughness, shininess;

float minZ, maxZ;
string xtoonTexturePaths[] = 
{
	"default.png",
	"loa_1.png",
	"loa_2.png",
	"loa_3.png",
	"default.png",
	"backlighting_1.png",
	"backlighting_2.png",
	"backlighting_3.png",
	"default.png",
	"aerial_1.png",
	"aerial_2.png",
	"aerial_3.png",
	"default.png",
	"highlighting_1.png",
	"highlighting_2.png",
	"highlighting_3.png"
};
int xtoonCurrentTexture;

// Camera
Camera * camera; 
float cameraXY[2], cameraZ;
int freeMode;
vec2 currentTarget;

vector<RigidBodyModel*> rigidBodies;
int currentBodyIndex;
float currentBodyRotation[16];
//int spinning;

bool pause;

void restart()
{
	for(unsigned int i=0; i<rigidBodies.size(); ++i)
	{
		delete rigidBodies[i];
	}
	rigidBodies.clear();
	
	rigidBodies.push_back(new RigidBodyModel(new ModelBody(string("suzanne.obj"), vec3(2.0f, 0.0f, 0.0f), quat(), vec3(12.0f, 9.0f, 8.0f)), shaders[1]));
	rigidBodies.push_back(new RigidBodyModel(new ModelBody(string("bunny.obj"), vec3(102.0f, 0.0f, 0.0f), quat(), vec3(11.0f, 12.0f, 10.0f)), shaders[1]));
	rigidBodies.push_back(new RigidBodyModel(new Terrain(vec3(202.0f, 0.0f, 0.0f), quat(), vec3(36.0f, 16.0f, 36.0f)), shaders[1]));
	rigidBodies.push_back(new RigidBodyModel(new ModelBody(string("elephal.obj"), vec3(302.0f, 0.0f, 0.0f), quat(), vec3(8.0f, 10.0f, 15.0f)), shaders[1]));
}

void translateBody(float x, float y, float z)
{
	rigidBodies[currentBodyIndex]->GetBody()->SetLinearMomentum(12.0f * vec3(x, y, z) * rigidBodies[currentBodyIndex]->GetBody()->GetMass());
}

// Mouse & Keyboard Handler Functions
#pragma region INPUT_HANDLERS

int mouseX, mouseY;
int moveMode;

float mult = 0.5f;

float translationMult = 0.125f;

void mousePressed(int btn, int state, int x, int y)
{	
	if ( glutGetWindow() != main_window )
		glutSetWindow(main_window);

	if(state == GLUT_DOWN)
	{
		mouseX = x;
		mouseY = y;

		if(btn == GLUT_LEFT_BUTTON) 
			moveMode = 1;
		else if(btn == GLUT_RIGHT_BUTTON)
			moveMode = 2;
	}

	if(state == GLUT_UP)
	{
		moveMode = 0;
	}
}

void mouseWheel(int btn, int dir, int x, int y)
{
	vec3 forward = camera->GetTargetVector()-camera->GetEyeVector();

	if(dir<0 || glm::length(forward) > mult)
	{
		vec3 translation = vec3(vec3(toMat4(HelperFunctions::RotationBetweenVectors(vec3(0,0,-1), forward)) * vec4(0, 0, (dir>0) ? -mult : mult, 1.0f)));

		camera->SetEyeVector(camera->GetEyeVector() + translation);
		camera->SetTargetVector(camera->GetTargetVector() + translation);
	}
}

void mouseDragged(int x, int y)
{
	mat4 V = shaders[currentShaderIndex]->GetViewMatrix();

	float dx = float(x - mouseX);
	float dy = float(y - mouseY);

	vec3 forward = camera->GetTargetVector()-camera->GetEyeVector();


	if(moveMode == 2)	// rotation
	{
		mat3 rotation = toMat3(quat(vec3(-radians(dy), -radians(dx), 0.0f)));

		camera->SetTargetVector(rotation * forward + camera->GetEyeVector());
	}

	else if(moveMode == 1)	// translation
	{
		dx *= translationMult;
		dy *= translationMult;

		vec3 translation = vec3(vec3(toMat4(HelperFunctions::RotationBetweenVectors(vec3(0,0,-1), forward)) * vec4(-dx, dy, 0, 1.0f)));

		camera->SetEyeVector(camera->GetEyeVector() + translation);
		camera->SetTargetVector(camera->GetTargetVector() + translation);
	}

	mouseX = x;
	mouseY = y;
}

void keyPressed(unsigned char key, int x, int y)
{	
	if ( glutGetWindow() != main_window )
		glutSetWindow(main_window);

	if(key > 48 && key < 58)
	{
		//audioManager->ChangeSong((int)(key - 49));
		currentBodyIndex = std::min((int)(rigidBodies.size()-1), (int)(key - 49));
		return;
	}

	switch(key)
	{
	case 27:
		exit(1);
		break;

	case 'n':
		roughness = std::max(0.0f, roughness - 0.1f);
		for(unsigned int i=0; i<shaders.size(); ++i)
			shaders[i]->SetRoughness(roughness);
		break;

	case 'm':
		roughness = std::min(1.0f, roughness + 0.1f);
		for(unsigned int i=0; i<shaders.size(); ++i)
			shaders[i]->SetRoughness(roughness);
		break;

	case ',':
		shininess = std::max(8.0f, shininess / 2.0f);
		for(unsigned int i=0; i<shaders.size(); ++i)
			shaders[i]->SetSpecularComponent(vec3(1, 1, 1), specularIntensity, shininess);
		break;

	case '.':
		shininess = std::min(128.0f, shininess * 2.0f);
		for(unsigned int i=0; i<shaders.size(); ++i)
			shaders[i]->SetSpecularComponent(vec3(1, 1, 1), specularIntensity, shininess);
		break;

	case 'c':
		freeMode = !freeMode;
		break;

	case 'a':
		translateBody(-0.1f, 0.0f, 0.0f);
		break;
	case 'd':
		translateBody(0.1f, 0.0f, 0.0f);
		break;
	case 'w':
		translateBody(0.0f, 0.1f, 0.0f);
		break;
	case 's':
		translateBody(0.0f, -0.1f, 0.0f);
		break;
	case 'q':
		translateBody(0.0f, 0.0f, 0.1f);
		break;
	case 'e':
		translateBody(0.0f, 0.0f, -0.1f);
		break;
	case 'r':
		translateBody(0.0f, 0.0f, 0.0f);
		break;

	case 'p':
		pause = !pause;
		break;

	case ' ':
		if(!freeMode)
		{
			if(currentTarget == vec2())
			{
				currentTarget = vec2(40, 0); 
				currentBodyIndex = 1;
			}
			else
			{
				currentTarget = vec2();
				currentBodyIndex = 0;
			}
		}
		break;

	case '\\':
		restart();
		break;
	}
}

void valueModified(int id)
{
	switch((GUI_CONTROLLER_TYPE)id)
	{
	case GUI_CONTROLLER_TYPE::CAMERA_TRANSLATION:
		camera->SetEyeVector(vec3(cameraXY[0], cameraXY[1], -cameraZ + 20));
		camera->SetTargetVector(vec3(cameraXY[0], cameraXY[1], -cameraZ));
	
		break;

	case GUI_CONTROLLER_TYPE::TARGET_CHANGE:
		freeMode = false;
		currentTarget = vec2(currentBodyIndex * 100.0f, 0.0f);

		xtoonCurrentTexture = 0;
		for(int i=0; i<4; ++i)
		{
			string path = xtoonTexturePaths[(currentBodyIndex * 4) + i];
			path = path.substr(0, path.length() - 4);

			xtoonTextureButtons[i]->set_name(path.c_str());
		}
		break;

	case GUI_CONTROLLER_TYPE::BODY_ROTATION:
		{
			mat4 rotation = mat4(
				currentBodyRotation[0],currentBodyRotation[1],currentBodyRotation[2], currentBodyRotation[3],
				currentBodyRotation[4],currentBodyRotation[5],currentBodyRotation[6], currentBodyRotation[7],
				currentBodyRotation[8],currentBodyRotation[9],currentBodyRotation[10], currentBodyRotation[11],
				currentBodyRotation[12],currentBodyRotation[13],currentBodyRotation[14], currentBodyRotation[15]
			);
			rigidBodies[currentBodyIndex]->GetBody()->SetOrientation(quat(rotation));
		}
		break;

	case GUI_CONTROLLER_TYPE::LIGHT_ROTATION:
		{
			mat3 rotation = mat3(
				lightDirectionRotation[0],lightDirectionRotation[1],lightDirectionRotation[2],
				lightDirectionRotation[4],lightDirectionRotation[5],lightDirectionRotation[6],
				lightDirectionRotation[8],lightDirectionRotation[9],lightDirectionRotation[10]
			);
			directionalLightDirection = normalize(rotation * vec3(1, 1, 1));
		}
		for(unsigned int i=0; i<shaders.size(); ++i)
			shaders[i]->SetDirectionalLight(directionalLightDirection, vec3(1,1,1), directionalLightIntensity);
		break;
	
	case GUI_CONTROLLER_TYPE::XTOON_TEXTURE:
		((XToonMesh*)(rigidBodies[currentBodyIndex])->GetModel())->ChangeTexture("xtoon\\" + xtoonTexturePaths[currentBodyIndex * 4 + xtoonCurrentTexture]);
		break;
	
	case GUI_CONTROLLER_TYPE::XTOON_DISTANCE:
		for(unsigned int i=0; i<shaders.size(); ++i)
			shaders[i]->SetDistanceThresholds(minZ, maxZ);
		break;
	}
}

#pragma endregion INPUT_HANDLERS

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	EntityManager::GetInstance()->DrawEntities();

	glutSwapBuffers();
}

void idle()
{  
	if ( glutGetWindow() != main_window )
		glutSetWindow(main_window);

	glutPostRedisplay();
}


void update(int frame)
{
	if(!pause)
	{
		// Update audio manager
		//audioManager->Update();

		//Update camera motion
		if(!freeMode)
		{
			camera->SetEyeVector(mix(camera->GetEyeVector(), vec3(currentTarget, 20), DELTA_TIME));
			camera->SetTargetVector(mix(camera->GetTargetVector(), vec3(currentTarget, 0), DELTA_TIME * 2));

			if(abs(camera->GetEyeVector().x - currentTarget.x) < 0.5f && abs(camera->GetTargetVector().x - currentTarget.x) < 0.5f)
			{
				freeMode = true;
			}
		} 

		EntityManager::GetInstance()->UpdateEntities(DELTA_TIME);

	}
	else
	{
		camera->Update(DELTA_TIME);
	}

	// Re-trigger update callback
	glutTimerFunc(unsigned int(DELTA_TIME * 1000.0f), update, (frame+1) % 1000);
}

void init()
{
	// Set up the shaders
	GenericShader * lineShader = new GenericShader("Default.vert", "Default.frag");
	shaders.push_back(lineShader);

	GenericShader * xToonShader = new GenericShader("XToon.vert", "XToon.frag", "XToon");
	shaders.push_back(xToonShader);




	for(unsigned int i=0; i<shaders.size(); ++i)
	{
		//// Default projection matrix
		shaders[i]->SetProjectionMatrix( perspective(
			60.0f, 
			4.0f / 3.0f, 
			0.5f, 1000.0f
			) );

		// Default model matrix
		shaders[i]->SetModelMatrix(mat4(1.0f));

		// Lighting
		ambientLightIntensity = 0.2f;
		directionalLightIntensity = 0.6f;
		specularIntensity = 0.8f;

		shaders[i]->SetAmbientLight(vec3(1,1,1), ambientLightIntensity);

		directionalLightDirection = vec3(1, 1, 1);
		shaders[i]->SetDirectionalLight(directionalLightDirection, vec3(1,1,1), directionalLightIntensity);

		shininess = 64.0f;
		shaders[i]->SetSpecularComponent(vec3(1, 1, 1), specularIntensity, shininess);

		roughness = 1.0f;
		shaders[i]->SetRoughness(roughness);

		minZ = 10.0f;
		maxZ = 30.0f;
		shaders[i]->SetDistanceThresholds(minZ, maxZ);
	}

	currentShaderIndex = 2;

	// Create the camera
	camera = new Camera(shaders, vec3(0,0,20), vec3(0,0,0), vec3(0,1,0));

	freeMode = true;


	currentBodyIndex = 0;

	restart();

	pause = false;

	// OpenGL initial setup
	glClearColor(0.04f, 0.04f, 0.05f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_DEPTH_FUNC);
	glDepthFunc(GL_LEQUAL);
} 

void releaseResources()
{
	for(unsigned int i=0; i<shaders.size(); ++i)
		delete shaders[i];
	shaders.clear();

	EntityManager::Destroy();

}

int main(int argc, char** argv){
	srand((unsigned int)time(NULL));

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(1024, 768);
	main_window = glutCreateWindow("[IET] CS7055");

	// Tell glut where the display function is
	glutDisplayFunc(display);

	//glutIdleFunc(idle);
	glutTimerFunc(1000/FrameRate, update, 0);

	// Input handling functions
	//glutMouseFunc(mousePressed);
	glutMouseWheelFunc(mouseWheel);
	glutMotionFunc(mouseDragged);

	//glutKeyboardFunc(keyPressed);
	//glutSpecialFunc(specialKeyPressed);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();

	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	// Entity handler
	EntityManager::Create();

	// Set up your objects and shaders
	init();

	// GLUI init  
	GLUI *glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_LEFT);
	glui->set_main_gfx_window( main_window );

	// Controls  
	GLUI_Panel *cameraPanel = glui->add_panel ("Camera Controls");
	GLUI_Translation * xyTranslation = glui->add_translation_to_panel(cameraPanel, "Move", GLUI_TRANSLATION_XY, cameraXY, GUI_CONTROLLER_TYPE::CAMERA_TRANSLATION, valueModified);
	GLUI_Translation * zTranslation = glui->add_translation_to_panel(cameraPanel, "Zoom", GLUI_TRANSLATION_Z, &cameraZ, GUI_CONTROLLER_TYPE::CAMERA_TRANSLATION, valueModified);
	
	glui->add_separator();

	GLUI_Panel *entityPanel = glui->add_panel ("Entity Controls");
	GLUI_Rollout *bodyIndexRollout = glui->add_rollout_to_panel(entityPanel, "Shading Examples", 1);
	GLUI_RadioGroup *bodyIndex_selector = glui->add_radiogroup_to_panel(bodyIndexRollout, &currentBodyIndex, TARGET_CHANGE, valueModified);
	glui->add_radiobutton_to_group(bodyIndex_selector, "Level-of-Abstraction");
	glui->add_radiobutton_to_group(bodyIndex_selector, "Backlighting");
	glui->add_radiobutton_to_group(bodyIndex_selector, "Aerial Perspective");
	glui->add_radiobutton_to_group(bodyIndex_selector, "Highlighting");

	GLUI_Rotation *bodyRotation = glui->add_rotation_to_panel(entityPanel, "Orientation", currentBodyRotation, GUI_CONTROLLER_TYPE::BODY_ROTATION, valueModified);
	bodyRotation->reset();

	glui->add_separator();

	GLUI_Panel *lightPanel = glui->add_panel ("Light Source");
	GLUI_Rotation *lightRotation = glui->add_rotation_to_panel(lightPanel, "Direction", lightDirectionRotation, GUI_CONTROLLER_TYPE::LIGHT_ROTATION, valueModified);
	lightRotation->reset();

	glui->add_separator();

	GLUI_Panel *xtoonPanel = glui->add_panel ("X-Toon Controls");
	
	GLUI_Panel *xtoonDistancePanel = glui->add_panel_to_panel(xtoonPanel, "Distance");
	GLUI_Spinner *minZ_spinner = glui->add_spinner_to_panel(xtoonDistancePanel, "Min Z", GLUI_SPINNER_FLOAT, &minZ, GUI_CONTROLLER_TYPE::XTOON_DISTANCE, valueModified);
	minZ_spinner->set_float_limits( 1.0f, 20.0f );
	GLUI_Spinner *maxZ_spinner = glui->add_spinner_to_panel(xtoonDistancePanel, "Max Z", GLUI_SPINNER_FLOAT, &maxZ, GUI_CONTROLLER_TYPE::XTOON_DISTANCE, valueModified);
	maxZ_spinner->set_float_limits( 21.0f, 100.0f );
	
	GLUI_Panel *textureLabel = glui->add_panel_to_panel(xtoonPanel, "Current Texture");
	GLUI_Rollout *xtoonTextureRollout = glui->add_rollout_to_panel(textureLabel, "Texture Types", 0);
	GLUI_RadioGroup *xtoonTextures = glui->add_radiogroup_to_panel(xtoonTextureRollout, &xtoonCurrentTexture, GUI_CONTROLLER_TYPE::XTOON_TEXTURE, valueModified);	
	for(int i=0; i<4; ++i)
	{
		string path = xtoonTexturePaths[(currentBodyIndex * 4) + i];
		path = path.substr(0, path.length() - 4);

		xtoonTextureButtons[i] = glui->add_radiobutton_to_group(xtoonTextures, path.c_str());
	}

	glui->add_separator();
	glui->add_separator();

	glui->add_button("Quit",0,(GLUI_Update_CB)exit);

	GLUI_Master.set_glutIdleFunc( idle ); 
	GLUI_Master.set_glutKeyboardFunc( keyPressed );
	GLUI_Master.set_glutMouseFunc( mousePressed );


	// Begin infinite event loop
	glutMainLoop();

	releaseResources();

	return 0;
}