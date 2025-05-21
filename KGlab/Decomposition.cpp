#include "Decomposition.h"
#include <random>
#include <gl/GL.h>

void WriteNormalTriangle(Point A, Point B, Point C) {
	Point N = getNormal(A, B, C);
	glNormal3dv(N.p());
	//displayNormal(A, B, C, N);
}

// призма на 40
void SemiCircle(std::mt19937& gen, std::uniform_real_distribution<double>& r) {
	glBegin(GL_TRIANGLES);
	// задаем краевые точки наших окружности
	Point B = { 4, 1. / 2,0 };
	Point C = { 3. / 2, 2, 0 };
	// центры окружности на разных высотах
	Point O = { (B.x + C.x) / 2., (B.y + C.y) / 2., 0 };
	Point O2 = { (B.x + C.x) / 2., (B.y + C.y) / 2., 1 };
	// вычисляем радиус
	double R = sqrt((O.x - C.x) * (O.x - C.x) + (O.y - C.y) * (O.y - C.y));
	// 42 братуха
	int num_segments = 42;
	// вычисляем начальный сдвиг
	double StartAngle = -atan(abs(C.y - B.y) / abs(C.x - B.x));
	// задаем координаты концов вектора (тоже на разных высотах)
	Point EndOfVector = { O.x, O.y, 0 };
	Point EndOfVector2 = { O.x, O.y, 1 };
	// угол на который двигаемся
	double Angle = M_PI / num_segments;
	// временные переменные для хранения текущих значений
	Point Temp, Temp2;

	// крутим угол
	for (double i = StartAngle; i <= (M_PI + EPSILON + StartAngle); i += Angle) {

		// красим сегмент разблокировать если надо
		glColor3d(r(gen), r(gen), r(gen));
		// задаем начальную точку
		glVertex3dv(O.p());
		// задаем начальную точку конца вектора
		glVertex3dv(EndOfVector.p());
		// сохраняем значение точки перед тем как его менять
		Temp = EndOfVector;
		// получаем новые значения точек после сдвига
		EndOfVector.x = O.x + R * cos(i);
		EndOfVector.y = O.y + R * sin(i);
		// рисуем треуголник 
		glVertex3dv(EndOfVector.p());
		WriteNormalTriangle(EndOfVector,Temp,O);

		// делаем тоже самое на другой высоте
		glTexCoord3dv(O2.p());
		glVertex3dv(O2.p());
		glTexCoord3dv(EndOfVector2.p());
		glVertex3dv(EndOfVector2.p());

		Temp2 = EndOfVector2;

		EndOfVector2.x = O2.x + R * cos(i);
		EndOfVector2.y = O2.y + R * sin(i);

		glTexCoord3d(O2.x + R * cos(i), O2.y + R * sin(i), 1);
		glVertex3dv(EndOfVector2.p());
		// зарисовываем квадраты, соединяющие окружности
		// 1 раз рисовать не нужно, т.к. еще не нарисовали 1 треугольник
		if (i == StartAngle) continue;
		WriteNormal(EndOfVector2, EndOfVector, Temp, Temp2);
		glVertex3dv(Temp.p());
		glVertex3dv(EndOfVector.p());
		glVertex3dv(Temp2.p());

		glVertex3dv(Temp2.p());
		glVertex3dv(EndOfVector2.p());
		glVertex3dv(EndOfVector.p());
		WriteNormalTriangle(O, Temp, EndOfVector);
	}

	glEnd();
}

