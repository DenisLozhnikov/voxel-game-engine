#pragma once
#include <queue>

using namespace std;

class Chunks;

struct lightentry {	//свет для применения к координате
	int x;
	int y;
	int z;
	unsigned char light;
};


class LightSolver
{
	queue<lightentry> addqueue;	//очередь на добавление света
	queue<lightentry> remqueue;	//очередь на удаление
	Chunks* chunks;	//для получения карты освещения чанка
	int channel;	//канал rgb

public:
	LightSolver(Chunks* chunks, int channel);

	void add(int x, int y, int z);	//добавление света
	void add(int x, int y, int z, int emission);	//явное указание яркости
	void remove(int x, int y, int z);	//удаление

	void solve();	//работа с очередями
};

