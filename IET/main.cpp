#include <time.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glui.h>

#include "EntityManager.h"

#include "Camera.h"

#include "RigidBodyModel.h"
#include "Terrain.h"
#include "ModelBody.h"
#include "Image2D.h"

#include "HelperFunctions.h"

using namespace std;
using namespace glm;

int main_window;

enum GUI_CONTROLLER_TYPE { CAMERA_TRANSLATION, TARGET_CHANGE, BODY_ROTATION, LIGHT_ROTATION, XTOON_TEXTURE, XTOON_DISTANCE, XTOON_FOCUS, XTOON_SHININESS };
GLUI_RadioGroup *xtoonTexturesGroup;
GLUI_RadioButton * xtoonTextureButtons[4];
GLUI_Panel *xtoonDistancePanel, *xtoonBacklightPanel, *xtoonSpecularPanel; 
GLUI_Spinner *magnitude_spinner, *shininess_spinner, *focusZ_spinner;
GLUI_Rotation *bodyRotation;

// Update interval
const int FrameRate = 40;
const float DELTA_TIME = 1.0f/FrameRate;

// Shading properties
vector<GenericShader*> shaders;
int currentShaderIndex;

vec3 directionalLightDirection;
mat4 lightDirectionRotation;

float shininess;

float minZ, maxZ, focusZ;

const int textureCount = 16;
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
int xtoonTextureIDs[textureCount];
int xtoonTextureIndices[4];
int xtoonCurrentTexture;

// Camera
Camera * camera; 
int freeMode;
vec2 currentTarget;

vector<RigidBodyModel*> rigidBodies;
int currentBodyIndex;
mat4 currentBodyRotation;
//int spinning;

Image2D *textureThumb, *textureLabel;
GenericShader *imageShader;

void preloadXtoonTextures()
{
	for(int i=0; i<textureCount; ++i)
	{
		xtoonTextureIDs[i] = MeshLoader::loadTexture("xtoon\\" + xtoonTexturePaths[i]);
	}
}

void restart()
{
	for(unsigned int i=0; i<rigidBodies.size(); ++i)
	{
		delete rigidBodies[i];
	}
	rigidBodies.clear();

	rigidBodies.push_back(new RigidBodyModel(new ModelBody(string("suzanne.obj"), vec3(2.0f, 0.0f, 0.0f), quat(), vec3(12.0f, 9.0f, 8.0f)), shaders[0]));
	rigidBodies.push_back(new RigidBodyModel(new ModelBody(string("bunny.obj"), vec3(202.0f, 0.0f, 0.0f), quat(), vec3(11.0f, 12.0f, 10.0f)), shaders[0]));
	rigidBodies.push_back(new RigidBodyModel(new Terrain(vec3(402.0f, 0.0f, 0.0f), quat(), vec3(64.0f, 16.0f, 64.0f)), shaders[0]));
	rigidBodies.push_back(new RigidBodyModel(new ModelBody(string("elephal.obj"), vec3(602.0f, 0.0f, 0.0f), quat(), vec3(8.0f, 10.0f, 15.0f)), shaders[0]));

	for(unsigned int i=0; i<rigidBodies.size(); ++i)
	{
		((XToonMesh*)(rigidBodies[i]->GetModel()))->SetTexture(xtoonTextureIDs[0]);
	}

	xtoonTextureIndices[0] = xtoonTextureIndices[1] = xtoonTextureIndices[2] = xtoonTextureIndices[3] = 0;
	textureThumb = new Image2D(vec2(1.25f, 0.875f), 0.25f, xtoonTextureIDs[0]);
	textureThumb->SetShader(imageShader);
	textureLabel = new Image2D(string("xtoon\\label.png"), vec2(1.0f, 0.875f), 0.25f);
	textureLabel->SetShader(imageShader);
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

	switch(key)
	{
	case 27:
		exit(1);
		break;

	case ',':
		shininess = std::max(8.0f, shininess / 2.0f);
		for(unsigned int i=0; i<shaders.size(); ++i)
			shaders[i]->SetSpecularShininess(shininess);
		break;

	case '.':
		shininess = std::min(128.0f, shininess * 2.0f);
		for(unsigned int i=0; i<shaders.size(); ++i)
			shaders[i]->SetSpecularShininess(shininess);
		break;

	case 'c':
		freeMode = !freeMode;
		break;
	}
}

