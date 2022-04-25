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
	//初始化函数 初始化坐标位置
	character(int x = 0, int y = 0);

	//设置物体的坐标------------------------
	void set_coord(int x, int y);

	//对血量的操作函数---------------------
	//加血的函数需要提供边界检查 
	//不能超过一定量
	void blood_add(int x);
	void blood_sub(int x);

	//get函数-------------------------------
	//返回血量的剩余量
	int get_blood()const;
	int get_coord_x()const;
	int get_coord_y()const;
	//返回物体的坐标位置
	COORD get_coord()const;

	//判断函数-------------------------------
	//判断是否死亡
	bool is_dead()const;

	//移动物体函数 move_----------------------
	//1 向上移动
	bool move_up();
	//2 向右移动
	bool move_right();
	//3 向左移动
	bool move_left();
	//4 想下移动
	bool move_down();
private:
	const int blood_max = 5500;
	//表示坐标
	short coord_x;
	short coord_y;
	//表示血量的剩余量
	int blood;
};

#endif