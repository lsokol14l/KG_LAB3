#include "Render.h"
#include "GUItextRectangle.h"
#include "Decomposition.h"

#include <vector>
#include <iostream>
#include <fstream>

#ifndef GLU_AUTO_NORMAL
#define GLU_AUTO_NORMAL 100108
#endif

#ifdef _DEBUG
#include <Debugapi.h> 
struct debug_print
{
	template<class C>
	debug_print& operator<<(const C& a)
	{
		OutputDebugStringA((std::stringstream() << a).str().c_str());
		return *this;
	}
} debout;
#else
struct debug_print
{
	template<class C>
	debug_print& operator<<(const C& a)
	{
		return *this;
	}
} debout;
#endif

//библиотека для разгрузки изображений
//https://github.com/nothings/stb
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//внутренняя логика "движка"
#include "MyOGL.h"
extern OpenGL gl;
#include "Light.h"
Light light;
#include "Camera.h"
Camera camera;


bool texturing = true;
bool lightning = true;
bool alpha = false;

//переключение режимов освещения, текстурирования, альфаналожения
void switchModes(OpenGL *sender, KeyEventArg arg)
{
	//конвертируем код клавиши в букву
	auto key = LOWORD(MapVirtualKeyA(arg.key, MAPVK_VK_TO_CHAR));

	switch (key)
	{
	case 'L':
		lightning = !lightning;
		break;
	case 'T':
		texturing = !texturing;
		break;
	case 'A':
		alpha = !alpha;
		break;
	}
}

//Текстовый прямоугольничек в верхнем правом углу.
//OGL не предоставляет возможности для хранения текста
//внутри этого класса создается картинка с текстом (через виндовый GDI),
//в виде текстуры накладывается на прямоугольник и рисуется на экране.
//Это самый простой способ что то написать на экране
//но ооооочень не оптимальный
GuiTextRectangle text;

bool LoadNURBSParametersFromFile(const std::string& filename,
	int& uDegree, int& vDegree,
	int& uNumPoints, int& vNumPoints,
	std::vector<std::vector<GLfloat>>& controlPoints,
	std::vector<std::vector<GLfloat>>& weights) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Не удалось открыть файл для NURBS: " << filename << std::endl;
		return false;
	}

	// Чтение степеней поверхности (u и v)
	if (!(file >> uDegree >> vDegree)) {
		std::cerr << "Ошибка при чтении степеней поверхности" << std::endl;
		return false;
	}

	// Чтение количества контрольных точек (по u и v)
	if (!(file >> uNumPoints >> vNumPoints)) {
		std::cerr << "Ошибка при чтении количества контрольных точек" << std::endl;
		return false;
	}

	// Инициализация массива для хранения контрольных точек и весов
	controlPoints.resize(uNumPoints * vNumPoints, std::vector<GLfloat>(3, 0.0f));
	weights.resize(uNumPoints * vNumPoints, std::vector<GLfloat>(1, 1.0f)); // По умолчанию все веса = 1.0

	// Чтение координат контрольных точек и их весов
	for (int i = 0; i < uNumPoints; i++) {
		for (int j = 0; j < vNumPoints; j++) {
			int idx = i * vNumPoints + j;
			if (!(file >> controlPoints[idx][0] >> controlPoints[idx][1] >> controlPoints[idx][2] >> weights[idx][0])) {
				std::cerr << "Ошибка при чтении координат и веса точки [" << i << "," << j << "]" << std::endl;
				return false;
			}
		}
	}

	file.close();
	return true;
}

// Функция для создания узловых векторов (равномерных)
void GenerateUniformKnotVector(int degree, int numPoints, std::vector<GLfloat>& knots) {
	int numKnots = numPoints + degree + 1;
	knots.resize(numKnots);

	// Формируем равномерный узловой вектор
	// Первые degree+1 узлов равны 0
	for (int i = 0; i <= degree; i++) {
		knots[i] = 0.0f;
	}

	// Средние узлы равномерно распределены между 0 и 1
	int numMiddleKnots = numKnots - 2 * (degree + 1);
	if (numMiddleKnots > 0) {
		for (int i = 1; i <= numMiddleKnots; i++) {
			knots[degree + i] = (GLfloat)i / (numMiddleKnots + 1);
		}
	}

	// Последние degree+1 узлов равны 1
	for (int i = numKnots - degree - 1; i < numKnots; i++) {
		knots[i] = 1.0f;
	}
}

