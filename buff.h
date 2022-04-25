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
	//使用buff函数 
	//返回是否使用成功
	virtual bool use(control& rhs) = 0;
	//打印图标函数
	virtual void print(control& rhs);
	//返回道具的信息
	virtual string information() = 0;
	//设置坐标函数
	void set_coord(int x, int y);
	//判断是否能相遇
	bool operator==(character& rhs);
	//遇到道具的反馈
	virtual void occur(control& rhs) = 0;
	//存放坐标位置信息
	int coord_x;
	int coord_y;


	//展示提示以及选择菜单
	//操作使用道具
	virtual void show_select_menu(control& rhs) = 0;
};

//火把类--------------------------------------照亮视野
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

//水晶球类------------------------------------照亮部分视野
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

//铲子类--------------------------------------开墙
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

//缓毒剂--------------------------------------增加血量
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

//圣光----------------------------------------改变迷雾的等级
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