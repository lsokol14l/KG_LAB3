#include "Prism.h"


void WriteSquare(Point A, Point B, Point C, Point D) {
	glColor3d(0.5, 1, 0);
	glVertex3dv(A.p());
	glVertex3dv(B.p());
	glVertex3dv(C.p());
	glVertex3dv(D.p());
};

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

	// потолок
	WriteSquare(A, A_t, B_t, B);
	WriteSquare(B, B_t, C_t, C);
	WriteSquare(C, C_t, D_t, D);
	WriteSquare(D, D_t, E_t, E);
	WriteSquare(E, E_t, F_t, F);
	WriteSquare(F, F_t, G_t, G);
	WriteSquare(G, G_t, H_t, H);
	WriteSquare(H, H_t, A_t, A);

	//// стенки
	WriteSquare(A, B, C, D);
	WriteSquare(D, G, H, A);
	WriteSquare(D, E, H, A);
	WriteSquare(D, E, F, G);

	WriteSquare(A_t, B_t, C_t, D_t);
	WriteSquare(D_t, E_t, H_t, A_t);
	WriteSquare(D_t, G_t, H_t, A_t);
	WriteSquare(D_t, E_t, F_t, G_t);

	glEnd();

	// --- рисуем оси лск ---
	glLineWidth(3.0f);

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

	glLineWidth(1.0f);
}