// призма на 30
void Squads(Point Gap)// SQUADS
{
	glBegin(GL_QUADS);

	// Все точки призмы уменьшенные в 2 раза и с учетом смещения
	Point A = { (1.0 / 2) / 2 + Gap.x,      (0.0) / 2 + Gap.y,        (0.0) / 2 + Gap.z };
	Point A_t = { (1.0 / 2) / 2 + Gap.x,      (0.0) / 2 + Gap.y,        (1.0) / 2 + Gap.z };

	Point B = { (4.0) / 2 + Gap.x,          (1.0 / 2) / 2 + Gap.y,    (0.0) / 2 + Gap.z };
	Point B_t = { (4.0) / 2 + Gap.x,          (1.0 / 2) / 2 + Gap.y,    (1.0) / 2 + Gap.z };

	Point C = { (3.0 / 2) / 2 + Gap.x,      (2.0) / 2 + Gap.y,        (0.0) / 2 + Gap.z };
	Point C_t = { (3.0 / 2) / 2 + Gap.x,      (2.0) / 2 + Gap.y,        (1.0) / 2 + Gap.z };

	Point D = { (0.0) / 2 + Gap.x,          (1.0 / 2) / 2 + Gap.y,    (0.0) / 2 + Gap.z };
	Point D_t = { (0.0) / 2 + Gap.x,          (1.0 / 2) / 2 + Gap.y,    (1.0) / 2 + Gap.z };

	Point E = { (-3.0) / 2 + Gap.x,         (2.0) / 2 + Gap.y,        (0.0) / 2 + Gap.z };
	Point E_t = { (-3.0) / 2 + Gap.x,         (2.0) / 2 + Gap.y,        (1.0) / 2 + Gap.z };

	Point F = { (-9.0 / 2) / 2 + Gap.x,     (0.0) / 2 + Gap.y,        (0.0) / 2 + Gap.z };
	Point F_t = { (-9.0 / 2) / 2 + Gap.x,     (0.0) / 2 + Gap.y,        (1.0) / 2 + Gap.z };

	Point G = { (-1.0 / 2) / 2 + Gap.x,     (-9.0 / 2) / 2 + Gap.y,   (0.0) / 2 + Gap.z };
	Point G_t = { (-1.0 / 2) / 2 + Gap.x,     (-9.0 / 2) / 2 + Gap.y,   (1.0) / 2 + Gap.z };

	Point H = { (2.0) / 2 + Gap.x,          (-7.0 / 2) / 2 + Gap.y,   (0.0) / 2 + Gap.z };
	Point H_t = { (2.0) / 2 + Gap.x,          (-7.0 / 2) / 2 + Gap.y,   (1.0) / 2 + Gap.z };

	// стенки
	WriteSquare(A, A_t, B_t, B);
	WriteSquare(B, B_t, C_t, C);
	WriteSquare(C, C_t, D_t, D);
	WriteSquare(D, D_t, E_t, E);
	WriteSquare(E, E_t, F_t, F);
	WriteSquare(F, F_t, G_t, G);
	WriteSquare(G, G_t, H_t, H);
	WriteSquare(H, H_t, A_t, A);

	//// пол и потолок
	WriteSquare(A, B, C, D);
	WriteSquare(D, G, H, A);
	WriteSquare(D, E, H, A);
	WriteSquare(D, E, F, G);

	WriteSquare(D_t, C_t, B_t, A_t);
	WriteSquare(A_t, H_t, E_t, D_t);
	WriteSquare(A_t, H_t, G_t, D_t);
	WriteSquare(G_t, F_t, E_t, D_t);

	glEnd();

	// --- рисуем оси лск ---
	glLineWidth(3.0f);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	// ось X (красная)
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3d(Gap.x, Gap.y, Gap.z);
	glVertex3d(Gap.x + 2.5, Gap.y, Gap.z);
	glEnd();

	// Ось Y (зеленая)
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3d(Gap.x, Gap.y, Gap.z);
	glVertex3d(Gap.x, Gap.y + 2.5, Gap.z);
	glEnd();

	// Ось Z (Синяя)
	glColor3f(0.0f, 0.5f, 1.0f);
	glBegin(GL_LINES);
	glVertex3d(Gap.x, Gap.y, Gap.z);
	glVertex3d(Gap.x, Gap.y, Gap.z + 2.5);
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glLineWidth(1.0f);
}