void DrawNURBSSurface(int uDegree, int vDegree,
	int uNumPoints, int vNumPoints,
	const std::vector<std::vector<GLfloat>>& controlPoints,
	const std::vector<std::vector<GLfloat>>& weights) {

	// Проверка на наличие GLU NURBS объекта
	GLUnurbsObj* nurbsObject = gluNewNurbsRenderer();
	if (!nurbsObject) {
		std::cerr << "Не удалось создать NURBS объект" << std::endl;
		return;
	}

	// Настройка NURBS объекта
	gluNurbsProperty(nurbsObject, GLU_SAMPLING_TOLERANCE, 25.0);
	gluNurbsProperty(nurbsObject, GLU_DISPLAY_MODE, GLU_FILL);
	//gluNurbsProperty(nurbsObject, GLU_AUTO_NORMAL, GL_TRUE);
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	// Создание узловых векторов
	std::vector<GLfloat> uKnots, vKnots;
	GenerateUniformKnotVector(uDegree, uNumPoints, uKnots);
	GenerateUniformKnotVector(vDegree, vNumPoints, vKnots);

	// Преобразование контрольных точек и весов в формат, необходимый для GLU
	std::vector<GLfloat> ctrlPoints;
	for (int i = 0; i < uNumPoints; i++) {
		for (int j = vNumPoints - 1; j >= 0; j--) { // <-- инвертируем порядок по v
			int idx = i * vNumPoints + j;
			ctrlPoints.push_back(controlPoints[idx][0] * weights[idx][0]);
			ctrlPoints.push_back(controlPoints[idx][1] * weights[idx][0]);
			ctrlPoints.push_back(controlPoints[idx][2] * weights[idx][0]);
			ctrlPoints.push_back(weights[idx][0]);
		}
	}

	// Рисование NURBS поверхности
	glColor3f(0.0, 0.8, 0.2); // Цвет поверхности (зеленый)
	gluBeginSurface(nurbsObject);
	gluNurbsSurface(nurbsObject,
		uKnots.size(), uKnots.data(),
		vKnots.size(), vKnots.data(),
		4 * vNumPoints, 4,  // Шаг между контрольными точками (4, так как каждая точка имеет x, y, z, w)
		ctrlPoints.data(),
		uDegree + 1, vDegree + 1,
		GL_MAP2_VERTEX_4);  // Используем 4D контрольные точки (x, y, z, w)
	gluEndSurface(nurbsObject);

	// Рисование контрольных точек и сетки для наглядности
	// Контрольные точки
	glPointSize(5.0);
	glColor3f(1.0, 0.0, 0.0); // Красный цвет для контрольных точек
	glBegin(GL_POINTS);
	for (int i = 0; i < uNumPoints * vNumPoints; i++) {
		glVertex3f(controlPoints[i][0], controlPoints[i][1], controlPoints[i][2]);
	}
	glEnd();

	// Соединения между контрольными точками (сетка)
	glColor3f(0.5, 0.5, 0.5); // Серый цвет для сетки
	glLineWidth(1.0);

	// Горизонтальные линии
	for (int i = 0; i < uNumPoints; i++) {
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < vNumPoints; j++) {
			int idx = i * vNumPoints + j;
			glVertex3f(controlPoints[idx][0], controlPoints[idx][1], controlPoints[idx][2]);
		}
		glEnd();
	}

	// Вертикальные линии
	for (int j = 0; j < vNumPoints; j++) {
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < uNumPoints; i++) {
			int idx = i * vNumPoints + j;
			glVertex3f(controlPoints[idx][0], controlPoints[idx][1], controlPoints[idx][2]);
		}
		glEnd();
	}

	// Освобождение ресурсов
	gluDeleteNurbsRenderer(nurbsObject);
}

