#pragma once
#include <queue>

using namespace std;

class Chunks;

struct lightentry {	//���� ��� ���������� � ����������
	int x;
	int y;
	int z;
	unsigned char light;
};


class LightSolver
{
	queue<lightentry> addqueue;	//������� �� ���������� �����
	queue<lightentry> remqueue;	//������� �� ��������
	Chunks* chunks;	//��� ��������� ����� ��������� �����
	int channel;	//����� rgb

public:
	LightSolver(Chunks* chunks, int channel);

	void add(int x, int y, int z);	//���������� �����
	void add(int x, int y, int z, int emission);	//����� �������� �������
	void remove(int x, int y, int z);	//��������

	void solve();	//������ � ���������
};