void valueModified(int id)
{
	switch((GUI_CONTROLLER_TYPE)id)
	{
	case GUI_CONTROLLER_TYPE::TARGET_CHANGE:
		freeMode = false;
		currentTarget = vec2(currentBodyIndex * 200.0f, 0.0f);

		currentBodyRotation = toMat4(rigidBodies[currentBodyIndex]->GetBody()->GetOrientation());
		bodyRotation->set_float_array_val(&currentBodyRotation[0][0]);

		shaders[currentShaderIndex]->SetXtoonType(currentBodyIndex);
		switch(currentBodyIndex)
		{
		case 0:
			xtoonDistancePanel->enable();
			focusZ_spinner->enable();
			focusZ_spinner->set_float_limits(minZ + 0.5f, maxZ - 0.5f);
			
			shaders[currentShaderIndex]->SetDistanceThresholds(minZ, maxZ);
			shaders[currentShaderIndex]->SetDistanceFocus(focusZ);

			xtoonBacklightPanel->disable();
			xtoonSpecularPanel->disable();
			break;
		case 1:		
			xtoonDistancePanel->disable();		
			xtoonBacklightPanel->enable();
			xtoonSpecularPanel->disable();

			shininess = magnitude_spinner->get_float_val();
			shaders[currentShaderIndex]->SetSpecularComponent(vec3(), 1.0f, shininess);
			break;
		case 2:
			xtoonDistancePanel->enable();
			focusZ_spinner->disable();
			xtoonBacklightPanel->disable();
			xtoonSpecularPanel->disable();

			shaders[currentShaderIndex]->SetDistanceThresholds(minZ, maxZ);
			break;
		case 3:
			xtoonDistancePanel->disable();		
			xtoonBacklightPanel->disable();
			xtoonSpecularPanel->enable();

			shininess = shininess_spinner->get_float_val();
			shaders[currentShaderIndex]->SetSpecularComponent(vec3(), 1.0f, shininess);
			break;
		}

		xtoonCurrentTexture = xtoonTextureIndices[currentBodyIndex];
		xtoonTexturesGroup->set_int_val(xtoonCurrentTexture);
		for(int i=0; i<4; ++i)
		{
			string path = xtoonTexturePaths[(currentBodyIndex * 4) + i];
			path = path.substr(0, path.length() - 4);

			xtoonTextureButtons[i]->set_name(path.c_str());
		}

		textureThumb->SetTexture(xtoonTextureIDs[currentBodyIndex * 4 + xtoonCurrentTexture]);
		break;

	case GUI_CONTROLLER_TYPE::BODY_ROTATION:
		rigidBodies[currentBodyIndex]->GetBody()->SetOrientation(quat(currentBodyRotation));
		break;

	case GUI_CONTROLLER_TYPE::LIGHT_ROTATION:
		directionalLightDirection = normalize(mat3(lightDirectionRotation) * vec3(1, 1, 1));
		shaders[currentShaderIndex]->SetDirectionalLightDirection(directionalLightDirection);
		break;

	case GUI_CONTROLLER_TYPE::XTOON_TEXTURE:
		{
			int id = xtoonTextureIDs[currentBodyIndex * 4 + xtoonCurrentTexture];
			((XToonMesh*)(rigidBodies[currentBodyIndex])->GetModel())->SetTexture(id);
			textureThumb->SetTexture(id);
			xtoonTextureIndices[currentBodyIndex] = xtoonCurrentTexture;
		}
		break;

	case GUI_CONTROLLER_TYPE::XTOON_SHININESS:
		shaders[currentShaderIndex]->SetSpecularComponent(vec3(), 1.0f, shininess);
		break;

	case GUI_CONTROLLER_TYPE::XTOON_DISTANCE:
		shaders[currentShaderIndex]->SetDistanceThresholds(minZ, maxZ);

		if(focusZ_spinner->enabled)
		{
			focusZ_spinner->set_float_limits(minZ + 0.5f, maxZ - 0.5f);
			shaders[currentShaderIndex]->SetDistanceFocus(focusZ);
		}
		break;

	case GUI_CONTROLLER_TYPE::XTOON_FOCUS:
		shaders[currentShaderIndex]->SetDistanceFocus(focusZ);
		break;
	}
}

