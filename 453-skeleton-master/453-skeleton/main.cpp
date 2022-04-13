#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// An example struct for Game Objects.
// You are encouraged to customize this as you see fit.
struct GameObject {
	// Struct's constructor deals with the texture.
	// Also sets default position, theta, scale, and transformationMatrix
	GameObject(std::string texturePath, GLenum textureInterpolation) :
		texture(texturePath, textureInterpolation),
		position(0.0f, 0.0f, 1.0f, 1.0f),
		theta(glm::radians(90.f)),
		scale(1),
		transformationMatrix(1.0f) // This constructor sets it as the identity matrix
	{}

	CPU_Geometry cgeom;
	GPU_Geometry ggeom;
	Texture texture;

	glm::vec4 position;
	float theta; // Object's rotation
	// Alternatively, you could represent rotation via a normalized heading vec:
	// glm::vec3 heading;
	glm::vec3 scale; // Or, alternatively, a glm::vec2 scale;
	glm::mat4 transformationMatrix;
};

// EXAMPLE CALLBACKS
class MyCallbacks : public CallbackInterface {

public:
	MyCallbacks(ShaderProgram& shader, int screenWidth, int screenHeight) :
		screenDim(screenWidth, screenHeight),
		shader(shader),
		leftMousePressed(false),
		upPressed(false),
		downPressed(false),
		rPressed(false)
	{}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
			shader.recompile();
			upPressed = true;
		}
		if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
			shader.recompile();
			upPressed = false;
		}
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
			shader.recompile();
			downPressed = true;
		}
		if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
			shader.recompile();
			downPressed = false;
		}
		if (key == GLFW_KEY_R && action == GLFW_PRESS) {
			shader.recompile();
			rPressed = true;
		}
		if (key == GLFW_KEY_R && action == GLFW_RELEASE) {
			shader.recompile();
			rPressed = false;
		}
	}

	virtual void mouseButtonCallback(int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			shader.recompile();
			leftMousePressed = true;
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			shader.recompile();
			leftMousePressed = false;
		}
	}

	void refreshStatuses() {
		//leftMousePressed = false;
		//upPressed = false;
		//downPressed = false;
		rPressed = false;
	}

	bool mouseButtonDown(int button) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			return leftMousePressed;
		}
		if (button == GLFW_KEY_UP) {
			return upPressed;
		}
		if (button == GLFW_KEY_DOWN) {
			return downPressed;
		}
		if (button == GLFW_KEY_R) {
			return rPressed;
		}
		return false;
	}

	glm::vec2 mouseGL() {
		glm::vec2 startingVec(xScreenPos, yScreenPos);
		glm::vec2 shiftedVec = startingVec + glm::vec2(0.5f);
		glm::vec2 scaled = shiftedVec / glm::vec2(screenDim);
		glm::vec2 flippedY = glm::vec2(scaled.x, 1.0f - scaled.y);
		glm::vec2 final = flippedY * 2.0f - glm::vec2(1.0f);
		return final;
	}

	virtual void cursorPosCallback(double xpos, double ypos) {
		xScreenPos = xpos;
		yScreenPos = ypos;
	}

private:
	bool leftMousePressed;
	bool upPressed;
	bool downPressed;
	bool rPressed;

	glm::ivec2 screenDim;

	double xScreenPos;
	double yScreenPos;

	ShaderProgram& shader;
};

CPU_Geometry initGeom(float width, float height) {
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	CPU_Geometry retGeom;
	// vertices for the spaceship quad
	//retGeom.verts.push_back(glm::vec3(-halfWidth, halfHeight, 0.f));	//TL
	//retGeom.verts.push_back(glm::vec3(-halfWidth, -halfHeight, 0.f));	//BL
	//retGeom.verts.push_back(glm::vec3(halfWidth, -halfHeight, 0.f));	//BR
	//retGeom.verts.push_back(glm::vec3(-halfWidth, halfHeight, 0.f));	//TL
	//retGeom.verts.push_back(glm::vec3(halfWidth, -halfHeight, 0.f));	//BR
	//retGeom.verts.push_back(glm::vec3(halfWidth, halfHeight, 0.f));		//TR

	// For full marks (Part IV), you'll need to use the following vertex coordinates instead.
	// Then, you'd get the correct scale/translation/rotation by passing in uniforms into
	// the vertex shader.
	
	retGeom.verts.push_back(glm::vec3(-1.f, 1.f, 0.f));
	retGeom.verts.push_back(glm::vec3(-1.f, -1.f, 0.f));
	retGeom.verts.push_back(glm::vec3(1.f, -1.f, 0.f));
	retGeom.verts.push_back(glm::vec3(-1.f, 1.f, 0.f));
	retGeom.verts.push_back(glm::vec3(1.f, -1.f, 0.f));
	retGeom.verts.push_back(glm::vec3(1.f, 1.f, 0.f));
	

	// texture coordinates
	retGeom.texCoords.push_back(glm::vec2(0.f, 1.f));
	retGeom.texCoords.push_back(glm::vec2(0.f, 0.f));
	retGeom.texCoords.push_back(glm::vec2(1.f, 0.f));
	retGeom.texCoords.push_back(glm::vec2(0.f, 1.f));
	retGeom.texCoords.push_back(glm::vec2(1.f, 0.f));
	retGeom.texCoords.push_back(glm::vec2(1.f, 1.f));
	return retGeom;
}



