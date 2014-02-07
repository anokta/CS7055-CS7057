#include <time.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "EntityManager.h"
#include "AudioManager.h"

#include "Camera.h"

#include "RigidBodyModel.h"
#include "Box.h"
#include "Ball.h"
#include "Ellipsoid.h"
#include "Cat.h"

#include "HelperFunctions.h"

#include "MeshLoader.h"

using namespace std;
using namespace glm;

const int FrameRate = 40;
const float DELTA_TIME = 1.0f/FrameRate;

AudioManager * audioManager;

vector<GenericShader*> shaders;
int currentShaderIndex;

vec3 directionalLightDirection;

float ambientLightIntensity;
float directionalLightIntensity;
float specularIntensity;

float roughness, shininess;

vector<RigidBodyModel*> rigidBodies;
int currentBodyIndex;

Camera * camera; 
bool freeMode;

vec3 currentColor, nextColor;

float gravity = -2.0f;

bool pause;


void rotateBody(float x, float y, float z)
{
	quat q(vec3(x, y, z));
	rigidBodies[currentBodyIndex]->GetBody()->SetOrientation(rigidBodies[currentBodyIndex]->GetBody()->GetOrientation() * q);
}

void translateBody(float x, float y, float z)
{
	rigidBodies[currentBodyIndex]->GetBody()->SetPosition(rigidBodies[currentBodyIndex]->GetBody()->GetPosition() + vec3(x, y, z));
}

// Mouse & Keyboard Handler Functions
#pragma region INPUT_HANDLERS

int mouseX, mouseY;
int moveMode;

float mult = 0.5f;

float translationMult = 0.125f;

void mousePressed(int btn, int state, int x, int y)
{
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

void specialKeyPressed(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_LEFT:
		directionalLightDirection = normalize(toMat3(quat(vec3(0.0f, -0.1f, 0.0f))) * directionalLightDirection);
		for(unsigned int i=0; i<shaders.size(); ++i)
			shaders[i]->SetDirectionalLight(directionalLightDirection, vec3(1,1,1), directionalLightIntensity);
		break;
	case GLUT_KEY_RIGHT:
		directionalLightDirection = normalize(toMat3(quat(vec3(0.0f, 0.1f, 0.0f))) * directionalLightDirection);
		for(unsigned int i=0; i<shaders.size(); ++i)
			shaders[i]->SetDirectionalLight(directionalLightDirection, vec3(1,1,1), directionalLightIntensity);
		break;
	case GLUT_KEY_UP:
		directionalLightDirection = normalize(toMat3(quat(vec3(-0.1f, 0.0f, 0.0f))) * directionalLightDirection);
		for(unsigned int i=0; i<shaders.size(); ++i)
			shaders[i]->SetDirectionalLight(directionalLightDirection, vec3(1,1,1), directionalLightIntensity);
		break;
	case GLUT_KEY_DOWN:
		directionalLightDirection = normalize(toMat3(quat(vec3(0.1f, 0.0f, 0.0f))) * directionalLightDirection);
		for(unsigned int i=0; i<shaders.size(); ++i)
			shaders[i]->SetDirectionalLight(directionalLightDirection, vec3(1,1,1), directionalLightIntensity);
		break;
	}
}

