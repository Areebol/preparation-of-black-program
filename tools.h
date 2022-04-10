//file: tools.h
#ifndef TOOLS_H
#define TOOLS_H

//对控制台操作的函数定义

//需要引用的头文件
#include<Windows.h>
#include<iostream>
#include<conio.h>
#include<ctime>

//一个控制台的句柄来控制控制台
HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

//工具函数 设置控制台窗口的大小
//参数1 表示控制台的长 需要大于0
//参数2 表示控制台的宽 需要大于0
void tool_set_window_size(short length, short width)
{
	char cmd[64];
	sprintf_s(cmd, "mode con cols=%d lines=%d", width, length);
	system(cmd);
}


//工具函数 恢复控制台输出的字体颜色
//黑底白字的标准型
void tool_set_regular_text_color()
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut, FOREGROUND_INTENSITY | 7);
}


//工具函数 改变控制台光标的位置
//参数1 光标的x位置
//参数2 光标的y位置
void tool_set_cursor_position(short position_x, short position_y)
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	//坐标结构体 用于改变光标的位置
	COORD coord = { position_x,position_y };
	SetConsoleCursorPosition(hOut, coord);
}


//工具函数 关闭光标的显示
void tool_close_cursor()
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	//设置参数 关闭光标
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	SetConsoleCursorInfo(hOut,&cursor_info);
}


//工具函数 打开光标的显示
void tool_open_cursor()
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	//设置参数打开光标
	CONSOLE_CURSOR_INFO cursor_info = { 20, 1 };
	SetConsoleCursorInfo(hOut, &cursor_info);
}


//工具函数 设置控制台输出的字体颜色
//参数1 表示前景色  
//参数2 表示后景
//0 = 浅灰 8 = 灰色
//1 = 蓝色 9 = 淡蓝色
//2 = 绿色 10 = 淡绿色
//3 = 浅绿 11 = 淡浅绿色
//4 = 红色 12 = 淡红色
//5 = 紫色 13 = 淡紫色
//6 = 黄色 14 = 淡黄色
//7 = 白色 15 = 亮白色
//使用后字体颜色保持不变
//如果要恢复默认值 
//需要调用工具函数 tool_set_regular_text_color()
void tool_set_text_color(int front_color, int back_color)
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut, BACKGROUND_INTENSITY | back_color * 16 | FOREGROUND_INTENSITY | front_color);
}


//工具函数 返回当前光标的坐标
//返回值是一个COORD结构体
//包含的了 x以及y 可以调整光标位置
COORD tool_get_cursor_now_position()
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	//得到缓冲区的信息 并且保存缓冲区信息
	CONSOLE_SCREEN_BUFFER_INFO   infoBuffer;
	GetConsoleScreenBufferInfo(hOut, &infoBuffer);

	//缓冲区信息的坐标进行保存
	COORD now_position = { infoBuffer.dwCursorPosition.X, infoBuffer.dwCursorPosition.Y };

	return now_position;
}

//工具类 获得从类开始构建的时间变化
//构建一个类开始计算时间
//成员函数get_clock()返回的是构造这个类开始经过的时间
class tool_time
{
public:
	//构造函数 初始化 start以及end
	tool_time()
	{
		start = clock();
		end = clock();
	}
	
	//返回经过的时间
	int get_clock()
	{
		//更新end
		end = clock();
		return (end-start) / CLOCKS_PER_SEC;
	}
private:
	int start;//存储创建的时间
	int end;//存储调用的时间
};
#endif