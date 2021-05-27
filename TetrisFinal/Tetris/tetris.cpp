#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <GL/glut.h>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

// Tablica kolor�w RGB
float colors[][3] = {
    0.95f, 0.95f, 0.95f,    // Szary
    1.0f, 1.0f, 1.0f,       // Bia�y
    1.0f, 0.0f, 0.0f,       // Czerwony 
    1.0f, 0.90f, 0.0f,      // ��ty
    0.2f, 0.6f, 1.0f,       // Niebieski
    0.0f, 0.8f, 0.4f,       // Zielony 
    1.0f, 0.49f, 0.0f,      // Pomara�czowy
    1.0f, 0.0f, 1.0f,       // R�owy
};

float colors2[][3] = {
    0.95f, 0.95f, 0.95f,    // Szary
    1.0f, 1.0f, 1.0f,       // Bia�y
    0.6f, 0.0f, 0.0f,       // Czerwony 
    0.74f, 0.67f, 0.07f,      // ��ty
    0.2f, 0.1f, 0.7f,       // Niebieski
    0.0f, 0.5f, 0.4f,       // Zielony
    1.0f, 0.8f, 0.0f,      // Pomara�zcowy
    0.65f, 0.0f, 1.0f,       // R�owy
};

// Zmienne dla planszy, kszta�tu oraz wyniku
int board[10][25] = { {0} };
int curr_shape[4][2] = { { -1 } };
int currShapeType = 0;
int currShapeColor = 0;
int direction = 1;
int tempPoints[4][2];
int score = 0;
int oldScore = 0;
int highScore = 0;
string scoreStr = "Wynik: 0";
string highScoreStr = "Najwy�szy wynik: 0";

// Funkcje pomocnicze s�u��ce do tworzenia, rotacji oraz wy�wietlania "klock�w" oraz wyniku. 
void drawSquare(float x, float y, float size, int cIndex );
void newShape();
void drawBoxes();
void clearMovingShape();
void moveShapeLeft();
void moveShapeRight();
void moveShapeDown();
void rotateLineShape();
void rotateLShape();
void rotateSShape();
void rotateTShape();
void rotateShape();
void rotationUpdate();
void removeCompleteLines();
void gameFinishCheck();
void displayGameName();
void displayScore();
void displayText(int x, int y, const string& str);
void displayHighScore();
void saveHighScore();
void getHighScore();

// G��wna funkcja s�u��ca do wy�wietlania 
void display() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Ustawianie t�a na kolor czarny
    glClear(GL_COLOR_BUFFER_BIT);         // Czyszczenie bufora kolor�w

    gameFinishCheck();
    displayGameName();
    displayScore();
    displayHighScore();
    moveShapeDown();
    drawBoxes();

    glFlush();  // Renderowanie
}

// Funkcja s�u��ca obs�udze przycisk�w do rotacji "klock�w"
void NonPrintableKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_DOWN)
        glutPostRedisplay();
    else {
        if (key == GLUT_KEY_LEFT)
            moveShapeLeft();
        else if (key == GLUT_KEY_RIGHT)
            moveShapeRight();
        else if (key == GLUT_KEY_UP)
            rotateShape();
        drawBoxes();
        glFlush();  // Renderowanie
    }
}

// Funkcja odpowiadaj�ca za pr�dko�� "spadania klocka" 
void Timer(int m) {
    glutPostRedisplay();
    //  Czas w kt�rym "klocek" pokonuje wysoko�� jednej kratki
    glutTimerFunc(500, Timer, 0);
}



int main(int argc, char** argv) {
    srand(time(NULL));
    getHighScore();
    int width = 480, height = 480;

    glutInit(&argc, argv);                
    glutInitDisplayMode(GLUT_RGBA); // Ustawienie kolor�w RGB
    glutInitWindowSize(width, height);   // Ustawianie wielko�ci okna aplikacji
    glutInitWindowPosition(400, 180); // Pozycjonowanie okna aplikacji
    glutCreateWindow("Tetris Game"); // Nazwa okna aplikacji

    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1); // Ustawianie rozmiaru okna aplikacji do podanej warto�ci
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    newShape();
    glutDisplayFunc(display); // Ustawienie funkcji odpowiedzialnej za rysowanie
    glutSpecialFunc(NonPrintableKeys); // Funkcja ta sprawia, �e przyciski kontroli s� niewpisywalne w trakcie gry
    glutTimerFunc(5 / 5, Timer, 0);

    glutMainLoop();           // Wej�cie do g��wnej p�tli programu 
    return 0;
}

