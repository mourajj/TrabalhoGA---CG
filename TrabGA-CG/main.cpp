#include <Windows.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <vector>

#include "Includes.h"
#include "Mesh.h"
#include "Material.h"
#include "Obj3D.h"
#include "Shader.h"

using namespace std;
void processarPedra(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
void processarQueda(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale, int idTar);
void processarCamera(GLFWwindow* window);
GLuint testedeColisao(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
void callbackDoMouse(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void processInput(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);

vector<Obj3D*> listaDeObjs;
float posicaoCaixaX[7];
float posicaoCaixaY[7];
float posicaoCaixaZ[7];
bool pedraJogada = false;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float velocidadeDaCamera = 0.03f;
float direcaoVelocidade = 0.06f;

bool firtMouse = true;

float anguloInclinacao = -30.f;
float yawAngle = -90.f;
glm::vec3 direcaoCamera = glm::vec3(
	glm::normalize(glm::vec3(
		cos(glm::radians(anguloInclinacao)) * cos(glm::radians(yawAngle)),
		sin(glm::radians(anguloInclinacao)),
		cos(glm::radians(anguloInclinacao)) * sin(glm::radians(yawAngle))
	))
);

glm::vec3 posicaoCamera(0.0f, 10.0f, 20.0f);
glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
glm::vec3 cameraFrontal = glm::normalize(glm::cross(direcaoCamera, worldUp));

glm::mat4 ViewMatrix = glm::lookAt(posicaoCamera, posicaoCamera + direcaoCamera, worldUp);

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Visualizador de Modelos 3D - GA/CG", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Falha na GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//------------------------- CALLBACK MOUSE -------------------------//
	glfwSetCursorPosCallback(window, callbackDoMouse);

	glewExperimental = GL_TRUE;
	glewInit();

	int framebufferWidth = 0;
	int framebufferHeight = 0;

	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//------------------------- SHADERS -------------------------//
	GLuint shaderProgram = LoadShader("Shaders/Core/core.vert", "Shaders/Core/core.frag");
	glUseProgram(shaderProgram);

	float fov = 90.f;
	float nearPlane = 0.1f;
	float farPlane = 1000.f;
	glm::mat4 ProjectionMatrix(1.f);

	ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(framebufferWidth) / framebufferHeight, nearPlane, farPlane);

	glm::mat4 ModelMatrix(1.f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	//------------------------- CHÃO -------------------------//
	Obj3D* obj3d = new Obj3D();
	obj3d->Inicializar();
	obj3d->position = glm::vec3(0.0f, -1.0f, 0.0f);
	obj3d->scale.x = 60.0f;
	obj3d->scale.z = 60.0f;
	Mesh* mesh = obj3d->processObj("caixa.obj");
	obj3d->mesh = mesh;
	vector<Material*> materials = obj3d->getMat();
	obj3d->materials = materials;
	listaDeObjs.push_back(obj3d);

	//------------------------- MESAS -------------------------//
	Obj3D* obj3d1 = new Obj3D();
	obj3d1->Inicializar();
	obj3d1->position = glm::vec3(10.0f, 0.0f, -10.0f);
	Mesh* mesh1 = obj3d1->processObj("mesa01.obj");
	obj3d1->mesh = mesh1;
	vector<Material*> materials1 = obj3d1->getMat();
	obj3d1->materials = materials1;
	listaDeObjs.push_back(obj3d1);

	Obj3D* obj3d2 = new Obj3D();
	obj3d2->Inicializar();
	obj3d2->position = glm::vec3(0.0f, 0.0f, -10.0f);
	Mesh* mesh2 = obj3d2->processObj("mesa01.obj");
	obj3d2->mesh = mesh2;
	vector<Material*> materials2 = obj3d2->getMat();
	obj3d2->materials = materials2;
	listaDeObjs.push_back(obj3d2);

	Obj3D* obj3d3 = new Obj3D();
	obj3d3->Inicializar();
	obj3d3->position = glm::vec3(-10.0f, 0.0f, -10.0f);
	Mesh* mesh3 = obj3d3->processObj("mesa01.obj");
	obj3d3->mesh = mesh3;
	vector<Material*> materials3 = obj3d3->getMat();
	obj3d3->materials = materials3;
	listaDeObjs.push_back(obj3d3);

	//------------------------- CAIXAS -------------------------//
	Obj3D* obj3d4 = new Obj3D();
	obj3d4->Inicializar();
	obj3d4->position = glm::vec3(obj3d2->position.x, 10.0f, obj3d2->position.z);
	posicaoCaixaX[0] = obj3d4->position.x;
	posicaoCaixaY[0] = obj3d4->position.y;
	posicaoCaixaZ[0] = obj3d4->position.z;
	Mesh* mesh4 = obj3d4->processObj("caixa01.obj");
	obj3d4->mesh = mesh4;
	vector<Material*> materials4 = obj3d4->getMat();
	obj3d4->materials = materials4;
	listaDeObjs.push_back(obj3d4);

	Obj3D* obj3d5 = new Obj3D();
	obj3d5->Inicializar();
	obj3d5->position = glm::vec3(obj3d2->position.x + 2.5f, 10.0f, obj3d2->position.z);
	posicaoCaixaX[1] = obj3d5->position.x;
	posicaoCaixaY[1] = obj3d5->position.y;
	posicaoCaixaZ[1] = obj3d5->position.z;
	Mesh* mesh5 = obj3d5->processObj("caixa01.obj");
	obj3d5->mesh = mesh5;
	vector<Material*> materials5 = obj3d5->getMat();
	obj3d5->materials = materials5;
	listaDeObjs.push_back(obj3d5);

	Obj3D* obj3d6 = new Obj3D();
	obj3d6->Inicializar();
	obj3d6->position = glm::vec3(obj3d2->position.x - 2.5f, 10.0f, obj3d2->position.z);
	posicaoCaixaX[2] = obj3d6->position.x;
	posicaoCaixaY[2] = obj3d6->position.y;
	posicaoCaixaZ[2] = obj3d6->position.z;
	Mesh* mesh6 = obj3d6->processObj("caixa01.obj");
	obj3d6->mesh = mesh6;
	vector<Material*> materials6 = obj3d6->getMat();
	obj3d6->materials = materials6;
	listaDeObjs.push_back(obj3d6);
	
	Obj3D* obj3d7 = new Obj3D();
	obj3d7->Inicializar();
	obj3d7->position = glm::vec3(obj3d1->position.x + -15.0f, 10.0f, obj3d1->position.z);
	posicaoCaixaX[3] = obj3d7->position.x;
	posicaoCaixaY[3] = obj3d7->position.y;
	posicaoCaixaZ[3] = obj3d7->position.z;
	Mesh* mesh7 = obj3d7->processObj("caixa01.obj");
	obj3d7->mesh = mesh7;
	vector<Material*> materials7 = obj3d7->getMat();
	obj3d7->materials = materials7;
	listaDeObjs.push_back(obj3d7);

	Obj3D* obj3d8 = new Obj3D();
	obj3d8->Inicializar();
	obj3d8->position = glm::vec3(obj3d1->position.x - 5.0f, 10.0f, obj3d1->position.z);
	posicaoCaixaX[4] = obj3d8->position.x;
	posicaoCaixaY[4] = obj3d8->position.y;
	posicaoCaixaZ[4] = obj3d8->position.z;
	Mesh* mesh8 = obj3d8->processObj("caixa01.obj");
	obj3d8->mesh = mesh8;
	vector<Material*> materials8 = obj3d8->getMat();
	obj3d8->materials = materials8;
	listaDeObjs.push_back(obj3d8);


	//------------------------- PEDRA -------------------------//
	Obj3D* bullet = new Obj3D();
	bullet->Inicializar();
	bullet->position.y = -2.0f;
	Mesh* mesh_b = bullet->processObj("rock.obj");
	bullet->mesh = mesh_b;
	vector<Material*> materials_b = bullet->getMat();
	bullet->materials = materials_b;
	listaDeObjs.push_back(bullet);


	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);

	std::cout << "Index:\n0 - Central Box\n1 - Right Box\n2 - Lef Box\n" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.42f, 0.62f, 0.85f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProgram);

		processInput(window);

		processarCamera(window);

		processarPedra(window, listaDeObjs[9]->position, listaDeObjs[9]->rotation, listaDeObjs[9]->scale);

		if (pedraJogada == true) {
			processInput(window, listaDeObjs[9]->position, listaDeObjs[9]->rotation, listaDeObjs[9]->scale);
		}
		
		for (int i = 0; i <= 6; i++) {
			if (testedeColisao(window, listaDeObjs[9]->position, listaDeObjs[9]->rotation, listaDeObjs[9]->scale) == i) {
				processarQueda(window, listaDeObjs[4 + i]->position, listaDeObjs[4 + i]->rotation, listaDeObjs[4 + i]->scale, i);
				std::cout << "A collision happened with box: " << i << std::endl;
			}
		}

		int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

		glm::vec3 cameraFrontalCalc;
		cameraFrontalCalc.x = cos(glm::radians(anguloInclinacao)) * cos(glm::radians(yawAngle));
		cameraFrontalCalc.y = sin(glm::radians(anguloInclinacao));
		cameraFrontalCalc.z = cos(glm::radians(anguloInclinacao)) * sin(glm::radians(yawAngle));
		direcaoCamera = glm::normalize(cameraFrontalCalc);

		ViewMatrix = glm::lookAt(posicaoCamera, posicaoCamera + direcaoCamera, worldUp);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));

		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

		ProjectionMatrix = glm::mat4(1.f);
		ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(framebufferWidth) / framebufferHeight, nearPlane, farPlane);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

		//------------------------- CARREGAR TEXTURAS -------------------------//
		for (int i = 0; i < listaDeObjs.size(); i++)
		{
			listaDeObjs[i]->transform();
			GLuint texture;
			for (Group* g : listaDeObjs[i]->mesh->groups) {
				for (Material* m : listaDeObjs[i]->materials) {
					texture = m->texture;
					glUniform3f(glGetUniformLocation(shaderProgram, "Ka"), m->ka->r, m->ka->g, m->ka->b);
					glUniform3f(glGetUniformLocation(shaderProgram, "Kd"), m->kd->r, m->kd->g, m->kd->b);
					glUniform3f(glGetUniformLocation(shaderProgram, "Ks"), m->ks->r, m->ks->g, m->ks->b);
					glUniform1f(glGetUniformLocation(shaderProgram, "Ns"), m->ns);
				}

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glUniform1i(glGetUniformLocation(shaderProgram, "texture"), 0);

				glBindVertexArray(g->vao);

				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(listaDeObjs[i]->ModelMatrix));

				glDrawArrays(GL_TRIANGLES, 0, g->faces.size() * 3);
			}
		}

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void callbackDoMouse(GLFWwindow* window, double xpos, double ypos)
{
	int pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

	if (pressed == 1) {
		cout << "mouse pressed";
	}
	else {

	}
}

