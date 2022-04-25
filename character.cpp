//file: character.cpp
#include"character.h"
#include<stdexcept>
using std::out_of_range;

//��ʼ������ ��ʼ������λ�� 
//����λ�� ��0-29
character::character(int x, int y)
{
	set_coord(x, y);
	blood = blood_max;//��ʼѪ��10000
}


//�������������λ��
void character::set_coord(int x, int y)
{
	if (x < 0 || x >= 30 || y < 0 || y >= 30)
		throw out_of_range("character coord is fault");
	coord_x = x;
	coord_y = y;
}


//��Ѫ���Ĳ�������---------------------
//��Ѫ�ĺ�����Ҫ�ṩ�߽��� 
//���ܳ���һ����
void character::blood_add(int x)
{
	blood += x;
	if (blood > blood_max)
		blood = blood_max;
}

//��Ѫ����
void character::blood_sub(int x)
{
	blood -= x;
}

//get����-------------------------------
//����Ѫ����ʣ����
int character::get_blood() const
{
	return blood;
}

int character::get_coord_x() const
{
	return coord_x;
}

int character::get_coord_y() const
{
	return coord_y;
}

//�������������λ��
COORD character::get_coord()const
{
	COORD coord = { coord_x,coord_y };
	return coord;
}


//�жϺ���-------------------------------
//�ж��Ƿ�����
bool character::is_dead() const
{
	return false;
}

//�ƶ����庯�� move_----------------------
//1 �����ƶ�
//��֤��Խ��
bool character::move_up()
{
	if (coord_y != 0)
		--coord_y;
	else
		return false;
	return true;
}
//2 �����ƶ�
//��֤��Խ��
bool character::move_right()
{
	if (coord_x != 29)
		++coord_x;
	else
		return false;
	return true;
}
//3 �����ƶ�
//��֤��Խ��
bool character::move_left()
{
	if (coord_x != 0)
		--coord_x;
	else
		return false;
	return true;
}
//4 �����ƶ�
//��֤��Խ��
bool character::move_down()
{
	if (coord_y != 29)
		++coord_y;
	else
		return false;
	return true;
}