// Draw Square: Tworzy cz�� sk�adow� klocka w odpowiednim miejscu. 
void drawSquare(float x, float y, float size, int cIndex ) {
    glBegin(GL_QUADS);              //  4 wierzcho�ki sk�adaj� si� na figur� wyj�ciow�
    glColor3f( colors[cIndex][0], colors[cIndex][1], colors[cIndex][2]);
    glVertex2f(x + size, y + size);
    glVertex2f(x, y + size);
    glVertex2f(x, y);
    glColor3f(colors2[cIndex][0], colors2[cIndex][1], colors2[cIndex][2]);
    glVertex2f(x + size, y);
    glEnd();
}

// New Shape: Funkcja s�u��ca do tworzenia losowych kszta�t�w "klock�w" 
void newShape() {
    int randNum = rand() % 5;
    currShapeType = randNum;
    currShapeColor = rand() % 6 + 2;
    direction = 1;

    // Pod�u�ny klocek
    if (randNum == 0) {
        //Wsp�rz�dne  x 
        curr_shape[0][0] = rand() % 7;
        curr_shape[1][0] = curr_shape[0][0] + 1;
        curr_shape[2][0] = curr_shape[0][0] + 2;
        curr_shape[3][0] = curr_shape[0][0] + 3;
        // Wsp�rz�dne  y
        curr_shape[0][1] = curr_shape[1][1] = curr_shape[2][1] = curr_shape[3][1] = 21;
    }

    // Kwadrat
    else if (randNum == 1) {
        // Wsp�rz�dne  x 
        curr_shape[0][0] = rand() % 9;
        curr_shape[1][0] = curr_shape[0][0] + 1;
        curr_shape[2][0] = curr_shape[0][0];
        curr_shape[3][0] = curr_shape[1][0];
        // Wsp�rz�dne  y
        curr_shape[0][1] = curr_shape[1][1] = 22;
        curr_shape[2][1] = curr_shape[3][1] = 21;
    }

    //Klocek w kszta�cie litery L
    else if (randNum == 2) {
        // Wsp�rz�dne  x 
        curr_shape[0][0] = rand() % 8;
        curr_shape[1][0] = curr_shape[0][0];
        curr_shape[2][0] = curr_shape[0][0] + 1;
        curr_shape[3][0] = curr_shape[1][0] + 2;
        // Wsp�rz�dne  y
        curr_shape[0][1] = 22;
        curr_shape[1][1] = curr_shape[2][1] = curr_shape[3][1] = 21;
    }

    // Klocek w kszta�cie litery S 
    else if (randNum == 3) {
        // Wsp�rz�dne  x 
        curr_shape[0][0] = rand() % 8 + 1;
        curr_shape[1][0] = curr_shape[0][0] + 1;
        curr_shape[2][0] = curr_shape[0][0];
        curr_shape[3][0] = curr_shape[0][0] - 1;
        // Wsp�rz�dne  y
        curr_shape[0][1] = curr_shape[1][1] = 22;
        curr_shape[2][1] = curr_shape[3][1] = 21;
    }

    // Klocek w kszta�cie litery T 
    else if (randNum == 4) {
        // Wsp�rz�dne  x 
        curr_shape[0][0] = rand() % 8;
        curr_shape[1][0] = curr_shape[0][0] + 1;
        curr_shape[2][0] = curr_shape[0][0] + 2;
        curr_shape[3][0] = curr_shape[1][0];
        // Wsp�rz�dne y
        curr_shape[0][1] = curr_shape[1][1] = curr_shape[2][1] = 22;
        curr_shape[3][1] = 21;
    }
}

// Clear Moving Shape: Usuwanie pozycji poprzedniego "klocka" (Przywracanie pozycji do domy�lnej) 
void clearMovingShape() {
    for (int a = 0; a < 10; a++) {
        for (int b = 0; b < 20; b++) {
            if (board[a][b] == 1)
                board[a][b] = 0;
        }
    }
}

// Move Shape Left: Pozwala na przesuni�cie "klocka" w lewo je�eli jest taka mo�liwo��
void moveShapeLeft() {
    // Sprawdzanie czy "klocek" mo�e zosta� przesuni�ty w lewo
    int canMove = 1;
    for (int a = 0; a < 4; a++) {
        if (curr_shape[a][0] == 0 || (board[curr_shape[a][0] - 1][curr_shape[a][1]] > 1))
            canMove = 0;
    }
    // Je�eli "klocek" mo�e zosta� przesuni�ty ustaw nowe wsp�rz�dne po przesuni�ciu
    if (canMove) {
        for (int a = 0; a < 4; a++)
            board[curr_shape[a][0]][curr_shape[a][1]] = 0;
        for (int a = 0; a < 4; a++)
            curr_shape[a][0] -= 1;
    }
}

