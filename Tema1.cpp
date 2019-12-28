#include "Tema1.h"

#include <vector>
#include <iostream>

#include <stb/stb_image.h>
#include <Core/Engine.h>
#include <glm/gtx/vector_angle.hpp>


using namespace std;

struct MyParticle
{
	glm::vec4 position;
	glm::vec4 speed;
	glm::vec4 rotation;
	glm::vec4 initialPos;
	glm::vec4 initialSpeed;
	glm::vec4 initialRotation;
	glm::vec4 lcm;

	MyParticle() {};

	MyParticle(const glm::vec4 &pos, const glm::vec4 &speed, const glm::vec4 &rotation, const glm::vec4 &lcm)
	{
		SetInitial(pos, speed, rotation, lcm);
	}

	void SetInitial(const glm::vec4 &pos, const glm::vec4 &speed, const glm::vec4 &rotation, const glm::vec4 &lcm)
	{
		position = pos;
		initialPos = pos;

		this->speed = speed;
		initialSpeed = speed;

		this->rotation = rotation;
		initialRotation = rotation;

		this->lcm = lcm;
	}
};

ParticleEffect<MyParticle> *pes;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/World.cpp

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}

std::vector<std::vector<glm::vec4>> calculeazaNormale(int totalLinii, int totalColoane) {
	std::vector<glm::vec4> linie(totalColoane, glm::vec4(0));
	std::vector<std::vector<glm::vec4>> pozitii(totalLinii, linie);
	// calcul distributie gausiana
	for (int i = 0; i < totalLinii; i++) {
		for (int j = 0; j < totalColoane; j++) {
			int ii = i - totalLinii / 2;
			int jj = j - totalColoane / 2;
			const float shrink_spread = 100;
			const float extend_shards = 10;
			const float shrink_rand = 2500;
			float rx = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) -0.5) / shrink_rand;
			float x = (float(ii) * 2.0f / float(totalLinii) + rx) / shrink_spread;
			float ry = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) -0.5)/ shrink_rand;
			float y = (float(jj) * 2.0f / float(totalColoane) + ry) / shrink_spread;

			float sz = 6;
			float rz = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) -0.5) / shrink_rand;
			float z = (exp(-(ii * ii + jj * jj) / 2.0f / sz /sz) / 2.0f / M_PI / sz / sz + rz) * extend_shards;
			pozitii[i][j] = glm::vec4(x, y, z, 1);
		}
	}
	return pozitii;
}

