//file: tools.h
#ifndef TOOLS_H
#define TOOLS_H

//�Կ���̨�����ĺ�������

//��Ҫ���õ�ͷ�ļ�
#include<Windows.h>
#include<iostream>
#include<conio.h>
#include<ctime>

//һ������̨�ľ�������ƿ���̨
HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

//���ߺ��� ���ÿ���̨���ڵĴ�С
//����1 ��ʾ����̨�ĳ� ��Ҫ����0
//����2 ��ʾ����̨�Ŀ� ��Ҫ����0
void tool_set_window_size(short length, short width)
{
	char cmd[64];
	sprintf_s(cmd, "mode con cols=%d lines=%d", width, length);
	system(cmd);
}


//���ߺ��� �ָ�����̨�����������ɫ
//�ڵװ��ֵı�׼��
void tool_set_regular_text_color()
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut, FOREGROUND_INTENSITY | 7);
}


//���ߺ��� �ı����̨����λ��
//����1 ����xλ��
//����2 ����yλ��
void tool_set_cursor_position(short position_x, short position_y)
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	//����ṹ�� ���ڸı����λ��
	COORD coord = { position_x,position_y };
	SetConsoleCursorPosition(hOut, coord);
}


//���ߺ��� �رչ�����ʾ
void tool_close_cursor()
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	//���ò��� �رչ��
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	SetConsoleCursorInfo(hOut,&cursor_info);
}


//���ߺ��� �򿪹�����ʾ
void tool_open_cursor()
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	//���ò����򿪹��
	CONSOLE_CURSOR_INFO cursor_info = { 20, 1 };
	SetConsoleCursorInfo(hOut, &cursor_info);
}


//���ߺ��� ���ÿ���̨�����������ɫ
//����1 ��ʾǰ��ɫ  
//����2 ��ʾ��
//0 = ǳ�� 8 = ��ɫ
//1 = ��ɫ 9 = ����ɫ
//2 = ��ɫ 10 = ����ɫ
//3 = ǳ�� 11 = ��ǳ��ɫ
//4 = ��ɫ 12 = ����ɫ
//5 = ��ɫ 13 = ����ɫ
//6 = ��ɫ 14 = ����ɫ
//7 = ��ɫ 15 = ����ɫ
//ʹ�ú�������ɫ���ֲ���
//���Ҫ�ָ�Ĭ��ֵ 
//��Ҫ���ù��ߺ��� tool_set_regular_text_color()
void tool_set_text_color(int front_color, int back_color)
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut, BACKGROUND_INTENSITY | back_color * 16 | FOREGROUND_INTENSITY | front_color);
}


//���ߺ��� ���ص�ǰ��������
//����ֵ��һ��COORD�ṹ��
//�������� x�Լ�y ���Ե������λ��
COORD tool_get_cursor_now_position()
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	//�õ�����������Ϣ ���ұ��滺������Ϣ
	CONSOLE_SCREEN_BUFFER_INFO   infoBuffer;
	GetConsoleScreenBufferInfo(hOut, &infoBuffer);

	//��������Ϣ��������б���
	COORD now_position = { infoBuffer.dwCursorPosition.X, infoBuffer.dwCursorPosition.Y };

	return now_position;
}

//������ ��ô��࿪ʼ������ʱ��仯
//����һ���࿪ʼ����ʱ��
//��Ա����get_clock()���ص��ǹ�������࿪ʼ������ʱ��
class tool_time
{
public:
	//���캯�� ��ʼ�� start�Լ�end
	tool_time()
	{
		start = clock();
		end = clock();
	}
	
	//���ؾ�����ʱ��
	int get_clock()
	{
		//����end
		end = clock();
		return (end-start) / CLOCKS_PER_SEC;
	}
private:
	int start;//�洢������ʱ��
	int end;//�洢���õ�ʱ��
};
#endif