// Move Shape Right: Pozwala na przesuni�cie "klocka" w prawo je�eli jest taka mo�liwo��
void moveShapeRight() {
    // Sprawdzanie czy "klocek" mo�e zosta� przesuni�ty w prawo
    int canMove = 1;
    for (int a = 0; a < 4; a++) {
        if (curr_shape[a][0] == 9 || (board[curr_shape[a][0] + 1][curr_shape[a][1]] > 1))
            canMove = 0;
    }
    // Je�eli "klocek" mo�e zosta� przesuni�ty ustaw nowe wsp�rz�dne po przesuni�ciu
    if (canMove) {
        for (int a = 0; a < 4; a++)
            board[curr_shape[a][0]][curr_shape[a][1]] = 0;
        for (int a = 0; a < 4; a++)
            curr_shape[a][0] += 1;
    }
}

// Rotate Line Shape: Rotacja pod�u�nego "klocka"
void rotateLineShape() {
    if (direction == 1 || direction == 3) {
        // Nowe wsp�rz�dne  x 
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[1][0] = tempPoints[2][0] = tempPoints[3][0] = tempPoints[0][0];
        // Nowe wsp�rz�dne   y
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[0][1] - 1;
        tempPoints[2][1] = tempPoints[0][1] - 2;
        tempPoints[3][1] = tempPoints[0][1] - 3;
    }
    else if (direction == 2 || direction == 4) {
        // Nowe wsp�rz�dne   x 
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[1][0] = tempPoints[0][0] + 1;
        tempPoints[2][0] = tempPoints[0][0] + 2;
        tempPoints[3][0] = tempPoints[0][0] + 3;
        // Nowe wsp�rz�dne   y
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[2][1] = tempPoints[3][1] = tempPoints[0][1];
    }
}

// Rotate L Shape: Rotacja "klocka" w kszta�cie litery L
void rotateLShape() {
    if (direction == 1) {
        // Nowe wsp�rz�dne   x 
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[1][0] = tempPoints[0][0] + 1;
        tempPoints[2][0] = tempPoints[0][0];
        tempPoints[3][0] = tempPoints[0][0];
        // Nowe wsp�rz�dne   y
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[0][1];
        tempPoints[2][1] = tempPoints[0][1] - 1;
        tempPoints[3][1] = tempPoints[0][1] - 2;
    }
    else if (direction == 2) {
        // Nowe wsp�rz�dne   x 
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[1][0] = tempPoints[0][0] + 1;
        tempPoints[2][0] = tempPoints[0][0] + 2;
        tempPoints[3][0] = tempPoints[0][0] + 2;
        // Nowe wsp�rz�dne   y
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[2][1] = tempPoints[0][1];
        tempPoints[3][1] = tempPoints[2][1] - 1;
    }
    else if (direction == 3) {
        // Nowe wsp�rz�dne   x 
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[0][0] = tempPoints[0][0] + 1;
        tempPoints[1][0] = tempPoints[2][0] = tempPoints[0][0];
        tempPoints[3][0] = tempPoints[0][0] - 1;
        // Nowe wsp�rz�dne   y
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[0][1] - 1;
        tempPoints[2][1] = tempPoints[0][1] - 2;
        tempPoints[3][1] = tempPoints[0][1] - 2;
    }
    else if (direction == 4) {
        // Nowe wsp�rz�dne   x 
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[0][0] = tempPoints[0][0] - 1;
        tempPoints[1][0] = tempPoints[0][0];
        tempPoints[2][0] = tempPoints[0][0] + 1;
        tempPoints[3][0] = tempPoints[0][0] + 2;
        // Nowe wsp�rz�dne   y
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[2][1] = tempPoints[3][1] = tempPoints[0][1] - 1;
    }
}

// Rotate S Shape: Rotacja klocka w kszta�cie litery S
void rotateSShape() {
    if (direction == 1 || direction == 3) {
        // Nowe wsp�rz�dne   x 
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[0][0] = tempPoints[0][0] - 1;
        tempPoints[1][0] = tempPoints[0][0];
        tempPoints[2][0] = tempPoints[0][0] + 1;
        tempPoints[3][0] = tempPoints[0][0] + 1;
        // Nowe wsp�rz�dne   y
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[2][1] = tempPoints[0][1] - 1;
        tempPoints[3][1] = tempPoints[0][1] - 2;
    }
    else if (direction == 2 || direction == 4) {
        // Nowe wsp�rz�dne   x 
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[0][0] = tempPoints[0][0] + 1;
        tempPoints[1][0] = tempPoints[0][0] + 1;
        tempPoints[2][0] = tempPoints[0][0];
        tempPoints[3][0] = tempPoints[0][0] - 1;
        // Nowe wsp�rz�dne   y
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[0][1];
        tempPoints[2][1] = tempPoints[0][1] - 1;
        tempPoints[3][1] = tempPoints[2][1];
    }
}

