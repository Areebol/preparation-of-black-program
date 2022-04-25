//file: buff.h
#ifndef BUFF_H
#define BUFF_H

#include"control.h"
#include<ctime>
#include<string>
using std::string;
class character;
class control;
class buff_base
{
public:
	//ʹ��buff���� 
	//�����Ƿ�ʹ�óɹ�
	virtual bool use(control& rhs) = 0;
	//��ӡͼ�꺯��
	virtual void print(control& rhs);
	//���ص��ߵ���Ϣ
	virtual string information() = 0;
	//�������꺯��
	void set_coord(int x, int y);
	//�ж��Ƿ�������
	bool operator==(character& rhs);
	//�������ߵķ���
	virtual void occur(control& rhs) = 0;
	//�������λ����Ϣ
	int coord_x;
	int coord_y;


	//չʾ��ʾ�Լ�ѡ��˵�
	//����ʹ�õ���
	virtual void show_select_menu(control& rhs) = 0;
};

//�����--------------------------------------������Ұ
class torch
	:public buff_base
{
public:
	torch(int x, int y);
	bool use(control& rhs);
	string information();
	void occur(control& rhs);
	void show_select_menu(control& rhs);
};

//ˮ������------------------------------------����������Ұ
class crystalBall
	:public buff_base
{
public:
	crystalBall(int x, int y);
	bool use(control& rhs);
	string information();
	//void show_select_menu(control& rhs);
	//void occur(control& rhs);
};

//������--------------------------------------��ǽ
class shovel
	:public buff_base
{
public:
	shovel(int x, int y);
	bool use(control& rhs);
	string information();
	void show_select_menu(control& rhs);
	void occur(control& rhs);
	bool is_wall(control& rhs, int x, int y);
};

//������--------------------------------------����Ѫ��
class moderator
	:public buff_base
{
public:
	moderator(int x, int y);
	bool use(control& rhs);
	string information();
	void show_select_menu(control& rhs);
	void occur(control& rhs);
};

//ʥ��----------------------------------------�ı�����ĵȼ�
class holyLight
	:public buff_base
{
public:
	holyLight(int x, int y);
	bool use(control& rhs);
	string information();
	void show_select_menu(control& rhs);
	void occur(control& rhs);
};
#endif