#pragma endregion INPUT_HANDLERS

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	EntityManager::GetInstance()->DrawEntities();

	textureThumb->Render(imageShader);

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	textureLabel->Render(imageShader);
	glDisable(GL_BLEND);

	glutSwapBuffers();
}

void idle()
{  
	if ( glutGetWindow() != main_window )
		glutSetWindow(main_window);

	glutPostRedisplay();
}

void reshaped( int w, int h )
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glutPostRedisplay();
}

void update(int frame)
{
	//Update camera motion
	if(!freeMode)
	{
		vec3 eye = camera->GetEyeVector();
		vec3 target = camera->GetTargetVector();

		camera->SetEyeVector(mix(eye, vec3(currentTarget, 20), DELTA_TIME * 2));
		camera->SetTargetVector(mix(target, vec3(currentTarget, 0), DELTA_TIME * 4));

		if(abs(eye.x - currentTarget.x) < 0.5f && abs(target.x - currentTarget.x) < 0.5f)
		{
			freeMode = true;
		}
	} 

	EntityManager::GetInstance()->UpdateEntities(DELTA_TIME);

	// Re-trigger update callback
	glutTimerFunc(unsigned int(DELTA_TIME * 1000.0f), update, (frame+1) % 1000);
}

void init()
{
	// Set up the shaders
	imageShader = new GenericShader("Textured.vert", "DefaultTextured.frag");
	imageShader->SetProjectionMatrix( perspective(
		60.0f, 
		4.0f / 3.0f, 
		1.0f, 10.0f
		) );
	imageShader->SetModelMatrix(mat4(1.0f));
	imageShader->SetViewMatrix(lookAt(vec3(0,0,2.0f),vec3(0,0,0), vec3(0,1,0))); 

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
		directionalLightDirection = vec3(1, 1, 1);
		shaders[i]->SetDirectionalLightDirection(directionalLightDirection);

		shininess = 64.0f;
		shaders[i]->SetSpecularShininess(shininess);

		minZ = 10.0f;
		maxZ = 30.0f;
		shaders[i]->SetDistanceThresholds(minZ, maxZ);

		focusZ = 15.0f;
		shaders[i]->SetDistanceFocus(focusZ);
	}

	currentShaderIndex = 0;

	// Create the camera
	camera = new Camera(shaders, vec3(0,0,20), vec3(0,0,0), vec3(0,1,0));

	freeMode = true;

	currentBodyIndex = 0;

	preloadXtoonTextures();
	restart();

	// OpenGL initial setup
	glClearColor(0.06f, 0.07f, 0.08f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_DEPTH_FUNC);
	glDepthFunc(GL_LEQUAL);
} 