void Tema1::Init()
{
	auto camera = GetSceneCamera();
	camera->SetPositionAndRotation(glm::vec3(6.7477, 7.8413, 7.00711), glm::quat(glm::vec3(0 * TO_RADIANS, 0 * TO_RADIANS, 0)));
	camera->Update();

	// 
	std::string shaderPath = "Source/Laboratoare/Tema1/Shaders/";
	std::string textureCubeMapPath = shaderPath + "CubeMap/";
	std::string objectPath = "Source/Laboratoare/Tema1/Models/";

	{
		Mesh* mesh = new Mesh("cube");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("glassBreakObject");
		mesh->LoadMesh(objectPath, "boat.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Load textures
	{
		TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES, "particle2.png");
	}

	LoadShader("Simple", false);
	LoadShader("Normal", false);
	LoadShader("Particle");

	nrParticles = elemente_pe_coloana * elemente_pe_coloana / 2;

	pes = new ParticleEffect<MyParticle>();
	pes->Generate(nrParticles, true);

	auto particleSSBO = pes->GetParticleBuffer();
	MyParticle* data = const_cast<MyParticle*>(particleSSBO->GetBuffer());

	int cubeSize = 20;
	int hSize = cubeSize / 2;

	int linie, coloana;
	float a = latura / 3;
	std::vector<std::vector<glm::vec4>> tabel_normale = calculeazaNormale(nrParticles / elemente_pe_coloana, elemente_pe_coloana);

	for (unsigned int i = 0; i < nrParticles; i++)
	{
		linie = i / elemente_pe_coloana;
		coloana = i % elemente_pe_coloana;
		// Seteaza Pozitia Initiala a Particulelor
		glm::vec4 pos(1);
		pos.x = coloana / 2 * latura + a + a * (coloana % 2);
		pos.y = linie * latura + latura - a + a * (coloana % 2);
		pos.z = 0;

		// Seteaza Viteza Initiala a Particulelor
		glm::vec4 speed(0);
		speed.w = 1;

		glm::vec4 rotation(AI_DEG_TO_RAD(0));
		if (i % 2 == 0)
			rotation.z = AI_DEG_TO_RAD(225);
		else
			rotation.z = AI_DEG_TO_RAD(45);
		rotation.w = 1;

		data[i].SetInitial(pos, speed, rotation, tabel_normale[linie][coloana]);
	}
	pozl = pozh = elemente_pe_coloana / 2 * latura;
	// Transmite Datele la Shader Storage Buffet Object
	particleSSBO->SetBufferData(data);

	cubeMapTextureID = UploadCubeMapTexture(
		textureCubeMapPath + "posx.png",
		textureCubeMapPath + "posy.png",
		textureCubeMapPath + "posz.png",
		textureCubeMapPath + "negx.png",
		textureCubeMapPath + "negy.png",
		textureCubeMapPath + "negz.png"
	);
	LoadShader("CubeMap", false);
	glEnable(GL_DEPTH_TEST);

	auto resolution = window->GetResolution();
	std::cout << resolution.x << " " << resolution.y;
	frameBuffer = new FrameBuffer();
	frameBuffer->Bind();
	frameBuffer->Generate(resolution.x, resolution.y, 1);
}

unsigned int Tema1::UploadCubeMapTexture(const std::string &posx, const std::string &posy, const std::string &posz, const std::string& negx, const std::string& negy, const std::string& negz)
{
	int width, height, chn;
	
	unsigned char* data_posx = stbi_load(posx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posy = stbi_load(posy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posz = stbi_load(posz.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negx = stbi_load(negx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negy = stbi_load(negy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negz = stbi_load(negz.c_str(), &width, &height, &chn, 0);

	// TODO - create OpenGL texture
	unsigned int textureID = 0;
	glGenTextures(1, &textureID);

	// TODO - bind the texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	float maxAnisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// TODO - load texture information for each face
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posz);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negz);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// free memory
	SAFE_FREE(data_posx);
	SAFE_FREE(data_posy);
	SAFE_FREE(data_posz);
	SAFE_FREE(data_negx);
	SAFE_FREE(data_negy);
	SAFE_FREE(data_negz);

	return textureID;
}


void Tema1::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

float mod(glm::vec3 vect) {
	return sqrt(pow(vect.x, 2) + pow(vect.y, 2) + pow(vect.z, 2));
}

void Tema1::Update(float deltaTimeSeconds)
{
	if (reset == 0) {
		float sparge = 2.7;
		time_colision += deltaTimeSeconds;
		if (time_colision * speed_modifier > sparge)
			colision = 1;

		float boat_speed = 4;
		modelBoat = glm::translate(modelBoat, glm::vec3(0, 0, speed_modifier * boat_speed * deltaTimeSeconds));
	}
	else
	{
		colision = 0;
		time_colision = 0;
		modelBoat = initialModelBoat;

	}
	ClearScreen();
	
	// Save camera position and rotation
	auto camera = GetSceneCamera();
	//std::cout << camera->transform->GetWorldPosition() << " " << camera->transform->GetWorldRotation() << std::endl;

	glLineWidth(3);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (show_wireframe == true)
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	else
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);

	glm::vec3 camPosition = camera->transform->GetWorldPosition();
	glm::quat camRotation = camera->transform->GetWorldRotation();
	//TODO - Render scene view from the mirror point of view
	// Use camera->SetPosition() and camera->SetRotation(glm::quat(euler_angles)) 
	{
		camera->SetPosition(mirrorPos);
		camera->SetRotation(glm::quat(mirrorRotation));
		camera->SetPerspective(90, 1, 0.1, 200);
		frameBuffer->Bind();
		DrawScene();
	}

	// Render the scene normaly
	{
		FrameBuffer::BindDefault();
		camera->SetPosition(camPosition);
		camera->SetRotation(camRotation);
		camera->SetPerspective(60, (float)window->GetResolution().x / window->GetResolution().y, 0.1, 200);
		DrawScene();
	}

	{
		auto shader = shaders["Particle"];
		if (shader->GetProgramID())
		{
			shader->Use();
			GLint loc_grosime= glGetUniformLocation(shader->program, "grosime");
			glUniform1f(loc_grosime, grosime);
			GLint loc_latura= glGetUniformLocation(shader->program, "latura");
			glUniform1f(loc_latura, latura);
			GLint loc_colision = glGetUniformLocation(shader->program, "colision");
			glUniform1i(loc_colision, colision);
			GLint loc_togle_normal = glGetUniformLocation(shader->program, "togle_normal");
			glUniform1i(loc_togle_normal, togle_normal);

			GLint loc_pozl= glGetUniformLocation(shader->program, "pozl");
			glUniform1f(loc_pozl, pozl);
			GLint loc_pozh= glGetUniformLocation(shader->program, "pozh");
			glUniform1f(loc_pozh, pozh);
			GLint loc_reset= glGetUniformLocation(shader->program, "reset");
			glUniform1f(loc_reset, reset);
			GLint loc_speed_modifier= glGetUniformLocation(shader->program, "speed_modifier");
			glUniform1f(loc_speed_modifier, speed_modifier);

			auto cameraPosition = camera->transform->GetWorldPosition();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
			int loc_texture = shader->GetUniformLocation("texture_cubemap");
			glUniform1i(loc_texture, 0);

			int loc_camera = shader->GetUniformLocation("camera_position");
			glUniform3fv(loc_camera, 1, glm::value_ptr(cameraPosition));

			frameBuffer->BindTexture(0, GL_TEXTURE0);

			pes->Render(GetSceneCamera(), shader);
		}
	}
	reset = 0;
}

void Tema1::DrawScene()
{
	auto camera = GetSceneCamera();
	// draw the cubemap
	{
		Shader *shader = shaders["Simple"];
		shader->Use();

		modelMatrixCube = glm::scale(glm::mat4(1), glm::vec3(60));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrixCube));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
		int loc_texture = shader->GetUniformLocation("texture_cubemap");
		glUniform1i(loc_texture, 0);

		meshes["cube"]->Render();
	}
	{
		RenderMesh(meshes["glassBreakObject"], shaders["Normal"], modelBoat);
	}

}

void Tema1::FrameEnd()
{
	//DrawCoordinatSystem();
}

void Tema1::LoadShader(std::string name, bool hasGeomtery)
{
	static std::string shaderPath = "Source/Laboratoare/Tema1/Shaders/";

	// Create a shader program for particle system
	{
		Shader *shader = new Shader(name.c_str());
		shader->AddShader((shaderPath + name + ".VS.glsl").c_str(), GL_VERTEX_SHADER);
		shader->AddShader((shaderPath + name + ".FS.glsl").c_str(), GL_FRAGMENT_SHADER);
		if (hasGeomtery)
		{
			shader->AddShader((shaderPath + name + ".GS.glsl").c_str(), GL_GEOMETRY_SHADER);
		}

		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/Window/InputController.h

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	// treat continuous update based on input
	float modif = 1 * deltaTime;
	static float modx = 5.07917;
	static float mody = 4.77483;
	static float modz = 5.3424;
	  
	if (window->KeyHold(GLFW_KEY_T))
		modz += modif;
	if (window->KeyHold(GLFW_KEY_G))
		modz -= modif;
	if (window->KeyHold(GLFW_KEY_H))
		modx += modif;
	if (window->KeyHold(GLFW_KEY_F))
		modx -= modif;
	if (window->KeyHold(GLFW_KEY_Y))
		mody += modif;
	if (window->KeyHold(GLFW_KEY_R))
		mody -= modif;
	mirrorPos = glm::vec3(modx, mody, modz);
	if ((window->KeyHold(GLFW_KEY_T)) || (window->KeyHold(GLFW_KEY_G)) || (window->KeyHold(GLFW_KEY_F)) || (window->KeyHold(GLFW_KEY_H)) || (window->KeyHold(GLFW_KEY_R)) || (window->KeyHold(GLFW_KEY_Y))) {
		std::cout << mirrorPos << std::endl;
	}
};

void Tema1::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_0) {
		reset = 1;
	}
	if (key == GLFW_KEY_9) {
		show_wireframe = !show_wireframe;
	}
	if (key == GLFW_KEY_8) {
		togle_normal = (togle_normal + 1)%2;
	}
	const float incspeed = 0.1;
	if (key == GLFW_KEY_7) {
		speed_modifier += incspeed;
	}
	if (key == GLFW_KEY_6) {
		speed_modifier -= incspeed;
		if (speed_modifier < incspeed)
			speed_modifier = incspeed;
	}
	if (key == GLFW_KEY_6 || key == GLFW_KEY_7)
		std::cout << "Current speed" << speed_modifier << "\n";
	
};

void Tema1::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void Tema1::OnWindowResize(int width, int height)
{
	// treat window resize event
}