void keyPressed(unsigned char key, int x, int y)
{
	if(key > 48 && key < 58)
	{
		currentBodyIndex = std::min((int)(rigidBodies.size()-1), (int)(key - 49));
		//audioManager->ChangeSong(key-49);
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

	case 'f':
		RigidBodyModel::ToggleGizmos();
		break;

	case 'g':
		gravity *= -1;
		break;

	case 8:
		currentShaderIndex = (currentShaderIndex + shaders.size() - 3) % (shaders.size() - 1);

		for(unsigned int i=0; i<rigidBodies.size(); ++i)
		{
			if(rigidBodies[i]->GetBody()->GetType() == RigidBody::ELLIPSOID)
				rigidBodies[i]->ChangeShader(shaders[currentShaderIndex]);
			else if(rigidBodies[i]->GetBody()->GetType() == RigidBody::BOX || rigidBodies[i]->GetBody()->GetType() == RigidBody::CAT || rigidBodies[i]->GetBody()->GetType() == RigidBody::BALL)
				rigidBodies[i]->ChangeShader(shaders[currentShaderIndex+1]);
		}

		cout << "Current shader pair: " << shaders[currentShaderIndex]->GetName() << endl << endl;

		break;
	case 13:
		currentShaderIndex = (currentShaderIndex + 2) % (shaders.size() - 1);

		for(unsigned int i=0; i<rigidBodies.size(); ++i)
		{
			if(rigidBodies[i]->GetBody()->GetType() == RigidBody::ELLIPSOID)
				rigidBodies[i]->ChangeShader(shaders[currentShaderIndex]);
			else if(rigidBodies[i]->GetBody()->GetType() == RigidBody::BOX || rigidBodies[i]->GetBody()->GetType() == RigidBody::CAT || rigidBodies[i]->GetBody()->GetType() == RigidBody::BALL)
				rigidBodies[i]->ChangeShader(shaders[currentShaderIndex+1]);
		}

		cout << "Current shader pair: " << shaders[currentShaderIndex]->GetName() << endl << endl;

		break;

	case 'u':
		rotateBody(-0.1f, 0.0f, 0.0f);
		break;

	case 'j':
		rotateBody(0.1f, 0.0f, 0.0f);
		break;

	case 'h':
		rotateBody(0.0f, -0.1f, 0.0f);
		break;

	case 'k':
		rotateBody(0.0f, 0.1f, 0.0f);
		break;

	case 'i':
		rotateBody(0.0f, 0.0f, -0.1f);
		break;

	case 'y':
		rotateBody(0.0f, 0.0f, 0.1f);
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

	case 'p':
		pause = !pause;
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
	glutPostRedisplay();
}

void update(int frame)
{
	if(!pause)
	{
		// Update audio particleSystem
		audioManager->Update();

		// Update lighting
		if (currentColor != nextColor)
			currentColor = mix(currentColor, nextColor, 16 * DELTA_TIME);

		//Update camera motion
		if(!freeMode)
		{
			camera->SetEyeVector(mix(camera->GetEyeVector(), vec3(0, 0, 10), DELTA_TIME));
			camera->SetTargetVector(mix(camera->GetTargetVector(), vec3(0, 0, 0), DELTA_TIME * 2));
		} 

		// Update entitities

		if(rigidBodies.size() < 20)
		{
			if(rand() % 10 < 5)
			{
				rigidBodies.push_back(
					new RigidBodyModel(new Box(vec3(float(rand())/RAND_MAX * 40.0f - 20.0f, gravity < 0 ? float(rand())/RAND_MAX * 40.0f + 20.0f : -float(rand())/RAND_MAX * 40.0f - 20.0f,float(rand())/RAND_MAX * 20.0f - 20.0f), 
					quat(vec3(float(rand())/RAND_MAX * 2 * 3.14f - 3.14f,float(rand())/RAND_MAX * 2 * 3.14f - 3.14f,float(rand())/RAND_MAX * 2 * 3.14f - 3.14f)),
					vec3(0.05f + float(rand())/RAND_MAX * 5.0f,0.05f + float(rand())/RAND_MAX * 5.0f,0.05f + float(rand())/RAND_MAX * 5.0f)), //, float(rand())/RAND_MAX * 2.0f + 0.5f),

					shaders[currentShaderIndex+1], shaders[0]
				)
					);
			}
			else
			{
				rigidBodies.push_back(
					new RigidBodyModel(new Ellipsoid(vec3(float(rand())/RAND_MAX * 40.0f - 20.0f, gravity < 0 ? float(rand())/RAND_MAX * 40.0f + 20.0f : -float(rand())/RAND_MAX * 40.0f - 20.0f,float(rand())/RAND_MAX * 20.0f - 20.0f), 
					quat(vec3(float(rand())/RAND_MAX * 2 * 3.14f - 3.14f,float(rand())/RAND_MAX * 2 * 3.14f - 3.14f,float(rand())/RAND_MAX * 2 * 3.14f - 3.14f)),
					vec3(float(rand())/RAND_MAX * 5.0f + 0.05f)), //,float(rand())/RAND_MAX * 5.0f + 0.05f,float(rand())/RAND_MAX * 5.0f + 0.05f)), //, float(rand())/RAND_MAX * 2.0f + 0.5f),

					shaders[currentShaderIndex], shaders[0]
				)
					);
			}
			rigidBodies.back()->GetBody()->SetLinearMomentum(vec3(float(rand())/RAND_MAX * 20.0f - 10.0f, float(rand())/RAND_MAX * 10.0f - 5.0f,float(rand())/RAND_MAX * 10.0f - 5.0f));

		}

		bool * colliding = new bool[rigidBodies.size()];
		for(unsigned int i=0; i<rigidBodies.size(); ++i) colliding[i] = false;

		for(unsigned int i=0; i<rigidBodies.size(); ++i)
		{
			//rigidBodies[i]->GetBody()->ApplyGravity(gravity);
			rigidBodies[i]->GetBody()->SetTorque(vec3(float(rand())/RAND_MAX * 2.0f - 1.0f, float(rand())/RAND_MAX * 2.0f - 1.0f, float(rand())/RAND_MAX * 2.0f - 1.0f));

			vec3 p = rigidBodies[i]->GetBody()->GetPosition();

			//if((gravity < 0  && p.y < -25) || (gravity > 0  && p.y > 25)) 
			//{
			//	delete rigidBodies[i];
			//	rigidBodies.erase(rigidBodies.begin() + i);
			//}

			if(p.y < -10) 
			{
				rigidBodies[i]->GetBody()->SetLinearMomentum(
					vec3(rigidBodies[i]->GetBody()->GetLinearMomentum().x,-rigidBodies[i]->GetBody()->GetLinearMomentum().y * 0.75f,rigidBodies[i]->GetBody()->GetLinearMomentum().z));
				rigidBodies[i]->GetBody()->SetForce(vec3(0, 10, 0));
			}

			for(unsigned int j=i+1; j<rigidBodies.size(); ++j)
			{
				if(rigidBodies[i]->GetBody()->CheckCollision(rigidBodies[j]->GetBody()))
				{
					colliding[i] = true;
					colliding[j] = true;
				}
			}
		}

		for(unsigned int i=0; i<rigidBodies.size(); ++i)
			rigidBodies[i]->ChangeColor(colliding[i] ? vec4(1,0,0,1) : vec4(0,1,0,1));//rigidBodies[i]->SetGizmos(colliding[i]);
		delete [] colliding;

		EntityManager::GetInstance()->UpdateEntities(DELTA_TIME);

	}

	// Re-trigger update callback
	glutTimerFunc(unsigned int(DELTA_TIME * 1000.0f), update, (frame+1) % 1000);
}

void init()
{
	// Initialize audio manager
	audioManager->Initialize();
	audioManager->LoadResources();

	// Set up the shaders
	GenericShader * lineShader = new GenericShader("Default.vert", "Default.frag");
	shaders.push_back(lineShader);

	GenericShader * constantShader = new GenericShader("Default.vert", "Constant.frag", "Fixed Color");
	shaders.push_back(constantShader);
	GenericShader * constantTexturedShader = new GenericShader("Textured.vert", "Constant.frag");
	shaders.push_back(constantTexturedShader);	

	GenericShader * diffuseShader = new GenericShader("Default.vert", "Diffuse.frag", "Lambertian Diffuse");
	shaders.push_back(diffuseShader);
	GenericShader * diffuseTexturedShader = new GenericShader("Textured.vert", "DiffuseTextured.frag");
	shaders.push_back(diffuseTexturedShader);

	GenericShader * toonShader = new GenericShader("Default.vert", "Toon.frag", "Toon");
	shaders.push_back(toonShader);
	GenericShader * toonTexturedShader = new GenericShader("Textured.vert", "ToonTextured.frag");
	shaders.push_back(toonTexturedShader);

	GenericShader * phongShader = new GenericShader("Default.vert", "BlinnPhong.frag", "Blinn Phong + Lambertian");
	shaders.push_back(phongShader);
	GenericShader * phongTexturedShader = new GenericShader("Textured.vert", "BlinnPhongTextured.frag");
	shaders.push_back(phongTexturedShader);

	GenericShader * orenNayarShader = new GenericShader("Default.vert", "OrenNayar.frag", "Blinn Phong + Oren Nayar");
	shaders.push_back(orenNayarShader);
	GenericShader * orenNayarTexturedShader = new GenericShader("Textured.vert", "OrenNayarTextured.frag");
	shaders.push_back(orenNayarTexturedShader);

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
		directionalLightIntensity = 0.8f;
		specularIntensity = 0.5f;

		shaders[i]->SetAmbientLight(vec3(1,1,1), ambientLightIntensity);

		directionalLightDirection = vec3(0, -1, 0);
		shaders[i]->SetDirectionalLight(directionalLightDirection, vec3(1,1,1), directionalLightIntensity);

		shininess = 64.0f;
		shaders[i]->SetSpecularComponent(vec3(1, 1, 1), specularIntensity, shininess);

		roughness = 1.0f;
		shaders[i]->SetRoughness(roughness);
	}

	currentShaderIndex = 1;

	// Create the camera
	camera = new Camera(shaders, vec3(0,0,10), vec3(0,0,0), vec3(0,1,0));
	freeMode = true;
	pause = false;

	rigidBodies.push_back(new RigidBodyModel(new Ball(vec3(-5,0,0)), shaders[currentShaderIndex+1], shaders[0]));
	rigidBodies.push_back(new RigidBodyModel(new Cat(vec3(-2,0,0), quat(), vec3(0.5f, 1.0f, 2.0f)), shaders[currentShaderIndex+1], shaders[0]));
	rigidBodies.push_back(new RigidBodyModel(new Ellipsoid(vec3(2,0,0), quat(), vec3(0.7f, 0.4f, 0.5f)), shaders[currentShaderIndex], shaders[0]));
	rigidBodies.push_back(new RigidBodyModel(new Box(vec3(5,0,0), quat(), vec3(1.0f, 0.4f, 1.5f)), shaders[currentShaderIndex+1], shaders[0]));
	currentBodyIndex = 0;

	// OpenGL initial setup
	glClearColor(0.01f, 0.01f, 0.02f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_DEPTH_FUNC);
	glDepthFunc(GL_LEQUAL);

	//glEnable(GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Start music
	audioManager->StartMusic();
} 

void releaseResources()
{
	for(unsigned int i=0; i<shaders.size(); ++i)
		delete shaders[i];
	shaders.clear();

	AudioManager::Destroy();
	audioManager = NULL;

	EntityManager::Destroy();
}

int main(int argc, char** argv){
	srand((unsigned int)time(NULL));

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow("IET");

	// Tell glut where the display function is
	glutDisplayFunc(display);

	glutIdleFunc(idle);
	glutTimerFunc(1000/FrameRate, update, 0);


	// Input handling functions
	glutMouseFunc(mousePressed);
	glutMouseWheelFunc(mouseWheel);
	glutMotionFunc(mouseDragged);

	glutKeyboardFunc(keyPressed);
	glutSpecialFunc(specialKeyPressed);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();

	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	// Entity handler
	EntityManager::Create();

	// Audio handlers
	AudioManager::Create();
	audioManager = AudioManager::GetInstance();

	// Set up your objects and shaders
	init();

	// Begin infinite event loop
	glutMainLoop();

	releaseResources();

	return 0;
}