void LR3Na54() {
	// Параметры NURBS поверхности
	int uDegree, vDegree;
	int uNumPoints, vNumPoints;
	std::vector<std::vector<GLfloat>> controlPoints;
	std::vector<std::vector<GLfloat>> weights;

	// Загрузка параметров из файла
	if (LoadNURBSParametersFromFile("nurbs_surface.txt", uDegree, vDegree, uNumPoints, vNumPoints, controlPoints, weights)) {
		// Рисование NURBS поверхности
		DrawNURBSSurface(uDegree, vDegree, uNumPoints, vNumPoints, controlPoints, weights);
	}
	else {
		// В случае ошибки при чтении - использовать примерные данные
		std::cout << "Используются данные по умолчанию для NURBS поверхности" << std::endl;

		// Пример данных
		uDegree = 3;
		vDegree = 3;
		uNumPoints = 4;
		vNumPoints = 4;

		// Инициализация контрольных точек (примерно как в предыдущих примерах)
		controlPoints.resize(16, std::vector<GLfloat>(3, 0.0f));
		weights.resize(16, std::vector<GLfloat>(1, 1.0f));

		// Заполнение контрольных точек сеткой 4x4
		int idx = 0;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				controlPoints[idx][0] = -10.0f + i * 6.6f;
				controlPoints[idx][1] = -10.0f + j * 6.6f;
				controlPoints[idx][2] = (i == 1 || i == 2) && (j == 1 || j == 2) ? 5.0f : 0.0f;
				weights[idx][0] = 1.0f;
				idx++;
			}
		}

		DrawNURBSSurface(uDegree, vDegree, uNumPoints, vNumPoints, controlPoints, weights);
	}
}


//айдишник для текстуры
GLuint texId;
//выполняется один раз перед первым рендером
void initRender()
{
	//==============НАСТРОЙКА ТЕКСТУР================
	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//просим сгенерировать нам Id для текстуры
	//и положить его в texId
	glGenTextures(1, &texId);

	//делаем текущую текстуру активной
	//все, что ниже будет применено texId текстуре.
	glBindTexture(GL_TEXTURE_2D, texId);


	int x, y, n;

	//загружаем картинку
	//см. #include "stb_image.h" 
	unsigned char* data = stbi_load("texture.png", &x, &y, &n, 4);
	//x - ширина изображения
	//y - высота изображения
	//n - количество каналов
	//4 - нужное нам количество каналов
	//пиксели будут хранится в памяти [R-G-B-A]-[R-G-B-A]-[..... 
	// по 4 байта на пиксель - по байту на канал
	//пустые каналы будут равны 255

	//Картинка хранится в памяти перевернутой 
	//так как ее начало в левом верхнем углу
	//по этому мы ее переворачиваем -
	//меняем первую строку с последней,
	//вторую с предпоследней, и.т.д.
	unsigned char* _tmp = new unsigned char[x * 4]; //времянка
	for (int i = 0; i < y / 2; ++i)
	{
		std::memcpy(_tmp, data + i * x * 4, x * 4);//переносим строку i в времянку
		std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4); //(y-1-i)я строка -> iя строка
		std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4); //времянка -> (y-1-i)я строка
	}
	delete[] _tmp;


	//загрузка изображения в видеопамять
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//выгрузка изображения из опперативной памяти
	stbi_image_free(data);


	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
												  //GL_REPLACE -- полная замена политога текстурой
	//настройка тайлинга
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//настройка фильтрации
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//======================================================

	//================НАСТРОЙКА КАМЕРЫ======================
	camera.caclulateCameraPos();

	//привязываем камеру к событиям "движка"
	gl.WheelEvent.reaction(&camera, &Camera::Zoom);
	gl.MouseMovieEvent.reaction(&camera, &Camera::MouseMovie);
	gl.MouseLeaveEvent.reaction(&camera, &Camera::MouseLeave);
	gl.MouseLdownEvent.reaction(&camera, &Camera::MouseStartDrag);
	gl.MouseLupEvent.reaction(&camera, &Camera::MouseStopDrag);
	//==============НАСТРОЙКА СВЕТА===========================
	//привязываем свет к событиям "движка"
	gl.MouseMovieEvent.reaction(&light, &Light::MoveLight);
	gl.KeyDownEvent.reaction(&light, &Light::StartDrug);
	gl.KeyUpEvent.reaction(&light, &Light::StopDrug);
	//========================================================
	//====================Прочее==============================
	gl.KeyDownEvent.reaction(switchModes);
	text.setSize(512, 180);
	//========================================================
	   

	camera.setPosition(2, 1.5, 1.5);
}