void processInput(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
{
	//------------------------- MOVIMENTO DAS CAIXAS -------------------------//
	position.z -= 0.05 * (cameraFrontal.x);
	position.x += 0.05 * (cameraFrontal.z);
	rotation.z -= 0.6;
}

void processarQueda(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale, int idTar)
{
	//------------------------- COLISÃO DAS CAIXAS PARA O CHÃO -------------------------//
	position.z -= 1.5 * (cameraFrontal.x);
	position.x += 1.5 * (cameraFrontal.z);
	position.y = 1.0;
}

void processarPedra(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
{
	//------------------------- POSIÇÃO INICIAL DA CAIXA -------------------------//
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		position = glm::vec3(posicaoCamera.x + cameraFrontal.x - 1.0f, posicaoCamera.y + cameraFrontal.y -1.0f, posicaoCamera.z + cameraFrontal.z);
		pedraJogada = true;
	}
}

void processarCamera(GLFWwindow* window)
{
	//------------------------- POSIÇÃO DO FIRST PERSON SHOOTER -------------------------//
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		posicaoCamera -= glm::normalize(glm::cross(cameraFrontal, worldUp)) * velocidadeDaCamera;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		posicaoCamera -= velocidadeDaCamera * cameraFrontal;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		posicaoCamera += glm::normalize(glm::cross(cameraFrontal, worldUp)) * velocidadeDaCamera;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		posicaoCamera += velocidadeDaCamera * cameraFrontal;
	}
	//------------------------- POSIÇÃO DA CAMERA -------------------------//
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		posicaoCamera += velocidadeDaCamera * direcaoCamera;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		posicaoCamera -= velocidadeDaCamera * direcaoCamera;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		yawAngle -= direcaoVelocidade;
		cameraFrontal = cross(direcaoCamera, worldUp);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		yawAngle += direcaoVelocidade;
		cameraFrontal = cross(direcaoCamera, worldUp);
	}
}

GLuint testedeColisao(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
{

	for (int i = 0; i <= 6; i++) {
		if (position.x <= posicaoCaixaX[i] + 0.7 && position.x >= posicaoCaixaX[i] - 0.7)
		{
			if (position.y <= posicaoCaixaY[i] + 0.7 && position.y >= posicaoCaixaY[i] - 0.7)
			{
				if (position.z <= posicaoCaixaZ[i] + 0.7 && position.z >= posicaoCaixaZ[i] - 0.7)
				{
					return i;
				}
			}
		}
	}
}