// Rotate T Shape: Rotacja klocka w kszta�cie litery T
void rotateTShape() {
    if (direction == 1) {
        // Nowe wsp�rz�dne   x 
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[0][0] = tempPoints[0][0] + 1;
        tempPoints[1][0] = tempPoints[2][0] = tempPoints[0][0];
        tempPoints[3][0] = tempPoints[0][0] - 1;
        // Nowe wsp�rz�dne   y
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[0][1] - 1;
        tempPoints[2][1] = tempPoints[0][1] - 2;
        tempPoints[3][1] = tempPoints[0][1] - 1;
    }
    else if (direction == 2) {
        // Nowe wsp�rz�dne   x 
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[1][0] = tempPoints[0][0] - 1;
        tempPoints[2][0] = tempPoints[0][0];
        tempPoints[3][0] = tempPoints[0][0] + 1;
        // new Wsp�rz�dne  y
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[2][1] = tempPoints[3][1] = tempPoints[0][1] - 1;
    }
    else if (direction == 3) {
        // Nowe wsp�rz�dne   x 
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[0][0] = tempPoints[0][0] - 1;
        tempPoints[1][0] = tempPoints[2][0] = tempPoints[0][0];
        tempPoints[3][0] = tempPoints[0][0] + 1;
        // Nowe wsp�rz�dne  y
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[0][1] - 1;
        tempPoints[2][1] = tempPoints[0][1] - 2;
        tempPoints[3][1] = tempPoints[0][1] - 1;
    }
    else if (direction == 4) {
        // Nowe wsp�rz�dne   x 
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[1][0] = tempPoints[0][0] + 1;
        tempPoints[2][0] = tempPoints[0][0] + 2;
        tempPoints[3][0] = tempPoints[0][0] + 1;
        // Nowe wsp�rz�dne   y
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[2][1] = tempPoints[0][1];
        tempPoints[3][1] = tempPoints[0][1] - 1;
    }
}

// Remove Complete Lines: Usuwanie ca�kowicie wype�nionego rz�du
void removeCompleteLines() {
    int count = 0;
    for (int a = 0; a < 20; a++) {
        // sprawdzanie czy rz�d jest ca�kowicie wype�niony
        count = 0;
        for (int b = 0; b < 10; b++)
        {
            if ((board[b][a] != 1) && (board[b][a] > 1)) {
                count++;
            }
        }
        // je�eli jest kompletnie wype�niony -> usu�
        if (count == 10) {
            for (int i = a; i < 20; i++) {
                for (int b = 0; b < 10; b++) {
                    if (board[b][i + 1] != 1)
                        board[b][i] = board[b][i + 1];
                    else
                        board[b][i] = 0;
                }
            }
            a--;
            score++;    // Podniesienie punktacji o 1
        }
    }
}

// Draw Boxes: Wype�nianie kolorem gotowych kszta�t�w klock�w
void drawBoxes() {
    clearMovingShape();

    for (int a = 0; a < 4; a++) {
        board[curr_shape[a][0]][curr_shape[a][1]] = 1;
    }

    for (int a = 0; a < 10; a++) {
        for (int b = 0; b < 20; b++) {
            drawSquare(a * 24, b * 24, 23, board[a][b] );
            if (board[a][b] == 1) {
                drawSquare(a * 24, b * 24, 23, currShapeColor );
            }
        }
    }
}

// Rotate Shape: Wykonaj odpowiedni� funkcj� obracania zgodnie z kszta�tem
void rotateShape() {
    // Je�eli kszta�t jest kwadratem nie r�b nic
    if (currShapeType == 1) {
    }
    // Rotacje innych kszta�t�w
    else {
        if (currShapeType == 0)
            rotateLineShape();
        else if (currShapeType == 2)
            rotateLShape();
        else if (currShapeType == 3)
            rotateSShape();
        else if (currShapeType == 4)
            rotateTShape();
        rotationUpdate();
    }
}