void WriteSquare(Point A, Point B, Point C, Point D) {
	// Вычисляем нормаль для четырёхугольника
	Point N = getNormalSquare(A, B, C, D);
	glColor3d(0.5, 1, 0);
	glNormal3dv(N.p());
	glVertex3dv(A.p());
	glVertex3dv(B.p());
	glVertex3dv(C.p());
	glVertex3dv(D.p());
}


void WriteNormal(Point A, Point B, Point C, Point D) {
	Point N = getNormalSquare(A, B, C, D);
	glNormal3dv(N.p());
	//displayNormalSquare(A, B, C, D, N);
}

void displayNormal(Point A, Point B, Point C, Point N) {
	//Для проверки нарисуем вектор нормали.
	//Так как вектор - это просто направление,
	//для рисования его нужно приложить к какой-то точке.
	//Пусть вектор рисуется из центра полигона.
	//Вычисляем центр
	Point center{ (A.x + B.x + C.x) / 3.0,
	(A.y + B.y + C.y) / 3.0,
	(A.z + B.z + C.z) / 3.0 };
	//тут вектор нормали будет заканчиватся.
	Point N_end{ center.x + N.x,
	center.y + N.y,
	center.z + N.z };
	//рисуем вектор нормали
	//запоминаем, включено ли освещение
	//чтобы восстановить его, если это нужно.
	bool b_light = glIsEnabled(GL_LIGHTING);
	//отключаем освещение, чтобы раскрасить вектор привычным нам glColor
	if (b_light)
		glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3d(1, 0, 0);
	glVertex3dv((double*)&center);
	glVertex3dv((double*)&N_end);
	glEnd();
	//восстанавливаем освещение, если нужно.
	if (b_light)
		glEnable(GL_LIGHTING);
}

Point getNormal(Point A, Point B, Point C) {
	//вычисляем стороны-вектора
	Point BA1{ A.x - B.x,A.y - B.y,A.z - B.z };
	Point BC1{ C.x - B.x,C.y - B.y,C.z - B.z };
	Point N1{ BA1.y * BC1.z - BA1.z * BC1.y, -BA1.x * BC1.z + BA1.z * BC1.x, BA1.x * BC1.y - BA1.y * BC1.x };
	//Нормализуем нормаль
	double l1 = sqrt(N1.x * N1.x + N1.y * N1.y + N1.z * N1.z);
	N1.x /= l1;
	N1.y /= l1;
	N1.z /= l1;


	return N1;
}

Point getNormalSquare(Point A, Point B, Point C, Point D) {
	// Вычисляем нормали для двух треугольников
	Point N1 = getNormal(A, B, C);
	Point N2 = getNormal(A, C, D);

	// Усредняем нормали
	Point N{
		(N1.x + N2.x) / 2.0,
		(N1.y + N2.y) / 2.0,
		(N1.z + N2.z) / 2.0
	};

	// Нормализуем усреднённую нормаль
	double length = sqrt(N.x * N.x + N.y * N.y + N.z * N.z);
	N.x /= length;
	N.y /= length;
	N.z /= length;

	return N;
}

void displayNormalSquare(Point A, Point B, Point C, Point D, Point N) {
	// Вычисляем центр четырёхугольника
	Point center{
		(A.x + B.x + C.x + D.x) / 4.0,
		(A.y + B.y + C.y + D.y) / 4.0,
		(A.z + B.z + C.z + D.z) / 4.0
	};

	// Конечная точка нормали
	Point N_end{
		center.x + N.x,
		center.y + N.y,
		center.z + N.z
	};

	// Рисуем вектор нормали
	bool b_light = glIsEnabled(GL_LIGHTING);
	if (b_light)
		glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3d(1, 0, 0);
	glVertex3dv(center.p());
	glVertex3dv(N_end.p());
	glEnd();
	if (b_light)
		glEnable(GL_LIGHTING);
}