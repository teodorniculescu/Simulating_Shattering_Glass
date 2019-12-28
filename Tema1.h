#pragma once

#include <Component/SimpleScene.h>

class Tema1 : public SimpleScene
{
	public:
		Tema1();
		~Tema1();

		void Init() override;


	private:
		unsigned int UploadCubeMapTexture(const std::string & posx, const std::string & posy, const std::string & posz, const std::string & negx, const std::string & negy, const std::string & negz);
		void FrameStart() override;
		void calculateMirrorAngle();
		void Update(float deltaTimeSeconds) override;
		void DrawScene();
		void FrameEnd() override;

		void LoadShader(std::string name, bool hasGeomtery = true);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

	
	protected:
		int cubeMapTextureID;
		glm::mat4 modelMatrix;
		float translateX, translateY, translateZ;
		float scaleX, scaleY, scaleZ;
		float angularStepOX, angularStepOY, angularStepOZ;
		GLenum polygonMode;
		int colision = 0;
		float grosime = 0.01;
		float latura = 0.25;
		int elemente_pe_coloana = 80;
		glm::mat4 modelMatrixCube;
		bool show_wireframe = false;
		int togle_normal = 1;
		FrameBuffer *frameBuffer;
		glm::vec3 mirrorPos = glm::vec3(0);
		glm::vec3 mirrorRotation = glm::vec3(0, 0, 0);
		unsigned int nrParticles;
		float pozl, pozh;
		float boat_scale = 1.0 / 2.0;
		glm::mat4 modelBoat = glm::translate(glm::scale(glm::mat4(1), glm::vec3(boat_scale)), glm::vec3(12, 8, -20));
		glm::mat4 initialModelBoat = modelBoat;
		float time_colision = 0;
		float reset = 0;
		float speed_modifier = 1.0f;
};
