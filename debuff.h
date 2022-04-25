//file: debuff.h

#ifndef DEBUFF_H
#define DEBUFF_H

#include"control.h"
#include<ctime>
class character;
class control;
class debuff_base
{
public:
	virtual void occur(control& rhs) = 0;
	bool operator==(character& rhs);
	void set_coord(int x, int y);
	virtual void print(control& rhs) = 0;
	int coord_x;
	int coord_y;
};

//传送阵
class teleportationArray
	:public debuff_base
{
public:
	//设置传送阵的坐标位置 
	//设置随机数种子 使得随机传送成为可能
	teleportationArray(int x, int y);
	void occur(control& rhs);
	void print(control& rhs);
};

//晕晕蘑菇
class stunMushrooms
	:public debuff_base
{
public:
	stunMushrooms(int x, int y);
	void occur(control& rhs)override;
	void print(control& rhs)override;
};

//潘多拉魔盒
class pandoraBox
	:public debuff_base
{
public:
	pandoraBox(int x, int y);
	void occur(control& rhs);
	void print(control& rhs);
};

//斯芬克斯之问
class sphinxAsk
	:public debuff_base
{
public:
	sphinxAsk(int x, int y);
	void occur(control& rhs);
	void print(control& rhs);
};
#endif
