//file: character.cpp
#include"character.h"
#include<stdexcept>
using std::out_of_range;

//初始化函数 初始化坐标位置 
//坐标位置 从0-29
character::character(int x, int y)
{
	set_coord(x, y);
	blood = blood_max;//初始血量10000
}


//设置物体的坐标位置
void character::set_coord(int x, int y)
{
	if (x < 0 || x >= 30 || y < 0 || y >= 30)
		throw out_of_range("character coord is fault");
	coord_x = x;
	coord_y = y;
}


//对血量的操作函数---------------------
//加血的函数需要提供边界检查 
//不能超过一定量
void character::blood_add(int x)
{
	blood += x;
	if (blood > blood_max)
		blood = blood_max;
}

//扣血函数
void character::blood_sub(int x)
{
	blood -= x;
}

//get函数-------------------------------
//返回血量的剩余量
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

//返回物体的坐标位置
COORD character::get_coord()const
{
	COORD coord = { coord_x,coord_y };
	return coord;
}


//判断函数-------------------------------
//判断是否死亡
bool character::is_dead() const
{
	return false;
}

//移动物体函数 move_----------------------
//1 向上移动
//保证不越界
bool character::move_up()
{
	if (coord_y != 0)
		--coord_y;
	else
		return false;
	return true;
}
//2 向右移动
//保证不越界
bool character::move_right()
{
	if (coord_x != 29)
		++coord_x;
	else
		return false;
	return true;
}
//3 向左移动
//保证不越界
bool character::move_left()
{
	if (coord_x != 0)
		--coord_x;
	else
		return false;
	return true;
}
//4 想下移动
//保证不越界
bool character::move_down()
{
	if (coord_y != 29)
		++coord_y;
	else
		return false;
	return true;
}