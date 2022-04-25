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

//������
class teleportationArray
	:public debuff_base
{
public:
	//���ô����������λ�� 
	//������������� ʹ��������ͳ�Ϊ����
	teleportationArray(int x, int y);
	void occur(control& rhs);
	void print(control& rhs);
};

//����Ģ��
class stunMushrooms
	:public debuff_base
{
public:
	stunMushrooms(int x, int y);
	void occur(control& rhs)override;
	void print(control& rhs)override;
};

//�˶���ħ��
class pandoraBox
	:public debuff_base
{
public:
	pandoraBox(int x, int y);
	void occur(control& rhs);
	void print(control& rhs);
};

//˹�ҿ�˹֮��
class sphinxAsk
	:public debuff_base
{
public:
	sphinxAsk(int x, int y);
	void occur(control& rhs);
	void print(control& rhs);
};
#endif