// END EXAMPLES

int main() {
	
	Log::debug("Starting main");

	// WINDOW
	glfwInit();
	Window window(1800, 1800, "CPSC 453"); // can set callbacks at construction if desired


	GLDebug::enable();

	// SHADERS
	ShaderProgram shader("shaders/test.vert", "shaders/test.frag");

	std::shared_ptr<MyCallbacks> inputManager = std::make_shared<MyCallbacks>(shader, 1800, 1800);

	// CALLBACKS  
	window.setCallbacks(inputManager); // can also update callbacks to new ones


	

	// GL_NEAREST looks a bit better for low-res pixel art than GL_LINEAR.
	// But for most other cases, you'd want GL_LINEAR interpolation.
	GameObject ship("textures/ship.png", GL_NEAREST);
	GameObject diamond("textures/diamond.png", GL_NEAREST);

	ship.cgeom = initGeom(0.18f, 0.12f);
	diamond.cgeom = initGeom(0.14f, 0.14f);


	ship.ggeom.setVerts(ship.cgeom.verts);
	ship.ggeom.setTexCoords(ship.cgeom.texCoords);

	diamond.ggeom.setVerts(diamond.cgeom.verts);
	diamond.ggeom.setTexCoords(diamond.cgeom.texCoords);

	ship.scale = glm::vec3(0.18f/2, 0.12f/2, 1.f);
	diamond.scale = glm::vec3(0.14f/2, 0.14f/2, 1.f);

	// create transformations
	glm::mat4 shipTransform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	//shipTransform = glm::translate(shipTransform, glm::vec3(0.5f, -0.5f, 0.0f));

	ship.transformationMatrix = glm::scale(ship.transformationMatrix, ship.scale);
	//ship.transformationMatrix = glm::scale(ship.transformationMatrix, ship.scale);
	glm::vec3 diamondPos = glm::vec3(0.7f, 0.7f, 0.0f);
	glm::vec3 diamondPos2 = glm::vec3(-.3f, 0.8f, 0.0f);
	glm::vec3 diamondPos3 = glm::vec3(-0.7f, -0.2f, 0.0f);
	glm::vec3 diamondPos4 = glm::vec3(0.2f, -0.8f, 0.0f);

	diamond.transformationMatrix = glm::translate(diamond.transformationMatrix, diamondPos);
	glm::mat4 diamondTranform2 = glm::translate(glm::mat4(1.0f), diamondPos2);
	glm::mat4 diamondTranform3 = glm::translate(glm::mat4(1.0f), diamondPos3);
	glm::mat4 diamondTranform4 = glm::translate(glm::mat4(1.0f), diamondPos4);

	diamond.transformationMatrix = glm::scale(diamond.transformationMatrix, diamond.scale);
	diamondTranform2 = glm::scale(diamondTranform2, diamond.scale);
	diamondTranform3 = glm::scale(diamondTranform3, diamond.scale);
	diamondTranform4 = glm::scale(diamondTranform4, diamond.scale);

	

	float omega = 0.001;
	float speed = 0.001;
	glm::vec3 velocity(0.f);
	bool absorbed = false;
	bool absorbed2 = false;
	bool absorbed3 = false;
	bool absorbed4 = false;
	bool upsized = false;
	bool upsized2 = false;
	bool upsized3 = false;
	bool upsized4 = false;

	int score = 0;

	// RENDER LOOP
	while (!window.shouldClose()) {

		//inputManager->refreshStatuses();
		glfwPollEvents();

		if (inputManager->mouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
			glm::vec2 mousePos = inputManager->mouseGL();
			float cursorAngle = atan2f(mousePos[1] - ship.position[1], mousePos[0] - ship.position[0]);// -ship.theta;
			if (cursorAngle < 0) {
				cursorAngle = cursorAngle + glm::radians(360.f);
			}
			if (ship.theta < 0) {
				ship.theta = ship.theta + glm::radians(360.f);
			}
			if (cursorAngle - ship.theta < glm::radians(180.f) && cursorAngle - ship.theta > 0) {
			float temp = ship.theta;
			//ship.theta = atan2f(mousePos[1] - ship.position[1], mousePos[0] - ship.position[0]);
			ship.transformationMatrix = glm::rotate(ship.transformationMatrix, omega, glm::vec3(0.0f, 0.0f, 1.0f));
			ship.theta = ship.theta + omega;
				
				//std::cout << glm::degrees(ship.theta) << ", " << glm::degrees(cursorAngle) << std::endl;
			}
			else if (cursorAngle - ship.theta > glm::radians(180.f) || cursorAngle - ship.theta < 0) {
				ship.transformationMatrix = glm::rotate(ship.transformationMatrix, -omega, glm::vec3(0.0f, 0.0f, 1.0f));
				ship.theta = ship.theta - omega;
				//std::cout << glm::degrees(ship.theta) << ", " << glm::degrees(cursorAngle) << std::endl;
			}
		}

		if ((inputManager->mouseButtonDown(GLFW_KEY_UP) || inputManager->mouseButtonDown(GLFW_KEY_DOWN))){// && (abs(ship.position[0]) <= 1 - speed && abs(ship.position[1]) <= 1 - speed)) {

			if (inputManager->mouseButtonDown(GLFW_KEY_UP)) {
				//velocity = glm::vec3(speed * cos(ship.theta), speed * sin(ship.theta), 0.0f);
				
				ship.transformationMatrix = glm::translate(ship.transformationMatrix, glm::vec3(0.0f, speed, 0.0f));

			}
			else if(inputManager->mouseButtonDown(GLFW_KEY_DOWN)) {
				//velocity = glm::vec3(-speed * cos(ship.theta), -speed * sin(ship.theta), 0.0f);
				ship.transformationMatrix = glm::translate(ship.transformationMatrix, glm::vec3(0.0f, -speed, 0.0f));
			}
			//glm::mat3 posTrans = glm::mat3(1.f, 0.f, speed * cos(ship.theta), 0.f, 1.f, speed * sin(ship.theta), 0.f, 0.f, 1.f);
			
			//ship.transformationMatrix = glm::rotate(ship.transformationMatrix, -ship.theta, glm::vec3(0.0f, 0.0f, 1.0f));
			
			ship.position = ship.transformationMatrix * ship.position;

			float stdAngle = fmod(glm::degrees(ship.theta), 360);
			if (stdAngle < 0) {
				stdAngle = stdAngle + 360;
			}

			if (ship.position[1] >= 1 - speed && stdAngle < 180) {
				ship.transformationMatrix = glm::rotate(ship.transformationMatrix, -2 * ship.theta, glm::vec3(0.0f, 0.0f, 1.0f));
				ship.theta = -ship.theta;
			}
			else if (ship.position[1] <= -1 + speed && stdAngle > 180) {
				ship.transformationMatrix = glm::rotate(ship.transformationMatrix, -2 * ship.theta, glm::vec3(0.0f, 0.0f, 1.0f));
				ship.theta = -ship.theta;
			}
			else if (ship.position[0] >= 1 - speed && (stdAngle > 270 || stdAngle < 90)) {
				ship.transformationMatrix = glm::rotate(ship.transformationMatrix, ship.theta, glm::vec3(0.0f, 0.0f, 1.0f));
				ship.theta = 2 * ship.theta;
			}
			else if (ship.position[0] <= -1 + speed && (stdAngle > 90 && stdAngle < 270)) {
				ship.transformationMatrix = glm::rotate(ship.transformationMatrix, glm::radians(180.f) - 2 * ship.theta, glm::vec3(0.0f, 0.0f, 1.0f));
				ship.theta = glm::radians(180.f) - ship.theta;
			}
			//ship.transformationMatrix = glm::rotate(ship.transformationMatrix, ship.theta, glm::vec3(0.0f, 0.0f, 1.0f));
		//	std::cout << ship.transformationMatrix << std::endl;
			//std::cout << velocity << std::endl;
			//std::cout << posTrans << std::endl;
			//std::cout << glm::degrees(ship.theta) << std::endl;
			//std::cout << glm::to_string(ship.position) << std::endl;
		}

		glm::mat4 diamondTransform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

		//diamondTransform = glm::translate(diamondTransform, glm::vec3(0.5f, -0.5f, 0.0f));
		//diamondTransform = glm::rotate(diamondTransform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		//diamondTransform = glm::scale(diamondTransform, diamond.scale);

		



		shader.use();
		ship.ggeom.bind();
		diamond.ggeom.bind();

		//ship.transformationMatrix = glm::scale(ship.transformationMatrix, ship.scale);
		glm::mat4 transform = ship.transformationMatrix;
		//transform = glm::scale(transform, ship.scale);
		unsigned int transformLoc = glGetUniformLocation(shader.getProgram(), "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glEnable(GL_FRAMEBUFFER_SRGB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		ship.texture.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		ship.texture.unbind();

		float distance = sqrt(pow(ship.position[0] - diamondPos[0], 2) + pow(ship.position[1] - diamondPos[1], 2));
		
		if (distance >= 0.1 && absorbed == false) {
			transform = diamond.transformationMatrix;
			transformLoc = glGetUniformLocation(shader.getProgram(), "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

			diamond.texture.bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			diamond.texture.unbind();
		}
		else if (absorbed == false) {
			absorbed = true;
			score = score + 1;
			
			ship.scale = glm::vec3(1.05f, 1.05f, 1.f);
			ship.transformationMatrix = glm::scale(ship.transformationMatrix, ship.scale);
		}
		

		//diamond.transformationMatrix = diamondTranform2;
		distance = sqrt(pow(ship.position[0] - diamondPos2[0], 2) + pow(ship.position[1] - diamondPos2[1], 2));
		if (distance >= 0.1 && absorbed2 == false) {
			transform = diamondTranform2;
			transformLoc = glGetUniformLocation(shader.getProgram(), "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

			diamond.texture.bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			diamond.texture.unbind();
		}
		else if (absorbed2 == false) {
			absorbed2 = true;
			score = score + 1;

			ship.scale = glm::vec3(1.05f, 1.05f, 1.f);
			ship.transformationMatrix = glm::scale(ship.transformationMatrix, ship.scale);
		}

		distance = sqrt(pow(ship.position[0] - diamondPos3[0], 2) + pow(ship.position[1] - diamondPos3[1], 2));
		if (distance >= 0.1 && absorbed3 == false) {
			transform = diamondTranform3;
			transformLoc = glGetUniformLocation(shader.getProgram(), "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

			diamond.texture.bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			diamond.texture.unbind();
		}
		else if (absorbed3 == false) {
			absorbed3 = true;
			score = score + 1;

			ship.scale = glm::vec3(1.05f, 1.05f, 1.f);
			ship.transformationMatrix = glm::scale(ship.transformationMatrix, ship.scale);
		}

		distance = sqrt(pow(ship.position[0] - diamondPos4[0], 2) + pow(ship.position[1] - diamondPos4[1], 2));
		if (distance >= 0.1 && absorbed4 == false) {
			transform = diamondTranform4;
			transformLoc = glGetUniformLocation(shader.getProgram(), "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

			diamond.texture.bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			diamond.texture.unbind();
		}
		else if (absorbed4 == false) {
			absorbed4 = true;
			score = score + 1;

			ship.scale = glm::vec3(1.05f, 1.05f, 1.f);
			ship.transformationMatrix = glm::scale(ship.transformationMatrix, ship.scale);
		}

		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui

		// Starting the new ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// Putting the text-containing window in the top-left of the screen.
		ImGui::SetNextWindowPos(ImVec2(5, 5));

		// Setting flags
		ImGuiWindowFlags textWindowFlags =
			ImGuiWindowFlags_NoMove |				// text "window" should not move
			ImGuiWindowFlags_NoResize |				// should not resize
			ImGuiWindowFlags_NoCollapse |			// should not collapse
			ImGuiWindowFlags_NoSavedSettings |		// don't want saved settings mucking things up
			ImGuiWindowFlags_AlwaysAutoResize |		// window should auto-resize to fit the text
			ImGuiWindowFlags_NoBackground |			// window should be transparent; only the text should be visible
			ImGuiWindowFlags_NoDecoration |			// no decoration; only the text should be visible
			ImGuiWindowFlags_NoTitleBar;			// no title; only the text should be visible

		// Begin a new window with these flags. (bool *)0 is the "default" value for its argument.
		ImGui::Begin("scoreText", (bool *)0, textWindowFlags);

		
		if (score == 4) {
			ImGui::SetWindowFontScale(30.0f);
			ImGui::Text("YOU WON");
		}
		else {
			// Scale up text a little, and set its value
			ImGui::SetWindowFontScale(1.5f);
			ImGui::Text("Score: %d", score); // Second parameter gets passed into "%d"
		}
		// End the window.
		ImGui::End();

		ImGui::Render();	// Render the ImGui window
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Some middleware thing

		window.swapBuffers();
	}
	// ImGui cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}