// Move Shape Down: R�czne przesuwanie "klocka" o jedn� kratk� w d�
void moveShapeDown() {
    // Sprawdzanie czy "klocek" mo�e zosta� przesuni�ty o jedn� kratk� w d�
    int stop = 0;
    for (int a = 0; a < 4; a++) {
        if (curr_shape[a][1] == 0 || (board[curr_shape[a][0]][curr_shape[a][1] - 1] > 1))
            stop = 1;
    }
    // Je�eli jest mo�liwo�� przesuni�cia -> przesu�
    if (stop == 0) {
        for (int a = 0; a < 4; a++) {
            curr_shape[a][1] -= 1;
        }
    }
    // Je�eli "klocek" nie jest w stanie porusza� si� ju� w d�, sprawdzane jest czy rz�d zosta� wype�niony,
    //je�eli tak usuwa rz�d dodaj�c punkty, je�eli nie zostaje stworzony nowy "klocek"
    else {
        for (int a = 0; a < 4; a++) {
            board[curr_shape[a][0]][curr_shape[a][1]] = currShapeColor;
        }
        removeCompleteLines();
        newShape();
    }
}

// Rotation Update: Zrotuj aktualny "klocek" je�eli jest taka mo�liwo��
void rotationUpdate() {
    // Sprawdzanie czy aktualny "klocek" mo�e zosta� zrotowany
    int otherShape = 0;
    int i = -1, j = -1;
    for (int a = 0; a < 4; a++) {
        i = tempPoints[a][0];
        j = tempPoints[a][1];
        if (i >= 0 && i <= 9 && j >= 0)
        {
            if (board[tempPoints[a][0]][tempPoints[a][1]] > 1)
                otherShape = 1;
        }
        else
            otherShape = 1;

    }
    // Je�eli aktualny "klocek" mo�e zosta� zrotowany -> ustaw nowe wsp�rz�dne
    if (otherShape == 0) {
        for (int a = 0; a < 4; a++) {
            curr_shape[a][0] = tempPoints[a][0];
            curr_shape[a][1] = tempPoints[a][1];
        }
        if (direction >= 4)
            direction = 1;
        else
            direction++;
    }
}

// Game Finish Check:Sprawdzanie czy gra zosta�a zako�czona lub czy plansza zosta�a wype�niona
void gameFinishCheck() {
    // Sprawdzanie czy plansza zosta�a wype�niona
    int complete = 0;
    for (int a = 0; a < 10; a++) {
        if (board[a][19] > 1)
            complete = 1;
    }
    if (complete) {
        // Por�wnywanie aktualnego wyniku z aktualnym najwy�szym wynikiem, je�eli aktualny wynik jest wi�kszy -> podmie� warto�ci
        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }
        // Wyczy�� aktualn� punktacj� oraz plansz�
        score = 0;
        for (int a = 0; a < 10; a++) {
            for (int b = 0; b < 25; b++) {
                board[a][b] = 0;
            }
        }
        newShape();
    }
}

// Display Game Name: Wy�wietlanie nazwy gry nad wynikiem
void displayGameName() {
    displayText(320, 400, "Tetris");
}

// Display Score: S�u�y do wy�wietlania aktualnej punktacji na ekranie 
void displayScore() {
    // Aktualizacja wyniku
    if (score != oldScore) {
        oldScore = score;
        stringstream s;
        s << score;
        string tmpStr = "";
        s >> tmpStr;
        scoreStr = "Wynik: " + tmpStr;
    }
    displayText(300, 350, scoreStr);
}

// Display Text: Funkcja odpowiadaj�ca za wypisywanie znak�w
void displayText(int x, int y, const string& str)
{
    GLvoid* font_style = GLUT_BITMAP_TIMES_ROMAN_24;
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(x, y, 1);
    //  Wypisywanie znak�w jeden po drugim
    for (int a = 0; a < str.size(); a++)
        glutBitmapCharacter(font_style, str[a]);
}

// Display High Score: S�u�y do wy�wietlania ostatniego najwi�kszego wyniku w oknie aplikacji
void displayHighScore() {
    displayText(280, 300, highScoreStr);
}

// Save High Score: Zapisywanie najlepszego wyniku w pliku tekstowym
void saveHighScore() {
    fstream file;
    file.open("High_Score.txt", ios::out | ios::trunc);
    file << score;
    file.close();

    file.open("High_Score.txt");
    highScoreStr = "";
    file >> highScoreStr;
    file.close();
    highScoreStr = "Najwy�szy wynik: " + highScoreStr;
}

// Get High Score: Pobieranie najlepszego wyniku z pliku tekstowego 
void getHighScore() {
    fstream file;
    file.open("High_Score.txt");
    file >> highScore;
    file.close();

    file.open("High_Score.txt");
    file >> highScoreStr;
    file.close();
    highScoreStr = "Najwy�szy wynik: " + highScoreStr;
}