void initGUI()
{
	GLUI *glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_LEFT);
	glui->set_main_gfx_window( main_window );

	GLUI_Panel *entityPanel = glui->add_panel ("Scene Controls");
	GLUI_Rollout *bodyIndexRollout = glui->add_rollout_to_panel(entityPanel, "Shading Examples", 1);
	GLUI_RadioGroup *bodyIndex_selector = glui->add_radiogroup_to_panel(bodyIndexRollout, &currentBodyIndex, TARGET_CHANGE, valueModified);
	glui->add_radiobutton_to_group(bodyIndex_selector, "Level-of-Abstraction");
	glui->add_radiobutton_to_group(bodyIndex_selector, "Backlighting");
	glui->add_radiobutton_to_group(bodyIndex_selector, "Aerial Perspective");
	glui->add_radiobutton_to_group(bodyIndex_selector, "Highlighting");

	GLUI_Panel *objectPanel = glui->add_panel_to_panel (entityPanel, "Current Entity");
	bodyRotation = glui->add_rotation_to_panel(objectPanel, "Orientation", &currentBodyRotation[0][0], GUI_CONTROLLER_TYPE::BODY_ROTATION, valueModified);
	bodyRotation->reset();

	glui->add_separator();

	GLUI_Panel *lightPanel = glui->add_panel_to_panel (entityPanel, "Light Source");
	GLUI_Rotation *lightRotation = glui->add_rotation_to_panel(lightPanel, "Direction", &lightDirectionRotation[0][0], GUI_CONTROLLER_TYPE::LIGHT_ROTATION, valueModified);
	lightRotation->reset();

	glui->add_separator();

	GLUI_Panel *xtoonPanel = glui->add_panel ("X-Toon Controls");

	xtoonDistancePanel = glui->add_panel_to_panel(xtoonPanel, "Distance");
	GLUI_Spinner *minZ_spinner = glui->add_spinner_to_panel(xtoonDistancePanel, "Min Z", GLUI_SPINNER_FLOAT, &minZ, GUI_CONTROLLER_TYPE::XTOON_DISTANCE, valueModified);
	minZ_spinner->set_float_limits( 1.0f, 20.0f );

	focusZ_spinner = glui->add_spinner_to_panel(xtoonDistancePanel, "Focus Z", GLUI_SPINNER_FLOAT, &focusZ, GUI_CONTROLLER_TYPE::XTOON_FOCUS, valueModified);
	focusZ_spinner->set_float_limits( 11.0f, 29.0f );

	GLUI_Spinner *maxZ_spinner = glui->add_spinner_to_panel(xtoonDistancePanel, "Max Z", GLUI_SPINNER_FLOAT, &maxZ, GUI_CONTROLLER_TYPE::XTOON_DISTANCE, valueModified);
	maxZ_spinner->set_float_limits( 21.0f, 100.0f );

	xtoonBacklightPanel = glui->add_panel_to_panel(xtoonPanel, "Backlight");
	magnitude_spinner = glui->add_spinner_to_panel(xtoonBacklightPanel, "Magnitude", GLUI_SPINNER_FLOAT, &shininess, GUI_CONTROLLER_TYPE::XTOON_SHININESS, valueModified);
	magnitude_spinner->set_float_limits( 1.0f, 10.0f );
	xtoonBacklightPanel->disable();

	xtoonSpecularPanel = glui->add_panel_to_panel(xtoonPanel, "Specular");
	shininess_spinner = glui->add_spinner_to_panel(xtoonSpecularPanel, "Shininess", GLUI_SPINNER_FLOAT, &shininess, GUI_CONTROLLER_TYPE::XTOON_SHININESS, valueModified);
	shininess_spinner->set_float_limits( 4.0f, 128.0f );
	xtoonSpecularPanel->disable();

	GLUI_Panel *textureLabel = glui->add_panel_to_panel(xtoonPanel, "Current Texture");
	GLUI_Rollout *xtoonTextureRollout = glui->add_rollout_to_panel(textureLabel, "Texture Types", 1);
	xtoonTexturesGroup = glui->add_radiogroup_to_panel(xtoonTextureRollout, &xtoonCurrentTexture, GUI_CONTROLLER_TYPE::XTOON_TEXTURE, valueModified);	
	for(int i=0; i<4; ++i)
	{
		string path = xtoonTexturePaths[(currentBodyIndex * 4) + i];
		path = path.substr(0, path.length() - 4);

		xtoonTextureButtons[i] = glui->add_radiobutton_to_group(xtoonTexturesGroup, path.c_str());
	}

	glui->add_separator();
	glui->add_separator();

	glui->add_button("Quit",0,(GLUI_Update_CB)exit);

	GLUI_Master.set_glutIdleFunc( idle ); 
	GLUI_Master.set_glutKeyboardFunc( keyPressed );
	GLUI_Master.set_glutMouseFunc( mousePressed );
	GLUI_Master.set_glutReshapeFunc( reshaped );
}

void releaseResources()
{
	for(unsigned int i=0; i<shaders.size(); ++i)
		delete shaders[i];
	shaders.clear();

	delete imageShader;

	delete textureThumb;
	delete textureLabel;

	EntityManager::Destroy();

	GLUI_Master.close_all();
}

int main(int argc, char** argv){
	srand((unsigned int)time(NULL));

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(1024, 768);
	main_window = glutCreateWindow("[IET] CS7055 - Assignment 4");

	// Tell glut where the display function is
	glutDisplayFunc(display);

	glutIdleFunc(idle);
	glutTimerFunc(1000/FrameRate, update, 0);

	// Input handling functions
	glutMouseWheelFunc(mouseWheel);
	glutMotionFunc(mouseDragged);

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

	// GLUI setup
	initGUI();

	// Begin infinite event loop
	glutMainLoop();

	releaseResources();

	return 0;
}