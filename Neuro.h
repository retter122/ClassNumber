#pragma once
#include <math.h>
// MODEL PARAMS
#define BufferSizeX 512
#define BufferSizeY 512

#define Svrtk1Step 3
#define Svrtk1SizeX (BufferSizeX - 2) / Svrtk1Step
#define Svrtk1SizeY (BufferSizeY - 2) / Svrtk1Step

#define Svrtk2Step 3
#define Svrtk2SizeX (Svrtk1SizeX - 2) / Svrtk2Step
#define Svrtk2SizeY (Svrtk1SizeY - 2) / Svrtk2Step

#define Svrtk3Step 1
#define Svrtk3SizeX (Svrtk2SizeX - 2) / Svrtk3Step
#define Svrtk3SizeY (Svrtk2SizeY - 2) / Svrtk3Step

#define Layer1NeuroCount 20

#define Layer2NeuroCount 10

#define LearnStep 0.0625f;
// LEARNING DATA
static const char DirectoryName[] = ".\\Database";

static const char Dir1Names[10][13] = { ".\\Database\\0", ".\\Database\\1", ".\\Database\\2", ".\\Database\\3", ".\\Database\\4",
										".\\Database\\5", ".\\Database\\6", ".\\Database\\7", ".\\Database\\8", ".\\Database\\9" };

static const char FileNames[10][20] = { ".\\Database\\0\\%d.txt", ".\\Database\\1\\%d.txt", ".\\Database\\2\\%d.txt", ".\\Database\\3\\%d.txt", ".\\Database\\4\\%d.txt",
										".\\Database\\5\\%d.txt", ".\\Database\\6\\%d.txt", ".\\Database\\7\\%d.txt", ".\\Database\\8\\%d.txt", ".\\Database\\9\\%d.txt" };

static unsigned int NumberImages[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned int IterationsImages[10];

static unsigned int BufferEducation[BufferSizeX * BufferSizeY];

static float Validates[10][10] = {
	{1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
	{0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
	{0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
	{0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
	{0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f},
	{0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f},
	{0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f},
	{0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f},
	{0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f},
	{0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f},
};

static FILE* File;
static char RealPath[25];
// SVERTKA DATA
extern float Svrtk1[Svrtk1SizeX * Svrtk1SizeY] = { 0 };
extern float Svrtk1Mat[] = {
							1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
							1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
							1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
};

extern float Svrtk2[Svrtk2SizeX * Svrtk2SizeY] = { 0 };
extern float Svrtk2Mat[] = {
							1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
							1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
							1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
};

extern float Svrtk3[Svrtk3SizeX * Svrtk3SizeY] = { 0 };
extern float Svrtk3Mat[] = {
							1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
							1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
							1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
};
// SVERTKA FUNCTIONS
extern void Svertka(unsigned int* Data);
// MODEL DATA
extern float Layer1Neuro[Layer1NeuroCount][Svrtk3SizeX * Svrtk3SizeY] = { 0.f };
extern float Layer1Container[Layer1NeuroCount] = { 0.f };

static float Layer1Correct[Layer1NeuroCount];
extern float Layer2Neuro[Layer2NeuroCount][Layer1NeuroCount] = { 0.f };
extern float Layer2Container[Layer2NeuroCount] = { 0.f };
// SOLVE MODEL FUNCTIONS
extern float SolveModel(unsigned int* Data);
// MODEL LEARNING FUNCTIONS
static void StepLearning(unsigned int* Data, float Valid[Layer2NeuroCount]) {
	SolveModel(Data);

	float SumCont = 0.f;
	for (unsigned int i = 0; i < Layer1NeuroCount; ++i) {
		Layer1Correct[i] = Layer1Container[i];
		SumCont += Layer1Container[i];
	}
	for (unsigned int i = 0; i < Layer2NeuroCount; ++i) {
		float sum = 0.f;
		float error = Valid[i] - Layer2Container[i];

		for (unsigned int j = 0; j < Layer1NeuroCount; ++j) sum += fabsf(Layer2Neuro[i][j]);

		for (unsigned int j = 0; j < Layer1NeuroCount; ++j) {
			Layer1Correct[j] += (Layer2Neuro[i][j] / sum) * error * LearnStep;
			Layer2Neuro[i][j] += (Layer1Container[j] / SumCont) * error * LearnStep;
		}
	}

	float sum = 0.f;
	for (unsigned int j = 0; j < Svrtk3SizeX * Svrtk3SizeY; ++j) sum += Svrtk3[j];
	for (unsigned int i = 0; i < Layer1NeuroCount; ++i) {
		float error = Layer1Correct[i] - Layer1Container[i];

		for (unsigned int j = 0; j < Svrtk3SizeY * Svrtk3SizeY; ++j) {
			Layer1Neuro[i][j] += (Svrtk3[j] / sum) * error * LearnStep;
		}
	}
}
static void LearnEpocha() {
	for (unsigned int i = 0; i < 10; ++i) IterationsImages[i] = 0;
	unsigned char flg = 1;

	while (flg) {
		flg = 0;
		for (unsigned int i = 0; i < 10; ++i) {
			if (IterationsImages[i] < NumberImages[i]) {
				sprintf(RealPath, FileNames[i], IterationsImages[i]);
				File = fopen(RealPath, "r");
				fread(BufferEducation, 1, BufferSizeX * BufferSizeY, File);
				
				for (unsigned int f = BufferSizeX * BufferSizeY; f; --f) BufferEducation[f - 1] = (((char*)BufferEducation)[f - 1]) - '0';
				StepLearning(BufferEducation, Validates[i]);
				
				fclose(File);

				++IterationsImages[i];
				flg = 1;
			}
		}
	}
}
// MODEL INIT FUNCTION
static float ModelInit() {
	for (unsigned int i = 0; i < Layer1NeuroCount; ++i) {
		for (unsigned int j = 0; j < Svrtk3SizeX * Svrtk3SizeY; ++j) {
			Layer1Neuro[i][j] = (float)((rand() % 1000) - 500) / 1000.f;
		}
	}

	for (unsigned int i = 0; i < Layer2NeuroCount; ++i) {
		for (unsigned int j = 0; j < Layer1NeuroCount; ++j) Layer2Neuro[i][j] = (float)((rand() % 1000) - 500) / 1000.f;
	}
}