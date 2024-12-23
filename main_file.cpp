/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"


float speed = 0; //Prędkość kątowa obrotu obiektu
float skret = 0; //skret kół
//Models::Torus carWheel(0.3, 0.1, 12, 12);
//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Procedura obsługi klawiatury
void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed = -PI; //Jeżeli wciśnięto klawisz "w lewo" ustaw prędkość na -PI
		if (key == GLFW_KEY_RIGHT) speed = PI; //Jeżeli wciśnięto klawisz "w prawo" ustaw prędkość na PI
		if (key == GLFW_KEY_A) {
			skret = PI / 6;
		}
		if (key == GLFW_KEY_D) {
			skret = -PI / 6;
		}
	}

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) speed = 0;
		if (key == GLFW_KEY_A || key == GLFW_KEY_D) skret = 0;
	}
}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	initShaders();
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0, 0, 0, 1);//Ustaw czarny kolor czyszczenia ekranu
	glEnable(GL_DEPTH_TEST); //Włącz test głębokości pikseli
	glfwSetKeyCallback(window, key_callback); //Zarejestruj procedurę obsługi klawiatury
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
}

// Generate a half-torus by limiting the angle to 180 degrees




//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle, float wheelAngle) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyszczenie bufora kolorów i bufora głębokości

	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //Wyliczenie macierzy rzutowania
	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 2.0f, -10.0f), glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Wyliczenie macierzy widoku

	spLambert->use();//Aktywacja programu cieniującego
	glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P)); //Załadowanie macierzy rzutowania do programu cieniującego
	glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V)); //Załadowanie macierzy widoku do programu cieniującego


	
	glm::mat4 Ms = glm::mat4(1.0f);
	Ms = glm::rotate(Ms, angle, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 leftBar = glm::translate(Ms, glm::vec3(-0.8f, -0.35f, 0.0f)); // Move left
	leftBar = glm::scale(leftBar, glm::vec3(0.2f, 1.0f, 0.2f)); // Scale to make it vertical
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(leftBar));
	Models::cube.drawSolid(); // Draw the left vertical bar

	// Right vertical bar
	glm::mat4 rightBar = glm::translate(Ms, glm::vec3(0.8f, -0.35f, 0.0f)); // Move right
	rightBar = glm::scale(rightBar, glm::vec3(0.2f, 1.0f, 0.2f)); // Scale to make it vertical
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(rightBar));
	Models::cube.drawSolid(); // Draw the right vertical bar

	// Left diagonal bar
	glm::mat4 leftDiagonal = glm::translate(Ms, glm::vec3(0.35f, 0.0f, 0.0f)); // Move to the left middle
	leftDiagonal = glm::rotate(leftDiagonal, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate to make it diagonal
	leftDiagonal = glm::scale(leftDiagonal, glm::vec3(0.2f, 0.7f, 0.2f)); // Scale to adjust thickness and length
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(leftDiagonal));
	Models::cube.drawSolid(); // Draw the left diagonal bar

	// Right diagonal bar
	glm::mat4 rightDiagonal = glm::translate(Ms, glm::vec3(-0.35f, 0.0f, 0.0f)); // Move to the right middle
	rightDiagonal = glm::rotate(rightDiagonal, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate to make it diagonal
	rightDiagonal = glm::scale(rightDiagonal, glm::vec3(0.2f, 0.7f, 0.2f)); // Scale to adjust thickness and length
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(rightDiagonal));
	Models::cube.drawSolid(); // Draw the right diagonal bar


	// Draw letter "R"
	glm::mat4 Mr = translate(Ms, glm::vec3(-1.5f, -0.35f, 0.0f));
	// Vertical bar
	glm::mat4 verticalBar = glm::scale(Mr, glm::vec3(0.2f, 1.0f, 0.2f)); // Scale to make it vertical
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(verticalBar));
	Models::cube.drawSolid(); // Draw the vertical bar

	// Half-torus (curved part of "R")
	glm::mat4 halfToruss = glm::translate(Mr, glm::vec3(-0.18f, 0.25f, 0.0f)); // Move up for the curved part
	halfToruss = glm::rotate(halfToruss, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(halfToruss));
	Models::halfTorus.drawSolid(); // Draw the half-torus

	// Diagonal leg
	glm::mat4 diagonalLeg = glm::translate(Mr, glm::vec3(-0.45f, -0.5f, 0.0f)); // Move to the diagonal leg position
	diagonalLeg = glm::rotate(diagonalLeg, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate to diagonal
	diagonalLeg = glm::scale(diagonalLeg, glm::vec3(0.2f, 0.5f, 0.2f)); // Scale to adjust length
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(diagonalLeg));
	Models::cube.drawSolid(); // Draw the diagonal leg


	//letter I
	glm::mat4 I = glm::translate(Ms, glm::vec3(0.0f, -3.0f, 0.0f)); // Move to the right middle
	I = glm::scale(I, glm::vec3(0.2f, 0.7f, 0.2f)); // Scale to adjust thickness and length
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(I));
	Models::cube.drawSolid(); // Draw the right diagonal bar


	// Draw letter "Ś"
	glm::mat4 Mss = glm::translate(Ms, glm::vec3(-1.3f, -3.0f, 0.0f));
	// Top half-torus
	Models::HalfTorus halftorus(0.3, 0.2, 12, 12);
	glm::mat4 topHalfTorus = glm::translate(Mss, glm::vec3(0.0f, 0.3f, 0.0f)); // Move up for the curved part
	topHalfTorus = glm::rotate(topHalfTorus, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate slightly to the left
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(topHalfTorus));
	halftorus.drawSolid();

	// Bottom half-torus
	glm::mat4 bottomHalfTorus = glm::translate(Mss, glm::vec3(0.0f, -0.45f, 0.0f)); // Position the bottom half-torus
	bottomHalfTorus = glm::rotate(bottomHalfTorus, glm::radians(200.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate to face downward
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(bottomHalfTorus));
	halftorus.drawSolid(); // Draw bottom half-torus

	// Middle bar
	glm::mat4 middleBar = glm::translate(Mss, glm::vec3(0.0f, 0.0f, 0.0f)); // Position the middle bar
	middleBar = glm::rotate(middleBar, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate slightly to the left
	middleBar = glm::scale(middleBar, glm::vec3(0.1f, 0.6f, 0.2f)); // Scale to adjust size
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(middleBar));
	Models::cube.drawSolid(); // Draw middle bar

	// Top diagonal bar
	glm::mat4 topDiagonalBar = glm::translate(Mss, glm::vec3(-0.1f, 1.1f, 0.0f)); // Position the top diagonal bar
	topDiagonalBar = glm::rotate(topDiagonalBar, glm::radians(60.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate diagonally
	topDiagonalBar = glm::scale(topDiagonalBar, glm::vec3(0.05f, 0.3f, 0.2f)); // Scale to adjust size
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(topDiagonalBar));
	Models::cube.drawSolid(); // Draw top diagonal bar


	//Skopiowanie bufora ukrytego do widocznego. Z reguły ostatnie polecenie w procedurze drawScene.
	glfwSwapBuffers(window);
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące


	float angle = 0; //Aktualny kąt obrotu obiektu
	float wheelAngle = 0; //Aktualny kąt obrotu kół
	glfwSetTime(0); //Wyzeruj timer
	//Główna pętla
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		angle += speed * glfwGetTime(); //Oblicz przyrost kąta po obrocie
		wheelAngle += -PI / 6 * glfwGetTime();
		glfwSetTime(0); //Wyzeruj timer
		drawScene(window, angle, wheelAngle); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
