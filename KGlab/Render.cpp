#include "Render.h"

#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

void Render(double delta_time)
{
    // Вертикальная линия
    {
        float P0[2] = { 2.5f, 3.5f }; // верхняя точка край
        float P1[2] = { 3.5f, 0.5f };  // средняя точка (контрольная)
        float P2[2] = { 2.5f, -3.5f };  // нижняя край
        glLineWidth(3.0f); // Установить толщину линии в 3 пикселя

        // Рисуем кривую Безье
        glColor3f(0.2f, 0.7f, 1.0f);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 100; ++i) {
            float t = i / 100.0f;
            float x = (1 - t) * (1 - t) * P0[0] + 2 * (1 - t) * t * P1[0] + t * t * P2[0];
            float y = (1 - t) * (1 - t) * P0[1] + 2 * (1 - t) * t * P1[1] + t * t * P2[1];
            glVertex2f(x, y);
        }
        glEnd();
    }

    // Верхняя петля (используем кривую Безье)
    {
        float P0[2] = { -2.5f, 3.5f }; // Левый край
        float P1[2] = { 0.f, 7.5f };  // Верхняя точка (контрольная)
        float P2[2] = { 2.5f, 3.5f };  // Правый край


        // Рисуем кривую Безье
        glColor3f(0.2f, 0.7f, 1.0f);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 100; ++i) {
            float t = i / 100.0f;
            float x = (1 - t) * (1 - t) * P0[0] + 2 * (1 - t) * t * P1[0] + t * t * P2[0];
            float y = (1 - t) * (1 - t) * P0[1] + 2 * (1 - t) * t * P1[1] + t * t * P2[1];
            glVertex2f(x, y);
        }
        glEnd();

        P1[1] = -0.5;
        // Рисуем кривую Безье
        glColor3f(0.2f, 0.7f, 1.0f);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 100; ++i) {
            float t = i / 100.0f;
            float x = (1 - t) * (1 - t) * P0[0] + 2 * (1 - t) * t * P1[0] + t * t * P2[0];
            float y = (1 - t) * (1 - t) * P0[1] + 2 * (1 - t) * t * P1[1] + t * t * P2[1];
            glVertex2f(x, y);
        }
        glEnd();
    }

    // Нижняя петля (используем кривую Безье)
    {
        float P0[2] = { -2.5f, -3.5f }; // Левый край
        float P1[2] = { 0.f, 0.5f };  // Верхняя точка (контрольная)
        float P2[2] = { 2.5f, -3.5f };  // Правый край

        // Рисуем кривую Безье
        glColor3f(0.2f, 0.7f, 1.0f);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 100; ++i) {
            float t = i / 100.0f;
            float x = (1 - t) * (1 - t) * P0[0] + 2 * (1 - t) * t * P1[0] + t * t * P2[0];
            float y = (1 - t) * (1 - t) * P0[1] + 2 * (1 - t) * t * P1[1] + t * t * P2[1];
            glVertex2f(x, y);
        }
        glEnd();

        P1[1] = -7.5;
        // Рисуем кривую Безье
        glColor3f(0.2f, 0.7f, 1.0f);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 100; ++i) {
            float t = i / 100.0f;
            float x = (1 - t) * (1 - t) * P0[0] + 2 * (1 - t) * t * P1[0] + t * t * P2[0];
            float y = (1 - t) * (1 - t) * P0[1] + 2 * (1 - t) * t * P1[1] + t * t * P2[1];
            glVertex2f(x, y);
        }
        glEnd();
    }
}


