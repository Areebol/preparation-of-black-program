//file: control.cpp

#include"control.h"

using namespace std;

//构造函数 初始化所有信息-------------------------------------------------------------------------------------------------------------------------
control::control()
{
	bufferSwapFlag=true;//选择输出到具体缓冲区的标志为true
	bytes = 0;//用于调用缓冲区函数的参数
	//调整窗口大小--------------------------为 36 * 90 的具体大小
	system("mode con cols=96 lines=36");
	//设置缓冲区信息
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
	HWND hWnd = GetConsoleWindow(); //获取窗口句柄
	LONG_PTR sty = GetWindowLongPtrA(hWnd, GWL_STYLE); //获取窗口样式
	sty = sty & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX; 
	//去除可变化大小,最大,最小化按钮
	SetWindowLongPtrA(hWnd, GWL_STYLE, sty); //设置窗体不可更改大小,不可最大化
	hOutBuf = CreateConsoleScreenBuffer(
		GENERIC_WRITE,//定义进程可以往缓冲区写数据
		FILE_SHARE_WRITE,//定义缓冲区可共享写权限
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	hOutput = CreateConsoleScreenBuffer(
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	//关闭缓冲区的光标----------------------------
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = 0;
	cci.dwSize = 1;
	SetConsoleCursorInfo(hOutput, &cci);
	SetConsoleCursorInfo(hOutBuf, &cci);

	set_player_vision(5);//设置初始视野大小5*5
}

//重设置函数  reset_------------------------------------------------------------------------------------------------------------------------
//1 初始化屏幕数组信息 清空屏幕数组信息为空
void control::reset_screen_data()
{
	//初始化屏幕数据为空
	for (int i = 0; i < interface_height; ++i)
		for (int j = 0; j < interface_width; ++j)
			screen_data[i][j] = ' ';//所有文本信息清空
}

//2 初始化迷雾数组信息 
//清空迷雾数组全部设置为真 全图都为迷雾
	void control::reset_foggy_data()
{
	//将foggy设置为全部为迷雾
	for (int i = 0; i < 30; ++i)
		for (int j = 0; j < 30; ++j)
			foggy[i][j] = true;//全部为迷雾
}

//3 初始化地图信息 全地图为空
void control::reset_map()
{
	//将地图全部清空
	for (int i = 0; i < 30; ++i)
		for (int j = 0; j < 30; ++j)
			map[i][j] = false;//全部为假
}

//4 初始化缓冲区的文本属性  全部重设置为黑底白字
void control::reset_buffer()
{
	WORD color = 0x0F;//选择原始颜色 黑底白字
	COORD coord={0,0};//变量用于调用函数
	HANDLE item;
	if (!bufferSwapFlag)//根据标志量设置缓冲区
		item = hOutBuf;
	else
		item = hOutput;
	for (int i = 0; i < interface_height; ++i)//清空缓冲区的文本属性
	{
		for (int j = 0; j < interface_width; ++j)
		{
			coord.X = j;
			coord.Y = i;
			WriteConsoleOutputAttribute(item, &color, 1, coord, &bytes);//逐个设置为黑底白字
		}
	}
}


//5 初始化缓冲区信息 对整个缓冲区屏幕清空包括文本内容文本属性
void control::reset_screen()
{
	reset_screen_data();//清空屏幕字符
	reset_buffer();//清空文本属性
}


//6 更新人物坐标位置 根据人物现在的坐标更新人物坐标位置player_coord
void control::reset_player_coord()
{
	player_coord.X = player.get_coord().X;
	player_coord.Y = player.get_coord().Y;
}


//7 更新游戏是否结束
void control::reset_game_over_flag()
{
	if (end.X == player_coord.X && end.Y == player_coord.Y)//判断是否到达终点
		winFlag = true;
	else
		winFlag = false;
	if (player.get_blood() < 0)//判断血量是否是负值 死亡则游戏失败
		loseFlag = true;
	else
		loseFlag = false;
}


//8 更新游戏的开始变量 在游戏开始对游戏数据 地图 人物 终点 迷雾 debuff buff 以及bag的数据设置 但不会改变人物的视野大小
void control::reset_game_start()
{
	player.blood_add(100000000);//加血量溢出 有边界机制 可以超标
	set_player_coord(1, 1);//设置玩家坐标开始位置
	set_end(29, 29);//设置终点坐标位置
	reset_game_over_flag();//更新游戏结束标志
	fog_level = 0;//初始化迷雾等级为0
	buff_bag.clear();//清空背包数据
	for (int i = 0; i < buff_vector.size(); ++i)
		delete buff_vector[i];//对于buff类的new指针释放
	for (int i = 0; i < debuff_vector.size(); ++i)
		delete debuff_vector[i];//对于debuff类的new指针释放
	buff_vector.clear();//清空地图上的buff类
	debuff_vector.clear();//清空地图上的debuff类
}

//9 更新游戏的屏幕数据 对一次操作之后 游戏所有数据的更新 并且输出到屏幕上
void control::reset_game_screen()
{
	player.blood_sub(2 * fog_level + 8);//对角色血量的扣除操作
	reset_game_screen_unshow();//打印游戏进行的屏幕数据 
	show_doublebuffer();//切换界面
}

//10 刷新游戏的屏幕数据不输出 对一次操作之后 游戏所有数据更新 输出到屏幕上 不包括血量的减少
void control::reset_game_screen_unshow()
{
	reset_screen();//清空缓冲区信息
	reset_player_coord();//更新玩家位置
	reset_game_over_flag();//更新游戏结束标志
	//判断debuff的相遇
	for (vector<debuff_base*>::const_iterator it = debuff_vector.cbegin(); it != debuff_vector.cend(); ++it)
	{
		if (*(*it) == player)//判断是否相遇
		{
			(*it)->occur(*this);//调用occur函数进行数据更新
			delete (*it);//释放debuff类new指针
			debuff_vector.erase(it);//从地图中清除
			break;
		}
	}
	//判断buff的相遇
	for (vector<buff_base*>::iterator it = buff_vector.begin(); it != buff_vector.end(); ++it)
	{
		if (*(*it) == player)//判断是否相遇
		{
			buff_base* ptr = *it;//调用occur函数进行数据更新
			buff_bag.push_back(ptr);//推入背包之中
			(*it)->occur(*this);//触发效果
			buff_vector.erase(it);//从地图中清除
			break;
		}
	}
	write_map();//绘制地图数据 会覆盖所有在地图区域的内容

	//以下操作会打印在地图区域
	for (vector<debuff_base*>::iterator it = debuff_vector.begin(); it != debuff_vector.end(); ++it)
	{
		(*it)->print(*this);//绘制debuff在地图上的显示
	}
	for (vector<buff_base*>::iterator it = buff_vector.begin(); it != buff_vector.end(); ++it)
	{
		(*it)->print(*this);//绘制buff在地图上的显示
	}
	write_player();//绘制玩家坐标位置
	write_end();//绘制终点位置

	//以下操作改变界面部分的屏幕数据
	write_interface();//绘制界面 不影响地图区域以及提示栏区域
	write_player_blood();//绘制玩家血条
	write_menu_guide();//绘制游戏提示栏

	//以下部分用迷雾覆盖地图
	reset_foggy_data();//重新全图都有迷雾
	player_light_foggy();//根据玩家视野来清除部分迷雾
	foggy_light_end(3);//根据终点视野来清除部分迷雾
	write_foggy();//绘制迷雾覆盖游戏地图
}

//打印函数 将缓冲区信息输出到屏幕 show_-----------------------------------------------------------------------------------------------------
//1 利用双缓冲打印数据的函数 需要将缓冲区输出到屏幕 直接调用就可以
void control::show_doublebuffer()
{
	COORD coord = { 0,0 };//变量用于设置数据更新屏幕
	coord.X = 0;
	//切换标志为假的时候 此时输出缓冲区为hOutput
	//写入操作应该对缓冲区hOutBuf
	if (bufferSwapFlag == false)
	{
		bufferSwapFlag = true;//更新标志
		for (int i = 0; i < interface_height;++i)
		{
			coord.Y = i;
			WriteConsoleOutputCharacterA(hOutBuf, screen_data[i], interface_width, coord, &bytes);
		}
		SetConsoleActiveScreenBuffer(hOutBuf);//更改活动缓冲区
	}
	//切换标志为假的时候 此时输出缓冲区为hOutBuf
	//写入操作应该对缓冲区hOutput
	else
	{
		bufferSwapFlag = false; // 更新标志
		for (int i = 0; i < interface_height;++i)
		{
			coord.Y = i;
			WriteConsoleOutputCharacterA(hOutput, screen_data[i], interface_width, coord, &bytes);
		}
		SetConsoleActiveScreenBuffer(hOutput);//更改活动缓冲区
	}
}


//写入数据到缓冲区之中的函数 ouput_buffer_----------------------------------------------------------------
//x是x坐标 y是y坐标 color是写入的颜色 ch是写入的字符
//对于color的使用 使用WORD类型 由两个十六进制数字指定：
//第一个为背景，第二个则为前景
//0 = 黑色		8 = 灰色
//1 = 蓝色		9 = 淡蓝色
//2 = 绿色		A = 淡绿色
//3 = 湖蓝色	B = 淡浅绿色
//4 = 红色		C = 淡红色
//5 = 紫色		D = 淡紫色
//6 = 黄色		E = 淡黄色
//7 = 白色		F = 亮白色
void control::output_buffer(short x, short y, WORD color, char ch)
{
	if (x < 0 || x >= 96 || y < 0 || y >= 36)//数据超出界面 忽略的原因是有很多操作会溢出
		return;
	COORD coord = { x,y };
	//切换标志为假的时候 此时输出缓冲区为hOutput
	//写入操作应该对缓冲区hOutBuf
	if (bufferSwapFlag == false)
	{
		WriteConsoleOutputAttribute(hOutBuf, &color, 1, coord, &bytes);//写入到幕后缓冲区
	}
	//切换标志为假的时候 此时输出缓冲区为hOutBuf
	//写入操作应该对缓冲区hOutput
	else
	{
		WriteConsoleOutputAttribute(hOutput, &color, 1, coord, &bytes);//写入到幕后缓冲区
	}
	screen_data[y][x] = ch;//将对应字符写入到屏幕字符数组 在切换缓冲区的时候会输入到缓冲区之中
}



//设置类内数据的函数 set_---------------------------------------------------------------------------------
//1 设置地图信息
//传入一个30*30的bool数组
void control::set_map(bool rhs[30][30])
{
	for (int i = 0; i < 30; ++i)//拷贝地图
		for (int j = 0; j < 30; ++j)
			map[i][j] = rhs[i][j];//拷贝地图
}


//2 设置迷雾信息
//将x y 设置sign true 为有迷雾
void control::set_foggy(int x, int y, bool sign)
{
	if (x < 0 || x >= 30 || y < 0 || y >= 30)//越界 不做操作 
		return;//选择return忽视的原因： 在其它照亮迷雾的操作中很多都会越界
	foggy[y][x] = sign;//设置为sign
}


//3 设置终点位置 
//按照30*30 范围从0—29 
//传入一个坐标将终点设置
//提供边界检查
void control::set_end(int x, int y)
{
	if (x < 0 || y < 0 || x >= 30 || y >= 30)
		throw out_of_range("set of end is fault");//设置出异常
	end.X = x;//成功设置
	end.Y = y;
}


//4 设置游戏玩家视野大小
//设置为 a*a的大小
void control::set_player_vision(int a)
{
	if (a <= 1)//最小视野 抛出异常
		throw out_of_range("vision can not be less than 0");
	player_vision_l = (a - 1) / 2;//对应的视野为 (player_vision_l*2+1)*（player_vision_l*2+1)
}

//5 设置游戏玩家的位置
//同时更新玩家位置
//按照地图30*30 
//玩家位置 范围 0-29
//提供边界检查
void control::set_player_coord(int x, int y)
{
	if (x < 0 || y < 0 || x >= 30 || y >= 30)
		throw out_of_range("player's coord is fault");//设置出错 抛出异常
	player.set_coord(x, y);//成功设置 提供边界检查 不会设置在地图有墙的位置
	reset_player_coord();//更改数据
}

//6 设置迷雾的等级
//level true 设置为红色迷雾
//level false 设置为白色迷雾
void control::set_foggy_level(bool level)
{
	fog_level = level;//设置为对应level
}

//对于buff类 以及debuff 类 为了方便添加 特加上注释
//方便添加 需要注意的是 所有的类都需要使用new指针
//举例： 在 3 4 坐标添加 debuff中的 sphinxAsk
//debuff_base* de_number=new sphinxAsk（3，4）
//debuff_vector.push_back(de_number);
//这样就可以成功添加
//后续在堆区的内存释放都有安全性的代码释放 无需考虑
//！！！特别注意 插入不提供边界检查 
//如果不小心放置在地图墙壁上 将会发生异常

//以下为debuff 以及 buff的具体名称
//debuff: 
//1 传送阵 teleportationArray 
//2 晕晕蘑菇 stunMushrooms
//3 潘多拉魔盒 pandoraBox
//4 斯芬克斯之问 sphinxAsk
//buff:
//1 火把 torch
//2 铲子 shovel
//3 解毒剂 moderator
//4 圣光 holyLight

//7 设置游戏为关卡1
void control::set_game_level_1()
{
	reset_game_start();//先重新设置游戏数据
	//------------------------------------ 对debuff的放置
	debuff_base* de_1 =new teleportationArray(10, 11);
	debuff_base* de_2 = new stunMushrooms(12, 13);
	debuff_base* de_3 = new pandoraBox(23, 18);
	debuff_base* de_4 = new sphinxAsk(29, 26);
	debuff_vector.push_back(de_1);
	debuff_vector.push_back(de_2);
	debuff_vector.push_back(de_3);
	debuff_vector.push_back(de_4);


	//------------------------------------ 对buff的放置
	buff_base* buf_1 =new torch(19, 13);
	buff_base* buf_2 = new shovel(10, 7);
	buff_base* buf_3 = new shovel(3, 10);
	buff_base* buf_4 = new moderator(13, 15);
	buff_base* buf_5 = new holyLight(19, 23);
	buff_vector.push_back(buf_1);
	buff_vector.push_back(buf_2);
	buff_vector.push_back(buf_3);
	buff_vector.push_back(buf_4);
	buff_vector.push_back(buf_5);
	set_map(map1);//设置地图为1
}

//8 设置游戏为关卡2
void control::set_game_level_2()
{
	reset_game_start();//先重新设置游戏数据
	//------------------------------------ 对debuff的放置



	//------------------------------------ 对buff的放置

	set_map(map2);//设置地图为2
}

//9 设置游戏为关卡3
void control::set_game_level_3()
{
	reset_game_start();//先重新设置游戏数据
	//------------------------------------ 对debuff的放置



	//------------------------------------ 对buff的放置

	set_map(map3);//设置地图为3
}


//10 设置地图x y坐标位置的数据为 sign
void control::set_map_dot(int x, int y,bool sign)
{
	if (x < 0 || y < 0 || x>29 || y>29)
		throw out_of_range("out of range");
	else
		map[y][x] = sign;

}


//写入数据到屏幕数据的函数 write_--------------------------------------------------------------------------
//1 绘制句子到旁栏 指定在第line行 指定sentence 指定color
void control::write_tip_sentence(int line,string sentence,WORD color)
{
	if (line < 0 || line >= tooltip_height || sentence.size()>(size_t)tooltip_width)//越界 抛出异常
		throw out_of_range("sentence is too long to print or line is unexcepted!");
	size_t x = (size_t)map_width+4+((size_t)tooltip_width-sentence.size())/2;//居中打印
	size_t y = line + 3;
	for (size_t i = 0; i < sentence.size(); ++i,++x)
	{
		output_buffer(x, y, color, sentence[i]);//利用循环输出语句
	}
}


//2 绘制地图 根据地图数据绘制地图在屏幕上 会覆盖地图区域上所有的显示
void control::write_map()
{
	WORD color_wall = 0x88;//墙的颜色
	WORD color_road = 0x00;//通路的颜色
	for(int i=0;i<30;++i)//将地图打印出来
		for (int j = 0; j < 30; ++j)
		{
			int x = 2 * j + 2;//坐标变换
			int y = i + 3;//坐标变换
			if (map[i][j])//此处是墙壁 
			{
				output_buffer(x, y, color_wall, ' ');
				output_buffer(x+1, y, color_wall, ' ');
			}
			else//此处是通路
			{
				output_buffer(x, y, color_road, ' ');
				output_buffer(x + 1, y, color_road, ' ');
			}
		}
}


//3 将提示栏的清空 将提示栏区域的所有内容清空
void control::write_tip_clear()
{
	WORD color = 0x00;//设置为空颜色
	string sentence = "                          ";
	for(int i=0;i<map_height;++i)
	write_tip_sentence(i, sentence, color);//清空提示栏的信息
}


//4 绘制无背景的开始 菜单栏函数 在提示栏
void control::write_menu_start()
{
	write_tip_clear();//先清空
	WORD color = 0x06;
	string sentence = "START MENU";
	write_tip_sentence(6, sentence, color);
	sentence = "PLAY GAME";
	write_tip_sentence(12, sentence, color);
	sentence = "QUIT GAME";
	write_tip_sentence(15, sentence, color);
}


//5 绘制无背景的选择游戏 难度 菜单栏函数 在提示栏
void control::write_menu_select()
{
	write_tip_clear();//先清空
	WORD color = 0x06;
	string sentence = "SELECT MENU";
	write_tip_sentence(6, sentence, color);
	sentence = "SELECT LEVEL";
	write_tip_sentence(12, sentence, color);
	sentence = "SET VISION";
	write_tip_sentence(15, sentence, color);
	sentence = "RETURN";
	write_tip_sentence(18, sentence, color);
}

//6 绘制无背景的选择游戏 难度 菜单栏函数 在提示栏
void control::write_menu_select_level()
{
	write_tip_clear();//先清空
	WORD color = 0x06;
	string sentence = "SELECT LEVEL";
	write_tip_sentence(6, sentence, color);
	sentence = "LEVEL   1";
	write_tip_sentence(12, sentence, color);
	sentence = "LEVEL   2";
	write_tip_sentence(15, sentence, color);
	sentence = "LEVEL   3";
	write_tip_sentence(18, sentence, color);
	sentence = "RETURN";
	write_tip_sentence(21, sentence, color);
}


//7 绘制无背景的选择游戏 选择视野 菜单栏函数 在提示栏
void control::write_menu_select_vision()
{
	write_tip_clear();//先清空
	WORD color = 0x06;
	string sentence = "SELECT YOUR VISION";
	write_tip_sentence(6, sentence, color);
	sentence = "VISION 3X3";
	write_tip_sentence(12, sentence, color);
	sentence = "VISION 5X5";
	write_tip_sentence(15, sentence, color);
	sentence = "VISION 7X7";
	write_tip_sentence(18, sentence, color);
	sentence = "RETURN";
	write_tip_sentence(21, sentence, color);
}

//8 绘制游戏进行的提示 菜单栏函数 在提示栏
void control::write_menu_guide()
{
	write_tip_clear();//先清空
	WORD color = 0x06;
	string sentence = "GUIDE";
	write_tip_sentence(6, sentence, color);
	sentence = "USE ← ↑ → ↓";
	write_tip_sentence(12, sentence, color);
	sentence = "MOVE";
	write_tip_sentence(13, sentence, color);
	sentence = "USE ESC";
	write_tip_sentence(17, sentence, color);
	sentence = "STOP";
	write_tip_sentence(18, sentence, color);
	sentence = "USE B";
	write_tip_sentence(21, sentence, color);
	sentence = "OPEN YOUR BAG";
	write_tip_sentence(22, sentence, color);
}


//9 绘制迷雾 根据迷雾信息覆盖地图区域的内容
void control::write_foggy()
{
	WORD color_blind;
	if (fog_level == 0)//根据迷雾的等级给迷雾改变颜色
		color_blind = 0x07;//白色
	else
		color_blind = 0x04;//红色
	for (int i = 0; i < 30; ++i)
		for (int j = 0; j < 30; ++j)
		{
			if (foggy[i][j])
			{
				int x = 2 * j + 2;
				int y = i + 3;//坐标变换
				output_buffer(x, y, color_blind, '*');//打印迷雾
				output_buffer(x + 1, y, color_blind, '*');
			}
		}
}


//10 绘制暂停菜单 在提示栏
void control::write_menu_pause()
{
	write_tip_clear();//先清空
	WORD color = 0x06;
	string sentence = "PAUSE";
	write_tip_sentence(6, sentence, color);
	sentence = "CONTINUE";
	write_tip_sentence(12, sentence, color);
	sentence = "RETURN";
	write_tip_sentence(15, sentence, color);
	sentence = "QUIT GAME";
	write_tip_sentence(18, sentence, color);
}


//11 绘制玩家 根据玩家坐标位置绘制玩家图像
void control::write_player()
{
	WORD color = 0x03;
	output_buffer(2*player_coord.X + 2, player_coord.Y + 3, color, '<');
	output_buffer(2*player_coord.X + 3, player_coord.Y + 3, color, '>');
}

//12 绘制游戏胜利菜单  在提示栏
void control::write_menu_win()
{
	write_tip_clear();//先清空
	WORD color = 0x06;
	string sentence = "YOU WIN";
	write_tip_sentence(6, sentence, color);
	sentence = "CONTINUE PLAY";
	write_tip_sentence(12, sentence, color);
	sentence = "QUIT GAME";
	write_tip_sentence(15, sentence, color);
}


//13 绘制终点坐标位置 根据终点位置绘制终点图像
void control::write_end()
{
	WORD color = 0x44;//全部为红色
	output_buffer(2 * end.X+2, end.Y + 3, color, ' ');
	output_buffer(2 * end.X+3, end.Y + 3, color, ' ');

}

//14 绘制界面边框函数
void control::write_interface()
{
	WORD text_color_regular = 0x0F;//文本的颜色
	WORD text_color1 = 0x22;//绿色
	for (int i = 0; i < map_width; ++i)//绘制地图上下颜色条
	{
		output_buffer(i + 2, 1, text_color1, ' ');
		output_buffer(i + 2, interface_height - 2, text_color1, ' ');
	}
	for (int i = 0; i < tooltip_width; ++i)//绘制提示栏上下颜色条
	{
		output_buffer(i + 4 + map_width, 1, text_color1, ' ');
		output_buffer(i + 4 + map_width, interface_height - 2, text_color1, ' ');
	}
	for (int i = 0; i < interface_width; ++i)//绘制边界上下的宽
	{
		output_buffer(i, 0, text_color_regular, 'H');
		output_buffer(i, 2, text_color_regular, 'H');
		output_buffer(i, interface_height - 3, text_color_regular, 'H');
		output_buffer(i, interface_height - 1, text_color_regular, 'H');
	}
	for (int i = 0; i < interface_height; ++i)//绘制边界左右的高
	{
		output_buffer(0, i, text_color_regular, 'H');
		output_buffer(1, i, text_color_regular, 'H');
		output_buffer(2 + map_width, i, text_color_regular, 'H');
		output_buffer(3 + map_width, i, text_color_regular, 'H');
		output_buffer(interface_width - 1, i, text_color_regular, 'H');
		output_buffer(interface_width - 2, i, text_color_regular, 'H');
	}
}


//15 在地图区域输出文本内容 指定在第line行 句子为sentence 颜色为color
void control::write_map_sentence(int line, string sentence, WORD color)
{
	if (line < 0 || line >= map_height || sentence.size()>(size_t)map_width)//越界 抛出异常
		throw out_of_range("sentence is too long to print or line is unexcepted!");
	int x = 2 + (map_width - sentence.size()) / 2;//居中打印
	int y = line + 3;//坐标变换
	for (size_t i = 0; i < sentence.size(); ++i, ++x)
	{
		output_buffer(x, y, color, sentence[i]);//循环打印句子
	}
}

//16 清空地图区域的文本内容 将地图区域所有内容清空
void control::write_map_clear()
{
	WORD color = 0x00;//文本无颜色属性
	string sentence = "                                                            ";
	for (int i = 0; i < map_height; ++i)
		write_map_sentence(i, sentence, color);//清空地图区域的内容
}


//17 在地图区域输出游戏的游戏开始选择菜单的时候的 提示文本内容 在地图区域 
void control::write_map_guide()
{
	WORD color0 = 0x03;
	WORD color1 = 0x06;
	WORD color2 = 0x04;
	WORD color3 = 0x02;
	string sentence = "SURVIVE IN THE FOG";
	write_map_sentence(3, sentence, color0);
	sentence = "Welcome to the Fog Survival Game";
	write_map_sentence(6, sentence, color3);

	sentence = "You are unfortunate to enter a foggy world ";
	write_map_sentence(10, sentence, color1);
	sentence = "Where dangerand opportunity coexist         ";
	write_map_sentence(11, sentence, color1);

	sentence = "The fog is full of unknown toxicity         ";
	write_map_sentence(15, sentence, color1);
	sentence = "Corroding your soul all the time            ";
	write_map_sentence(16, sentence, color1);

	sentence = "We hope you are brave enough                ";
	write_map_sentence(20, sentence, color1);
	sentence = "To survive in this world                    ";
	write_map_sentence(21, sentence, color1);

	sentence = "Or under the erosion of the poisonous fog   ";
	write_map_sentence(24, sentence, color2);
	sentence = "Then DIED...                                ";
	write_map_sentence(25, sentence, color2);
}


//18 绘制玩家血条 根据血条数据在界面区域绘制血条长度
void control::write_player_blood()
{
	WORD color_text = 0x03;//blood的颜色
	WORD color_blood = 0x44;//血量的颜色
	WORD color_empty = 0x00;//无颜色
	string sentence = "BLOOD";
	for (int i = 0; i < map_width; ++i)//重新绘制颜色条
	{
		output_buffer(i + 2, 1, color_empty, ' ');//将颜色条替换为空白条
	}
	for (int i = 0; i < sentence.size(); ++i)
		output_buffer(i + 2, 1, color_text, sentence[i]);//绘制blood的提示

	for (int i = 0; i < player.get_blood()/100; ++i)//绘制地图上下颜色条
	{
		output_buffer(i + 2+sentence.size(), 1, color_blood, ' ');//绘制血条的颜色长度
	}
}


//19 绘制游戏失败菜单 在提示栏
void control::write_menu_lose()
{
	write_tip_clear();//先清空
	WORD color = 0x06;
	string sentence = "YOU LOSE";
	write_tip_sentence(6, sentence, color);
	sentence = "CONTINUE PLAY";
	write_tip_sentence(12, sentence, color);
	sentence = "QUIT GAME";
	write_tip_sentence(15, sentence, color);
}

//20 绘制游戏失败菜单 在提示栏
void control::write_map_dead()
{
	WORD color0 = 0x04;
	string sentence = "YOU DIED...";
	write_map_sentence(14, sentence, color0);//地图区域显示
}


//21 在提示栏绘制背包面板选择菜单 在提示栏
void control::write_menu_bag()
{
	write_tip_clear();//先清空提示栏数据
	WORD color0 = 0x06;
	WORD color1 = 0x04;
	WORD color2 = 0x09;
	string sentence = "YOUR BAG";
	write_tip_sentence(6, sentence, color1);
	sentence = "ENTER ESC RETURN";
	write_tip_sentence(8, sentence, color0);
	if (buff_bag.empty())//如果背包为空 挑衅游戏玩家
	{
		sentence = "YOUR BAG IS EMPTY";
		write_tip_sentence(14, sentence, color1);
		sentence = "CLOWN!";
		write_tip_sentence(16, sentence, color1);
		return;
	}
	int size = buff_bag.size();//根据bag的数量 
	for (int i = 0; i < size; ++i)
	{
		sentence = buff_bag[i]->information();
		write_tip_sentence(12+2*i, sentence, color2);//在提示栏打印每一个道具的形状
	}
}





//播放动画的函数 play_----------------------------------------------------------------------------------
//1 播放开始动画函数
void control::play_start()
{
	int frame1 = 0;//用于播放第一段动画
	int frame2 = 0;//用于播放第二段动画
	int frame3 = 0;//用于播放第三段动画
	COORD initial_pos1 = { 0,16 };
	COORD initial_pos2 = { 46,0 };
	COORD initial_pos3 = { 23,14 };
	COORD initial_pos4 = { 23,13 };
	while (frame1 <= 40)
	{
		//横向打印
		for (int i = 0; i < 10 + frame1; i++)
		{
			if (initial_pos1.X + i < interface_width)
			{
				for (int j = -12; j <= 12; j += 3)
					output_buffer(initial_pos1.X + i, initial_pos1.Y + j, 4, '#');

			}
		}
		for (int i = 0; i < 10 + frame1; i++)
		{
			if (initial_pos1.X + i < interface_width)
			{
				for (int j = -12; j <= 12; j += 3)
					output_buffer(interface_width - initial_pos1.X - i, initial_pos1.Y + j, 4, '#');
			}
		}
		show_doublebuffer();//切换界面后重新写入
		reset_screen();
		Sleep(6);
		for (int i = 0; i < frame1; i++)
		{
			if (initial_pos2.Y + i < interface_height)
			{
				for (int j = -35; j <= 35; j += 5)
					output_buffer(initial_pos2.X + j, initial_pos2.Y + i, 4, '#');
			}
		}
		for (int i = 0; i < frame1; i++)
		{
			if (initial_pos2.Y + i < interface_height)
			{
				for (int j = -35; j <= 35; j += 5)
					output_buffer(initial_pos2.X + j, interface_height - initial_pos2.Y - i, 4, '#');
			}
		}
		frame1++;
	}
	reset_screen();//清空屏幕
	string sentence1[5] = { "******  ******  ******  ******  ******  ******",
							"**      **      **      **  **  **  **  **    ",
							"******  ******  **      ******  ******  ******",
							"**          **  **      **  **  **      **    ",
							"******  ******  ******  **  **  **      ******" };
	for (int i = 0; i < 5; i++)
	{
		for (size_t j = 0; j < sentence1[i].size(); j++)
		{
			output_buffer(initial_pos3.X + j, initial_pos3.Y + i, 4, sentence1[i][j]);
		}
	}
	show_doublebuffer();
	reset_screen();//清空屏幕
	for (int i = 0; i < 5; i++)
	{
		for (size_t j = 0; j < sentence1[i].size(); j++)
		{
			output_buffer(initial_pos3.X + j, initial_pos3.Y + i, 5, sentence1[i][j]);
		}
	}
	while (frame2 <= 5)
	{
		show_doublebuffer();
		Sleep(300);
		frame2++;
	}
	reset_screen();//清空屏幕
	string sentence2[8] = { "********    **    **    ********    **   **   **",
							"**    **    **    **    ********    **   **   **",
							"**    **    **    **    **    **    **   **   **",
							"********    **    **    **    **    **   **   **",
							"****        **    **    **    **    **   **   **",
							"*  **       **    **    **    **    **   **   **",
							"*   **      ********    **    **                ",
							"*    ***    ********    **    **    **   **   **"
	};
	for (int i = 0; i < 8; i++)
	{
		for (size_t j = 0; j < sentence2[i].size(); j++)
		{
			output_buffer(initial_pos4.X + j, initial_pos4.Y + i, 4, sentence2[i][j]);
		}
	}
	show_doublebuffer();
	reset_screen();//清空屏幕
	for (int i = 0; i < 8; i++)
	{
		for (size_t j = 0; j < sentence2[i].size(); j++)
		{
			output_buffer(initial_pos4.X + j, initial_pos4.Y + i, 9, sentence2[i][j]);
		}
	}
	while (frame3 <= 15)
	{
		show_doublebuffer();
		Sleep(100);
		frame3++;
	}
}


//2 播放结束动画函数
void control::play_end()
{
	//空函数体
}

//整个游戏操作函数 game_--------------------------------------------------------------------------------------------------------------------
//1 主体函数
void control::game()
{
	play_start();//播放开始动画
	game_start();//开始进入游戏
}

//2 开始菜单选择操作函数 
void control::game_start()
{
	reset_screen();//清空屏幕数据
	int sign = 0;
	WORD color = 0x00;//选择highlight的颜色
	write_menu_start();//显示提示栏菜单
	string sentence = "PLAY GAME";
	write_tip_sentence(12, sentence, color);//highlight一下
	write_interface();//输入界面
	write_map_guide();//输入地图栏提示
	show_doublebuffer();//切换界面

	//设置另一个缓冲区形成闪烁效果
	reset_screen();
	write_map_guide();//输入地图栏提示
	write_menu_start();
	write_interface();
	while (true)//循环等待选择
	{
		show_doublebuffer();//切换界面行程动画
		if (_kbhit())//判断是否有键入
		{
			int key = _getch();
			if (key == 13)//输入为回车
			{
				if (!sign)//选择进入游戏
				{
					game_select();//进入选择菜单
				}
				else//退出游戏
				{
					//game_end();//进入游戏结束环节
					exit(1);//并退出
				}
			}
			if (key == 0xE0)//防止其他键的干扰
			{
				key = _getch();
				if (key == 72 || key == 80)//键入是方向键
				{//切换选择
					if (sign == 1)//选择为进入游戏
					{
						sign = 0;
						write_menu_start();
						sentence = "PLAY GAME";
						write_tip_sentence(12, sentence, color);//hightlight
					}
					else//选择为退出游戏
					{
						sign = 1;
						write_menu_start();
						sentence = "QUIT GAME";
						write_tip_sentence(15, sentence, color);//hightlight
					}
					show_doublebuffer();//切换屏幕
					write_menu_start();//重新设置另一个缓冲区
				}
			}
			Sleep(200);
		}
		Sleep(300);
	}
}

//3 选择菜单操作函数
void control::game_select()
{
	int sign = 0;//选择标志
	WORD color = 0x00;//highlight的颜色标志
	reset_screen();//清空屏幕
	write_menu_select();//在提示栏显示选择菜单
	string sentence = "SELECT LEVEL";//highlight
	write_tip_sentence(12, sentence, color);
	write_interface();//绘制界面
	show_doublebuffer();//切屏
	//设置另一个缓冲区形成闪烁
	reset_screen();//清空屏幕
	write_menu_select();//绘制选择菜单
	write_interface();//绘制界面
	//!!!可以在这里添加在地图区域显示提示
	while (true)
	{
		show_doublebuffer();//切换屏幕
		Sleep(300);
		if (_kbhit())
		{
			int key = _getch();
			if (key == 13)
			{
				if (sign==0)//进入关卡选择菜单
				{
					game_select_map();
				}
				else if(sign==1)//进入视野选择菜单
				{
					game_select_vision();
				}
				else//返回上一级 主菜单
				{
					game_start();
				}
			}
			if (key == 0xE0)//防止其他键的干扰
			{
				key = _getch();
				if (key == 72 || key == 80)
				{
					if (key == 72)
						sign = (sign + 2) % 3;//向上移动
					else
						sign = (sign + 1) % 3;//向下移动

					switch (sign)
					{
					case 0://选择为选择关卡
						write_menu_select();
						sentence = "SELECT LEVEL";
						write_tip_sentence(12, sentence, color);//hightlight
						show_doublebuffer();

						write_menu_select();//设置另一个缓冲区
						break;
					case 1://选择为选择难度
						write_menu_select();
						sentence = "SET VISION";
						write_tip_sentence(15, sentence, color);//hightlight
						show_doublebuffer();

						write_menu_select();//设置另一个缓冲区
						break;
					case 2://选择为选择返回
						write_menu_select();
						sentence = "RETURN";
						write_tip_sentence(18, sentence, color);//hightlight
						show_doublebuffer();

						write_menu_select();//设置另一个缓冲区
						break;
					}
					Sleep(200);//防止连按
				}
			}
		}
	}
}

//4 选择关卡操作菜单函数
void control::game_select_map()
{
	int sign = 0;
	WORD color = 0x00;
	//设置输出缓冲区
	reset_screen();
	write_interface();
	write_menu_select_level();
	string sentence = "LEVEL   1";//hightlight
	write_tip_sentence(12, sentence, color);
	show_doublebuffer();
	//设置另一个缓冲区形成闪烁
	reset_screen();
	write_menu_select_level();
	write_interface();
	//！！！可以添加地图提示区域
	while (true)
	{
		show_doublebuffer();
		Sleep(300);
		if (_kbhit())
		{
			int key = _getch();
			if (key == 13)
			{
				if (sign == 3)//选择返回上一级菜单
					game_select();
				else
				{
					if (sign == 0)//进入关卡1
					{
						set_game_level_1();
					}
					else if (sign == 1)//进入关卡2
					{
						set_game_level_2();
					}
					else if (sign == 2)//进入关卡3
					{
						set_game_level_3();
					}
					game_control();//进入游戏控制环节
				}
			}
			if (key == 0xE0)//防止其他键的干扰
			{
				key = _getch();
				if (key == 72 || key == 80)
				{
					if (key == 72)
						sign = (sign + 3) % 4;//向上移动
					else
						sign = (sign + 1) % 4;//向下移动
					write_menu_select_level();
					switch (sign)
					{
					case 0://选择关卡1
						sentence = "LEVEL   1";
						write_tip_sentence(12, sentence, color);//hightlight
						break;
					case 1://选择关卡2
						sentence = "LEVEL   2";
						write_tip_sentence(15, sentence, color);//hightlight
						break;
					case 2://选择关卡3
						sentence = "LEVEL   3";
						write_tip_sentence(18, sentence, color);//hightlight
						break;
					case 3://选择关卡4
						sentence = "RETURN";
						write_tip_sentence(21, sentence, color);//highlight
						break;
					}
					show_doublebuffer();
					write_menu_select_level();//设置另一个缓冲区
				}
			}
			Sleep(200);//防止连按
		}
	}
}

//5 选择难度操作函数
void control::game_select_vision()
{
	int sign = 0;
	WORD color = 0x00;
	write_map_clear();//无需优化 为了可以形成闪烁
	reset_foggy_data();//重新设置迷雾
	write_menu_select_vision();
	foggy_light_around(4);//照亮四周区域
	write_interface();
	write_foggy();
	show_doublebuffer();
	//设置另一个缓冲区形成闪烁

	write_map_clear();
	reset_foggy_data();//重新设置迷雾
	write_menu_select_vision();
	foggy_light_around(4);//照亮四周区域
	foggy_light_for_show_center(1);//调用函数照亮中心区域
	string sentence = "VISION 3X3";
	write_tip_sentence(12, sentence, color);
	write_interface();
	write_foggy();
	while (true)
	{
		show_doublebuffer();
		Sleep(300);
		if (_kbhit())
		{
			int key = _getch();
			if (key == 13)
			{
				if (sign == 0)//设置难度1 返回游戏选择菜单
					set_player_vision(3);
				else if (sign == 1)//设置难度2 返回游戏选择菜单
					set_player_vision(5);
				else if (sign == 2)//设置难度3 返回游戏选择菜单
					set_player_vision(7);
				game_select();//返回游戏选择菜单
			}
			if (key == 0xE0)//防止其他键的干扰
			{
				key = _getch();
				if (key == 72 || key == 80)
				{
					if (key == 72)
						sign = (sign + 3) % 4;//向上移动
					else
						sign = (sign + 1) % 4;//向下移动
					reset_foggy_data();
					write_map_clear();
					write_interface();
					write_menu_select_vision();
					foggy_light_around(4);
					switch (sign)
					{
					case 0:
						foggy_light_for_show_center(1);
						write_foggy();
						sentence = "VISION 3X3";
						write_tip_sentence(12, sentence, color);//highlight
						show_doublebuffer();
						break;
					case 1:
						foggy_light_for_show_center(2);
						write_foggy();
						sentence = "VISION 5X5";
						write_tip_sentence(15, sentence, color);//highlight
						show_doublebuffer();
						break;
					case 2:
						foggy_light_for_show_center(3);
						write_foggy();
						sentence = "VISION 7X7";
						write_tip_sentence(18, sentence, color);//highlight
						show_doublebuffer();
						break;
					case 3:
						write_foggy();
						sentence = "RETURN";
						write_tip_sentence(21, sentence, color);//highlight
						show_doublebuffer();
						break;
					}
					reset_foggy_data();
					foggy_light_around(4);
					write_interface();
					write_foggy();
					write_menu_select_vision();
					Sleep(200);//防止连按
				}
			}
		}
	}
}


//6 游戏控制操作函数
void control::game_control()
{
	while (!winFlag&&!loseFlag)//游戏没有结束
	{
		reset_game_screen();//刷新游戏数据以及并且输出到屏幕上
		if (_kbhit())//读入操作
		{
			int key = _getch();
			if (key == 27)//读入的是esc
			{
				game_pause();//调用pause函数对游戏暂停
				continue;
			}
			else if (key == 66 || key == 98)//读入的是 B或b
			{
				game_bag();//打开背包进行操作；
			}
			else if(key==0xE0)//防止其他键干扰
			{
				key = _getch();
				switch (key)
				{
				case 72://向上移动
					player_move_up();
					break;
				case 75://向左移动
					player_move_left();
					break;
				case 77://向右移动
					player_move_right();
					break;
				case 80://向下移动
					player_move_down();
					break;
				}
			}
		}//读入操作 
	}
	if (winFlag)
		game_win();//游戏胜利
	else
		game_lose();//游戏失败
	reset_game_start();//游戏结束 重置游戏数据
}


//7 游戏暂停操作函数
void control::game_pause()
{
	int sign = 0;//选择标志
	WORD color = 0x00;

	//设置输出缓冲区
	write_tip_clear();
	write_menu_pause();//显示暂停菜单
	string sentence = "CONTINUE";
	write_tip_sentence(12, sentence, color);
	show_doublebuffer();
	//设置另一个缓冲区形成闪烁
	write_tip_clear();
	write_menu_pause();//显示暂停菜单
	while (true)
	{
		show_doublebuffer();
		Sleep(300);
		if (_kbhit())
		{
			int key = _getch();
			if (key == 13)
			{
				if (sign == 0)//选择继续游戏
					return;
				else if (sign == 1)//返回开始菜单
				{
					reset_game_start();//重置游戏数据
					game_start();//返回开始菜单
				}
				else//退出游戏
				{
					//!!!待完成函数
					//playe_end
					exit(1);//退出游戏函数
				}
			}
			if(key==0xE0)//防止其他键干扰
			{ 
				key = _getch();
				if (key == 72 || key == 80)
				{
					if (key == 72)
						sign = (sign + 2) % 3;//向上移动
					else
						sign = (sign + 1) % 3;//向下移动
					write_menu_pause();
					switch (sign)
					{
					case 0:
						sentence = "CONTINUE";
						write_tip_sentence(12, sentence, color);//hightlight
						break;
					case 1:
						sentence = "RETURN";
						write_tip_sentence(15, sentence, color);//hightlight
						break;
					case 2:
						sentence = "QUIT GAME";
						write_tip_sentence(18, sentence, color);//hightlight
						break;
					}
					show_doublebuffer();//切屏幕
					write_menu_pause();//设置另一个缓冲区
				}
				Sleep(200);//防止连按
			}
		}
	}
}


//8 游戏胜利操作函数
void control::game_win()
{
	int sign = 0;//选择标志
	WORD color = 0x00;
	//设置输出缓冲区
	reset_screen();
	write_interface();
	write_map();//展示游戏地图
	write_menu_win();//胜利菜单绘制
	string sentence = "CONTINUE PLAY";
	write_tip_sentence(12, sentence, color);
	show_doublebuffer();

	reset_screen();
	write_interface();
	write_menu_win();
	write_map();
	while (true)
	{
		show_doublebuffer();
		Sleep(300);
		if (_kbhit())
		{
			int key = _getch();
			if (key == 13)
			{
				if (!sign)//继续游戏选择菜单
					game_select();
				else//退出游戏
					//game_end
					exit(1);
			}
			if (key == 0xE0)//防止其他键的干扰
			{
				key = _getch();
				if (key == 72 || key == 80)
				{
					sign = (sign + 1) % 2;
					write_menu_win();
					switch (sign)
					{
					case 0:
						sentence = "CONTINUE PLAY";
						write_tip_sentence(12, sentence, color);
						break;
					case 1:
						sentence = "QUIT GAME";
						write_tip_sentence(15, sentence, color);
						break;
					}
					show_doublebuffer();
					write_menu_win();//设置另一个缓冲区
				}
			}
			Sleep(200);//防止连按
		}
	}
}


//9 游戏失败操作函数
void control::game_lose()
{
	int sign = 0;//选择标志
	WORD color = 0x00;
	//设置输出缓冲区
	reset_buffer();
	write_interface();
	write_menu_lose();
	write_map_clear();
	write_map_dead();
	string sentence = "CONTINUE PLAY";
	write_tip_sentence(12, sentence, color);

	show_doublebuffer();
	//设置另一个缓冲区形成闪烁
	reset_buffer();
	write_menu_lose();
	write_map_clear();
	write_map_dead();
	write_interface();
	while (true)
	{
		show_doublebuffer();
		Sleep(300);
		if (_kbhit())
		{
			int key = _getch();
			if (key == 13)
			{
				if (sign == 0)//选择继续游戏
					game_select();
				else if (sign == 1)//退出游戏
				{
					exit(1);
				}
			}
			if (key == 0xE0)//防止其他键的干扰
			{
				key = _getch();
				if (key == 72 || key == 80)
				{
					sign = (sign + 1) % 2;
					switch (sign)
					{
					case 0:
						write_menu_lose();
						sentence = "CONTINUE PLAY";
						write_tip_sentence(12, sentence, color);
						break;
					case 1:
						write_menu_lose();
						sentence = "QUIT GAME";
						write_tip_sentence(15, sentence, color);
						break;
					}
					show_doublebuffer();
					write_interface();
					write_menu_lose();//设置另一个缓冲区
					Sleep(200);//防止连按
				}
			}
		}
	}
}


//10 游戏打开背包操作函数
void control::game_bag()
{
	WORD color0 = 0x00;
	int sign = 0;
	int size = buff_bag.size();
	string sentence;
	reset_game_screen_unshow();
	write_menu_bag();//绘制基底面板
	if (!buff_bag.empty())//绘制背包提示栏闪烁效果
	{
		sentence = buff_bag[sign]->information();
		write_tip_sentence(12 + 2 * sign, sentence, color0);
	}
	show_doublebuffer();//切换界面

	reset_game_screen_unshow();
	write_menu_bag();//切出背包提示栏形成闪烁效果
	while (true)
	{
		show_doublebuffer();//切换界面
		Sleep(300);//闪烁效果
		if (_kbhit())
		{
			int key = _getch();
			if (key == 13&&size!=0)//只有背包不为空的时候才可以使用enter
			{
				if (buff_bag[sign]->use(*this))//判断是否使用道具成功 进行道具的使用
				{
					vector<buff_base*>::iterator it = buff_bag.begin()+sign;//成功使用道具 
					delete (*it);//释放new指针内存
					buff_bag.erase(it);//将其从道具栏中移除
					return;//返回到游戏界面
				}
				else
				{
					reset_game_screen_unshow();//刷新地图内容
					write_menu_bag();//绘制基底面板
					if (!buff_bag.empty())//输出背包道具数量
					{
						sentence = buff_bag[sign]->information();
						write_tip_sentence(12 + 2 * sign, sentence, color0);
					}
					show_doublebuffer();
					reset_game_screen_unshow();
					write_menu_bag();
				}
			}
			else if (key == 27)//输入esc返回
				return;
			//当背包不为空的时候使用上下键进行道具选择
			else if (key == 0xE0&&size!=0)//防止其他键的干扰 选择道具
			{
				key = _getch();
				if (key == 72 || key == 80)
				{
					if (key == 72)
						sign = (sign - 1) % size;//上滚
					else
						sign = (sign + 1) % size;//下滚
					sentence = buff_bag[sign]->information();
					write_menu_bag();
					write_tip_sentence(12 + 2 * sign, sentence, color0);
					write_player_blood();
					show_doublebuffer();
					write_menu_bag();//设置另一个缓冲区
					Sleep(200);//防止连按
				}
			}
		}
	}
}



//人物的操作函数 player_------------------------------------------------------------------------------
//1 移动类函数
void control::player_move_up()
{
	if (player_coord.Y != 0 && !map[player_coord.Y - 1][player_coord.X])//判断越界
		player.move_up();
}
void control::player_move_down()
{
	if (player_coord.Y != 29 && !map[player_coord.Y+1][player_coord.X])//判断越界
		player.move_down();
}
void control::player_move_right()
{
	if (player_coord.X != 29 && !map[player_coord.Y][player_coord.X+1])//判断越界
		player.move_right();
}
void control::player_move_left()
{
	if (player_coord.X != 0 && !map[player_coord.Y][player_coord.X-1])//判断越界
		player.move_left();
}

//2 角色照亮迷雾的函数
void control::player_light_foggy()
{
	int x = player_coord.X-player_vision_l;
	int y = player_coord.Y-player_vision_l;
	for(int i=0;i<2*player_vision_l+1;++i)
		for(int j=0;j<2*player_vision_l+1;++j)
			set_foggy(x+i, y+j, false);//将以自身为中心的照亮player_vision_l*2+1的平方区域
}


//对迷雾的操作函数 foggy_----------------------------------------------------------------------------
//1 对迷雾部分显示用于展示视野大小
void control::foggy_light_for_show_center(int x)
{
	for (int i = 0; i < 2 *x + 1; ++i)
		for (int j = 0; j < 2 * x + 1; ++j)
			set_foggy(15-x+i, 15-x+j, false);//将以自身为中心的(2*x+1)的平方区域
}

//2 照亮终点(2*i+1)平方的区域
void control::foggy_light_end(int i)
{
	int x = end.X - i;
	int y = end.Y - i;
	for (int k = 0; k < 2 * i + 1; ++k)
		for (int j = 0; j < 2 * i + 1; ++j)
			set_foggy(x + k, y + j, false);//将以终点四周的区域
}


//3 照亮界面外围四周i宽度的无语
void control::foggy_light_around(int i)
{
	for (int k = 0; k < i; ++k)
		for (int j = 0; j < 30; ++j)
		{
			set_foggy(k, j, false);
			set_foggy(j, k, false);
			set_foggy(29 - k, j, false);
			set_foggy(j, 29 - k, false);
		}
}