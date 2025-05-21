#include "Render.h"
#include "Prism.h"

#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <random>

void Render(double delta_time)
{
    // 7 контрольных точек
    float points[7][3] = {
        { -8.0f, -4.0f, 1.0f },
        { -6.0f,  6.0f, 2.0f },
        { -2.0f, 10.0f, 1.0f },
        {  0.0f,  4.0f, 3.0f },
        {  4.0f,  8.0f, 1.0f },
        {  6.0f,  0.0f, 4.0f },
        {  8.0f,  6.0f, 5.0f }
    };

    glColor3f(0.2f, 0.7f, 1.0f);
    glLineWidth(2.0f);

    // Рисуем B-сплайн
    glBegin(GL_LINE_STRIP);
    for (int i = 1; i < 6; ++i) {
        for (int j = 0; j <= 32; ++j) {
            float t = j / 32.0f;
            float b0 = (1 - t) * (1 - t) / 2.0f;
            float b1 = (1 + 2 * t - 2 * t * t) / 2.0f;
            float b2 = t * t / 2.0f;
            float x = b0 * points[i - 1][0] + b1 * points[i][0] + b2 * points[i + 1][0];
            float y = b0 * points[i - 1][1] + b1 * points[i][1] + b2 * points[i + 1][1];
            float z = b0 * points[i - 1][2] + b1 * points[i][2] + b2 * points[i + 1][2];
            glVertex3f(x, y, z);
        }
    }
    glEnd();

    // --- Призма едет по кривой туда-обратно ---
    static float anim = 0.0f;
    static int direction = 1; // 1 — вперёд, -1 — назад
    anim += delta_time * 0.2f * direction; // скорость движения

    // Проверяем границы и меняем направление
    if (anim > 5.0f) {
        anim = 5.0f;
        direction = -1;
    }
    if (anim < 0.0f) {
        anim = 0.0f;
        direction = 1;
    }

    int seg = (int)anim + 1; // сегмент (от 1 до 5)
    float t = anim - (int)anim; // локальный t на сегменте

    // Безопасность индексов
    if (seg < 1) seg = 1;
    if (seg > 5) seg = 5;

    // Вычисляем положение призмы на кривой
    float b0 = (1 - t) * (1 - t) / 2.0f;
    float b1 = (1 + 2 * t - 2 * t * t) / 2.0f;
    float b2 = t * t / 2.0f;
    Point O;
    O.x = b0 * points[seg - 1][0] + b1 * points[seg][0] + b2 * points[seg + 1][0];
    O.y = b0 * points[seg - 1][1] + b1 * points[seg][1] + b2 * points[seg + 1][1];
    O.z = b0 * points[seg - 1][2] + b1 * points[seg][2] + b2 * points[seg + 1][2];

    // --- Вычисляем касательный вектор (производная сплайна) ---
    float db0 = -(1 - t);
    float db1 = 1 - 2 * t;
    float db2 = t;
    float dx = db0 * points[seg - 1][0] + db1 * points[seg][0] + db2 * points[seg + 1][0];
    float dy = db0 * points[seg - 1][1] + db1 * points[seg][1] + db2 * points[seg + 1][1];
    float dz = db0 * points[seg - 1][2] + db1 * points[seg][2] + db2 * points[seg + 1][2];

    // Если движемся назад — инвертируем направление
    if (direction == -1) {
        dx = -dx;
        dy = -dy;
        dz = -dz;
    }

    // Нормализуем вектор направления
    float len = sqrt(dx * dx + dy * dy + dz * dz);
    if (len < 1e-6f) len = 1.0f; // чтобы не делить на 0
    dx /= len; dy /= len; dz /= len;

    // --- Ориентируем призму вдоль направления движения ---
    glPushMatrix();
    glTranslatef((float)O.x, (float)O.y, (float)O.z);

    // Ось X призмы должна совпадать с (dx, dy, dz)
    // Для этого найдём угол и ось вращения между (1,0,0) и (dx,dy,dz)
    float angle = acosf(dx) * 180.0f / 3.14159265f; // угол между (1,0,0) и (dx,dy,dz)
    float rx = 0.0f, ry = -dz, rz = dy; // ось вращения (перпендикулярна обоим векторам)
    float rlen = sqrt(rx * rx + ry * ry + rz * rz);
    if (rlen > 1e-6f) {
        rx /= rlen; ry /= rlen; rz /= rlen;
        glRotatef(angle, rx, ry, rz);
    }

    // Рисуем призму в текущей позиции и ориентации
    Squads(Point{ 0,0,0 });
    glPopMatrix();

}