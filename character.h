//file: character.h
#ifndef CHARACTER_H
#define CHARACTER_H

#include<conio.h>
#include<iostream>
#include<Windows.h>
class control;
class character
{
public:
	//��ʼ������ ��ʼ������λ��
	character(int x = 0, int y = 0);

	//�������������------------------------
	void set_coord(int x, int y);

	//��Ѫ���Ĳ�������---------------------
	//��Ѫ�ĺ�����Ҫ�ṩ�߽��� 
	//���ܳ���һ����
	void blood_add(int x);
	void blood_sub(int x);

	//get����-------------------------------
	//����Ѫ����ʣ����
	int get_blood()const;
	int get_coord_x()const;
	int get_coord_y()const;
	//�������������λ��
	COORD get_coord()const;

	//�жϺ���-------------------------------
	//�ж��Ƿ�����
	bool is_dead()const;

	//�ƶ����庯�� move_----------------------
	//1 �����ƶ�
	bool move_up();
	//2 �����ƶ�
	bool move_right();
	//3 �����ƶ�
	bool move_left();
	//4 �����ƶ�
	bool move_down();
private:
	const int blood_max = 5500;
	//��ʾ����
	short coord_x;
	short coord_y;
	//��ʾѪ����ʣ����
	int blood;
};

#endif