void Render(double delta_time)
{    
	glEnable(GL_DEPTH_TEST);
	
	//натройка камеры и света
	//в этих функциях находятся OGLные функции
	//которые устанавливают параметры источника света
	//и моделвью матрицу, связанные с камерой.

	if (gl.isKeyPressed('F')) //если нажата F - свет из камеры
	{
		light.SetPosition(camera.x(), camera.y(), camera.z());
	}
	camera.SetUpCamera();
	light.SetUpLight();


	//рисуем оси
	gl.DrawAxes();

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	

	//включаем режимы, в зависимости от нажания клавиш. см void switchModes(OpenGL *sender, KeyEventArg arg)
	if (lightning)
		glEnable(GL_LIGHTING);
	if (texturing)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0); //сбрасываем текущую текстуру
	}
		
	if (alpha)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
		
	//=============НАСТРОЙКА МАТЕРИАЛА==============


	//настройка материала, все что рисуется ниже будет иметь этот метериал.
	//массивы с настройками материала
	float  amb[] = { 0.2, 0.2, 0.1, 1. };
	float dif[] = { 0.4, 0.65, 0.5, 1. };
	float spec[] = { 0.9, 0.8, 0.3, 1. };
	float sh = 0.2f * 256;

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); 
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH); //закраска по Гуро      
			   //(GL_SMOOTH - плоская закраска)

	//============ РИСОВАТЬ ТУТ ==============

	LR3Na54();

	//===============================================

	//рисуем источник света
	light.DrawLightGizmo();

	//================Сообщение в верхнем левом углу=======================

	//переключаемся на матрицу проекции
	glMatrixMode(GL_PROJECTION);
	//сохраняем текущую матрицу проекции с перспективным преобразованием
	glPushMatrix();
	//загружаем единичную матрицу в матрицу проекции
	glLoadIdentity();

	//устанавливаем матрицу паралельной проекции
	glOrtho(0, gl.getWidth() - 1, 0, gl.getHeight() - 1, 0, 1);

	//переключаемся на моделвью матрицу
	glMatrixMode(GL_MODELVIEW);
	//сохраняем матрицу
	glPushMatrix();
    //сбразываем все трансформации и настройки камеры загрузкой единичной матрицы
	glLoadIdentity();

	//отрисованное тут будет визуалзироватся в 2д системе координат
	//нижний левый угол окна - точка (0,0)
	//верхний правый угол (ширина_окна - 1, высота_окна - 1)

	
	std::wstringstream ss;
	ss << std::fixed << std::setprecision(3);
	ss << "T - " << (texturing ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"текстур" << std::endl;
	ss << "L - " << (lightning ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"освещение" << std::endl;
	ss << "A - " << (alpha ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"альфа-наложение" << std::endl;
	ss << L"F - Свет из камеры" << std::endl;
	ss << L"G - двигать свет по горизонтали" << std::endl;
	ss << L"G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << L"Коорд. света: (" << std::setw(7) <<  light.x() << "," << std::setw(7) << light.y() << "," << std::setw(7) << light.z() << ")" << std::endl;
	ss << L"Коорд. камеры: (" << std::setw(7) << camera.x() << "," << std::setw(7) << camera.y() << "," << std::setw(7) << camera.z() << ")" << std::endl;
	ss << L"Параметры камеры: R=" << std::setw(7) << camera.distance() << ",fi1=" << std::setw(7) << camera.fi1() << ",fi2=" << std::setw(7) << camera.fi2() << std::endl;
	ss << L"delta_time: " << std::setprecision(5)<< delta_time << std::endl;

	
	text.setPosition(10, gl.getHeight() - 10 - 180);
	text.setText(ss.str().c_str());
	text.Draw();

	//восстанавливаем матрицу проекции на перспективу, которую сохраняли ранее.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	

}   



