//file: control.cpp

#include"control.h"

using namespace std;

//���캯�� ��ʼ��������Ϣ-------------------------------------------------------------------------------------------------------------------------
control::control()
{
	bufferSwapFlag=true;//ѡ����������建�����ı�־Ϊtrue
	bytes = 0;//���ڵ��û����������Ĳ���
	//�������ڴ�С--------------------------Ϊ 36 * 90 �ľ����С
	system("mode con cols=96 lines=36");
	//���û�������Ϣ
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
	HWND hWnd = GetConsoleWindow(); //��ȡ���ھ��
	LONG_PTR sty = GetWindowLongPtrA(hWnd, GWL_STYLE); //��ȡ������ʽ
	sty = sty & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX; 
	//ȥ���ɱ仯��С,���,��С����ť
	SetWindowLongPtrA(hWnd, GWL_STYLE, sty); //���ô��岻�ɸ��Ĵ�С,�������
	hOutBuf = CreateConsoleScreenBuffer(
		GENERIC_WRITE,//������̿�����������д����
		FILE_SHARE_WRITE,//���建�����ɹ���дȨ��
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
	//�رջ������Ĺ��----------------------------
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = 0;
	cci.dwSize = 1;
	SetConsoleCursorInfo(hOutput, &cci);
	SetConsoleCursorInfo(hOutBuf, &cci);

	set_player_vision(5);//���ó�ʼ��Ұ��С5*5
}

//�����ú���  reset_------------------------------------------------------------------------------------------------------------------------
//1 ��ʼ����Ļ������Ϣ �����Ļ������ϢΪ��
void control::reset_screen_data()
{
	//��ʼ����Ļ����Ϊ��
	for (int i = 0; i < interface_height; ++i)
		for (int j = 0; j < interface_width; ++j)
			screen_data[i][j] = ' ';//�����ı���Ϣ���
}

//2 ��ʼ������������Ϣ 
//�����������ȫ������Ϊ�� ȫͼ��Ϊ����
	void control::reset_foggy_data()
{
	//��foggy����Ϊȫ��Ϊ����
	for (int i = 0; i < 30; ++i)
		for (int j = 0; j < 30; ++j)
			foggy[i][j] = true;//ȫ��Ϊ����
}

//3 ��ʼ����ͼ��Ϣ ȫ��ͼΪ��
void control::reset_map()
{
	//����ͼȫ�����
	for (int i = 0; i < 30; ++i)
		for (int j = 0; j < 30; ++j)
			map[i][j] = false;//ȫ��Ϊ��
}

//4 ��ʼ�����������ı�����  ȫ��������Ϊ�ڵװ���
void control::reset_buffer()
{
	WORD color = 0x0F;//ѡ��ԭʼ��ɫ �ڵװ���
	COORD coord={0,0};//�������ڵ��ú���
	HANDLE item;
	if (!bufferSwapFlag)//���ݱ�־�����û�����
		item = hOutBuf;
	else
		item = hOutput;
	for (int i = 0; i < interface_height; ++i)//��ջ��������ı�����
	{
		for (int j = 0; j < interface_width; ++j)
		{
			coord.X = j;
			coord.Y = i;
			WriteConsoleOutputAttribute(item, &color, 1, coord, &bytes);//�������Ϊ�ڵװ���
		}
	}
}


//5 ��ʼ����������Ϣ ��������������Ļ��հ����ı������ı�����
void control::reset_screen()
{
	reset_screen_data();//�����Ļ�ַ�
	reset_buffer();//����ı�����
}


//6 ������������λ�� �����������ڵ����������������λ��player_coord
void control::reset_player_coord()
{
	player_coord.X = player.get_coord().X;
	player_coord.Y = player.get_coord().Y;
}


//7 ������Ϸ�Ƿ����
void control::reset_game_over_flag()
{
	if (end.X == player_coord.X && end.Y == player_coord.Y)//�ж��Ƿ񵽴��յ�
		winFlag = true;
	else
		winFlag = false;
	if (player.get_blood() < 0)//�ж�Ѫ���Ƿ��Ǹ�ֵ ��������Ϸʧ��
		loseFlag = true;
	else
		loseFlag = false;
}


//8 ������Ϸ�Ŀ�ʼ���� ����Ϸ��ʼ����Ϸ���� ��ͼ ���� �յ� ���� debuff buff �Լ�bag���������� ������ı��������Ұ��С
void control::reset_game_start()
{
	player.blood_add(100000000);//��Ѫ����� �б߽���� ���Գ���
	set_player_coord(1, 1);//����������꿪ʼλ��
	set_end(29, 29);//�����յ�����λ��
	reset_game_over_flag();//������Ϸ������־
	fog_level = 0;//��ʼ������ȼ�Ϊ0
	buff_bag.clear();//��ձ�������
	for (int i = 0; i < buff_vector.size(); ++i)
		delete buff_vector[i];//����buff���newָ���ͷ�
	for (int i = 0; i < debuff_vector.size(); ++i)
		delete debuff_vector[i];//����debuff���newָ���ͷ�
	buff_vector.clear();//��յ�ͼ�ϵ�buff��
	debuff_vector.clear();//��յ�ͼ�ϵ�debuff��
}

//9 ������Ϸ����Ļ���� ��һ�β���֮�� ��Ϸ�������ݵĸ��� �����������Ļ��
void control::reset_game_screen()
{
	player.blood_sub(2 * fog_level + 8);//�Խ�ɫѪ���Ŀ۳�����
	reset_game_screen_unshow();//��ӡ��Ϸ���е���Ļ���� 
	show_doublebuffer();//�л�����
}

//10 ˢ����Ϸ����Ļ���ݲ���� ��һ�β���֮�� ��Ϸ�������ݸ��� �������Ļ�� ������Ѫ���ļ���
void control::reset_game_screen_unshow()
{
	reset_screen();//��ջ�������Ϣ
	reset_player_coord();//�������λ��
	reset_game_over_flag();//������Ϸ������־
	//�ж�debuff������
	for (vector<debuff_base*>::const_iterator it = debuff_vector.cbegin(); it != debuff_vector.cend(); ++it)
	{
		if (*(*it) == player)//�ж��Ƿ�����
		{
			(*it)->occur(*this);//����occur�����������ݸ���
			delete (*it);//�ͷ�debuff��newָ��
			debuff_vector.erase(it);//�ӵ�ͼ�����
			break;
		}
	}
	//�ж�buff������
	for (vector<buff_base*>::iterator it = buff_vector.begin(); it != buff_vector.end(); ++it)
	{
		if (*(*it) == player)//�ж��Ƿ�����
		{
			buff_base* ptr = *it;//����occur�����������ݸ���
			buff_bag.push_back(ptr);//���뱳��֮��
			(*it)->occur(*this);//����Ч��
			buff_vector.erase(it);//�ӵ�ͼ�����
			break;
		}
	}
	write_map();//���Ƶ�ͼ���� �Ḳ�������ڵ�ͼ���������

	//���²������ӡ�ڵ�ͼ����
	for (vector<debuff_base*>::iterator it = debuff_vector.begin(); it != debuff_vector.end(); ++it)
	{
		(*it)->print(*this);//����debuff�ڵ�ͼ�ϵ���ʾ
	}
	for (vector<buff_base*>::iterator it = buff_vector.begin(); it != buff_vector.end(); ++it)
	{
		(*it)->print(*this);//����buff�ڵ�ͼ�ϵ���ʾ
	}
	write_player();//�����������λ��
	write_end();//�����յ�λ��

	//���²����ı���沿�ֵ���Ļ����
	write_interface();//���ƽ��� ��Ӱ���ͼ�����Լ���ʾ������
	write_player_blood();//�������Ѫ��
	write_menu_guide();//������Ϸ��ʾ��

	//���²����������ǵ�ͼ
	reset_foggy_data();//����ȫͼ��������
	player_light_foggy();//���������Ұ�������������
	foggy_light_end(3);//�����յ���Ұ�������������
	write_foggy();//������������Ϸ��ͼ
}

//��ӡ���� ����������Ϣ�������Ļ show_-----------------------------------------------------------------------------------------------------
//1 ����˫�����ӡ���ݵĺ��� ��Ҫ���������������Ļ ֱ�ӵ��þͿ���
void control::show_doublebuffer()
{
	COORD coord = { 0,0 };//���������������ݸ�����Ļ
	coord.X = 0;
	//�л���־Ϊ�ٵ�ʱ�� ��ʱ���������ΪhOutput
	//д�����Ӧ�öԻ�����hOutBuf
	if (bufferSwapFlag == false)
	{
		bufferSwapFlag = true;//���±�־
		for (int i = 0; i < interface_height;++i)
		{
			coord.Y = i;
			WriteConsoleOutputCharacterA(hOutBuf, screen_data[i], interface_width, coord, &bytes);
		}
		SetConsoleActiveScreenBuffer(hOutBuf);//���Ļ������
	}
	//�л���־Ϊ�ٵ�ʱ�� ��ʱ���������ΪhOutBuf
	//д�����Ӧ�öԻ�����hOutput
	else
	{
		bufferSwapFlag = false; // ���±�־
		for (int i = 0; i < interface_height;++i)
		{
			coord.Y = i;
			WriteConsoleOutputCharacterA(hOutput, screen_data[i], interface_width, coord, &bytes);
		}
		SetConsoleActiveScreenBuffer(hOutput);//���Ļ������
	}
}


//д�����ݵ�������֮�еĺ��� ouput_buffer_----------------------------------------------------------------
//x��x���� y��y���� color��д�����ɫ ch��д����ַ�
//����color��ʹ�� ʹ��WORD���� ������ʮ����������ָ����
//��һ��Ϊ�������ڶ�����Ϊǰ��
//0 = ��ɫ		8 = ��ɫ
//1 = ��ɫ		9 = ����ɫ
//2 = ��ɫ		A = ����ɫ
//3 = ����ɫ	B = ��ǳ��ɫ
//4 = ��ɫ		C = ����ɫ
//5 = ��ɫ		D = ����ɫ
//6 = ��ɫ		E = ����ɫ
//7 = ��ɫ		F = ����ɫ
void control::output_buffer(short x, short y, WORD color, char ch)
{
	if (x < 0 || x >= 96 || y < 0 || y >= 36)//���ݳ������� ���Ե�ԭ�����кܶ���������
		return;
	COORD coord = { x,y };
	//�л���־Ϊ�ٵ�ʱ�� ��ʱ���������ΪhOutput
	//д�����Ӧ�öԻ�����hOutBuf
	if (bufferSwapFlag == false)
	{
		WriteConsoleOutputAttribute(hOutBuf, &color, 1, coord, &bytes);//д�뵽Ļ�󻺳���
	}
	//�л���־Ϊ�ٵ�ʱ�� ��ʱ���������ΪhOutBuf
	//д�����Ӧ�öԻ�����hOutput
	else
	{
		WriteConsoleOutputAttribute(hOutput, &color, 1, coord, &bytes);//д�뵽Ļ�󻺳���
	}
	screen_data[y][x] = ch;//����Ӧ�ַ�д�뵽��Ļ�ַ����� ���л���������ʱ������뵽������֮��
}



//�����������ݵĺ��� set_---------------------------------------------------------------------------------
//1 ���õ�ͼ��Ϣ
//����һ��30*30��bool����
void control::set_map(bool rhs[30][30])
{
	for (int i = 0; i < 30; ++i)//������ͼ
		for (int j = 0; j < 30; ++j)
			map[i][j] = rhs[i][j];//������ͼ
}


//2 ����������Ϣ
//��x y ����sign true Ϊ������
void control::set_foggy(int x, int y, bool sign)
{
	if (x < 0 || x >= 30 || y < 0 || y >= 30)//Խ�� �������� 
		return;//ѡ��return���ӵ�ԭ�� ��������������Ĳ����кܶ඼��Խ��
	foggy[y][x] = sign;//����Ϊsign
}


//3 �����յ�λ�� 
//����30*30 ��Χ��0��29 
//����һ�����꽫�յ�����
//�ṩ�߽���
void control::set_end(int x, int y)
{
	if (x < 0 || y < 0 || x >= 30 || y >= 30)
		throw out_of_range("set of end is fault");//���ó��쳣
	end.X = x;//�ɹ�����
	end.Y = y;
}


//4 ������Ϸ�����Ұ��С
//����Ϊ a*a�Ĵ�С
void control::set_player_vision(int a)
{
	if (a <= 1)//��С��Ұ �׳��쳣
		throw out_of_range("vision can not be less than 0");
	player_vision_l = (a - 1) / 2;//��Ӧ����ҰΪ (player_vision_l*2+1)*��player_vision_l*2+1)
}

//5 ������Ϸ��ҵ�λ��
//ͬʱ�������λ��
//���յ�ͼ30*30 
//���λ�� ��Χ 0-29
//�ṩ�߽���
void control::set_player_coord(int x, int y)
{
	if (x < 0 || y < 0 || x >= 30 || y >= 30)
		throw out_of_range("player's coord is fault");//���ó��� �׳��쳣
	player.set_coord(x, y);//�ɹ����� �ṩ�߽��� ���������ڵ�ͼ��ǽ��λ��
	reset_player_coord();//��������
}

//6 ��������ĵȼ�
//level true ����Ϊ��ɫ����
//level false ����Ϊ��ɫ����
void control::set_foggy_level(bool level)
{
	fog_level = level;//����Ϊ��Ӧlevel
}

//����buff�� �Լ�debuff �� Ϊ�˷������ �ؼ���ע��
//������� ��Ҫע����� ���е��඼��Ҫʹ��newָ��
//������ �� 3 4 ������� debuff�е� sphinxAsk
//debuff_base* de_number=new sphinxAsk��3��4��
//debuff_vector.push_back(de_number);
//�����Ϳ��Գɹ����
//�����ڶ������ڴ��ͷŶ��а�ȫ�ԵĴ����ͷ� ���迼��
//�������ر�ע�� ���벻�ṩ�߽��� 
//�����С�ķ����ڵ�ͼǽ���� ���ᷢ���쳣

//����Ϊdebuff �Լ� buff�ľ�������
//debuff: 
//1 ������ teleportationArray 
//2 ����Ģ�� stunMushrooms
//3 �˶���ħ�� pandoraBox
//4 ˹�ҿ�˹֮�� sphinxAsk
//buff:
//1 ��� torch
//2 ���� shovel
//3 �ⶾ�� moderator
//4 ʥ�� holyLight

//7 ������ϷΪ�ؿ�1
void control::set_game_level_1()
{
	reset_game_start();//������������Ϸ����
	//------------------------------------ ��debuff�ķ���
	debuff_base* de_1 =new teleportationArray(10, 11);
	debuff_base* de_2 = new stunMushrooms(12, 13);
	debuff_base* de_3 = new pandoraBox(23, 18);
	debuff_base* de_4 = new sphinxAsk(29, 26);
	debuff_vector.push_back(de_1);
	debuff_vector.push_back(de_2);
	debuff_vector.push_back(de_3);
	debuff_vector.push_back(de_4);


	//------------------------------------ ��buff�ķ���
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
	set_map(map1);//���õ�ͼΪ1
}

//8 ������ϷΪ�ؿ�2
void control::set_game_level_2()
{
	reset_game_start();//������������Ϸ����
	//------------------------------------ ��debuff�ķ���



	//------------------------------------ ��buff�ķ���

	set_map(map2);//���õ�ͼΪ2
}

//9 ������ϷΪ�ؿ�3
void control::set_game_level_3()
{
	reset_game_start();//������������Ϸ����
	//------------------------------------ ��debuff�ķ���



	//------------------------------------ ��buff�ķ���

	set_map(map3);//���õ�ͼΪ3
}


//10 ���õ�ͼx y����λ�õ�����Ϊ sign
void control::set_map_dot(int x, int y,bool sign)
{
	if (x < 0 || y < 0 || x>29 || y>29)
		throw out_of_range("out of range");
	else
		map[y][x] = sign;

}


//д�����ݵ���Ļ���ݵĺ��� write_--------------------------------------------------------------------------
//1 ���ƾ��ӵ����� ָ���ڵ�line�� ָ��sentence ָ��color
void control::write_tip_sentence(int line,string sentence,WORD color)
{
	if (line < 0 || line >= tooltip_height || sentence.size()>(size_t)tooltip_width)//Խ�� �׳��쳣
		throw out_of_range("sentence is too long to print or line is unexcepted!");
	size_t x = (size_t)map_width+4+((size_t)tooltip_width-sentence.size())/2;//���д�ӡ
	size_t y = line + 3;
	for (size_t i = 0; i < sentence.size(); ++i,++x)
	{
		output_buffer(x, y, color, sentence[i]);//����ѭ��������
	}
}


//2 ���Ƶ�ͼ ���ݵ�ͼ���ݻ��Ƶ�ͼ����Ļ�� �Ḳ�ǵ�ͼ���������е���ʾ
void control::write_map()
{
	WORD color_wall = 0x88;//ǽ����ɫ
	WORD color_road = 0x00;//ͨ·����ɫ
	for(int i=0;i<30;++i)//����ͼ��ӡ����
		for (int j = 0; j < 30; ++j)
		{
			int x = 2 * j + 2;//����任
			int y = i + 3;//����任
			if (map[i][j])//�˴���ǽ�� 
			{
				output_buffer(x, y, color_wall, ' ');
				output_buffer(x+1, y, color_wall, ' ');
			}
			else//�˴���ͨ·
			{
				output_buffer(x, y, color_road, ' ');
				output_buffer(x + 1, y, color_road, ' ');
			}
		}
}


//3 ����ʾ������� ����ʾ������������������
void control::write_tip_clear()
{
	WORD color = 0x00;//����Ϊ����ɫ
	string sentence = "                          ";
	for(int i=0;i<map_height;++i)
	write_tip_sentence(i, sentence, color);//�����ʾ������Ϣ
}


//4 �����ޱ����Ŀ�ʼ �˵������� ����ʾ��
void control::write_menu_start()
{
	write_tip_clear();//�����
	WORD color = 0x06;
	string sentence = "START MENU";
	write_tip_sentence(6, sentence, color);
	sentence = "PLAY GAME";
	write_tip_sentence(12, sentence, color);
	sentence = "QUIT GAME";
	write_tip_sentence(15, sentence, color);
}


//5 �����ޱ�����ѡ����Ϸ �Ѷ� �˵������� ����ʾ��
void control::write_menu_select()
{
	write_tip_clear();//�����
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

//6 �����ޱ�����ѡ����Ϸ �Ѷ� �˵������� ����ʾ��
void control::write_menu_select_level()
{
	write_tip_clear();//�����
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


//7 �����ޱ�����ѡ����Ϸ ѡ����Ұ �˵������� ����ʾ��
void control::write_menu_select_vision()
{
	write_tip_clear();//�����
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

//8 ������Ϸ���е���ʾ �˵������� ����ʾ��
void control::write_menu_guide()
{
	write_tip_clear();//�����
	WORD color = 0x06;
	string sentence = "GUIDE";
	write_tip_sentence(6, sentence, color);
	sentence = "USE �� �� �� ��";
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


//9 �������� ����������Ϣ���ǵ�ͼ���������
void control::write_foggy()
{
	WORD color_blind;
	if (fog_level == 0)//��������ĵȼ�������ı���ɫ
		color_blind = 0x07;//��ɫ
	else
		color_blind = 0x04;//��ɫ
	for (int i = 0; i < 30; ++i)
		for (int j = 0; j < 30; ++j)
		{
			if (foggy[i][j])
			{
				int x = 2 * j + 2;
				int y = i + 3;//����任
				output_buffer(x, y, color_blind, '*');//��ӡ����
				output_buffer(x + 1, y, color_blind, '*');
			}
		}
}


//10 ������ͣ�˵� ����ʾ��
void control::write_menu_pause()
{
	write_tip_clear();//�����
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


//11 ������� �����������λ�û������ͼ��
void control::write_player()
{
	WORD color = 0x03;
	output_buffer(2*player_coord.X + 2, player_coord.Y + 3, color, '<');
	output_buffer(2*player_coord.X + 3, player_coord.Y + 3, color, '>');
}

//12 ������Ϸʤ���˵�  ����ʾ��
void control::write_menu_win()
{
	write_tip_clear();//�����
	WORD color = 0x06;
	string sentence = "YOU WIN";
	write_tip_sentence(6, sentence, color);
	sentence = "CONTINUE PLAY";
	write_tip_sentence(12, sentence, color);
	sentence = "QUIT GAME";
	write_tip_sentence(15, sentence, color);
}


//13 �����յ�����λ�� �����յ�λ�û����յ�ͼ��
void control::write_end()
{
	WORD color = 0x44;//ȫ��Ϊ��ɫ
	output_buffer(2 * end.X+2, end.Y + 3, color, ' ');
	output_buffer(2 * end.X+3, end.Y + 3, color, ' ');

}

//14 ���ƽ���߿���
void control::write_interface()
{
	WORD text_color_regular = 0x0F;//�ı�����ɫ
	WORD text_color1 = 0x22;//��ɫ
	for (int i = 0; i < map_width; ++i)//���Ƶ�ͼ������ɫ��
	{
		output_buffer(i + 2, 1, text_color1, ' ');
		output_buffer(i + 2, interface_height - 2, text_color1, ' ');
	}
	for (int i = 0; i < tooltip_width; ++i)//������ʾ��������ɫ��
	{
		output_buffer(i + 4 + map_width, 1, text_color1, ' ');
		output_buffer(i + 4 + map_width, interface_height - 2, text_color1, ' ');
	}
	for (int i = 0; i < interface_width; ++i)//���Ʊ߽����µĿ�
	{
		output_buffer(i, 0, text_color_regular, 'H');
		output_buffer(i, 2, text_color_regular, 'H');
		output_buffer(i, interface_height - 3, text_color_regular, 'H');
		output_buffer(i, interface_height - 1, text_color_regular, 'H');
	}
	for (int i = 0; i < interface_height; ++i)//���Ʊ߽����ҵĸ�
	{
		output_buffer(0, i, text_color_regular, 'H');
		output_buffer(1, i, text_color_regular, 'H');
		output_buffer(2 + map_width, i, text_color_regular, 'H');
		output_buffer(3 + map_width, i, text_color_regular, 'H');
		output_buffer(interface_width - 1, i, text_color_regular, 'H');
		output_buffer(interface_width - 2, i, text_color_regular, 'H');
	}
}


//15 �ڵ�ͼ��������ı����� ָ���ڵ�line�� ����Ϊsentence ��ɫΪcolor
void control::write_map_sentence(int line, string sentence, WORD color)
{
	if (line < 0 || line >= map_height || sentence.size()>(size_t)map_width)//Խ�� �׳��쳣
		throw out_of_range("sentence is too long to print or line is unexcepted!");
	int x = 2 + (map_width - sentence.size()) / 2;//���д�ӡ
	int y = line + 3;//����任
	for (size_t i = 0; i < sentence.size(); ++i, ++x)
	{
		output_buffer(x, y, color, sentence[i]);//ѭ����ӡ����
	}
}

//16 ��յ�ͼ������ı����� ����ͼ���������������
void control::write_map_clear()
{
	WORD color = 0x00;//�ı�����ɫ����
	string sentence = "                                                            ";
	for (int i = 0; i < map_height; ++i)
		write_map_sentence(i, sentence, color);//��յ�ͼ���������
}


//17 �ڵ�ͼ���������Ϸ����Ϸ��ʼѡ��˵���ʱ��� ��ʾ�ı����� �ڵ�ͼ���� 
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


//18 �������Ѫ�� ����Ѫ�������ڽ����������Ѫ������
void control::write_player_blood()
{
	WORD color_text = 0x03;//blood����ɫ
	WORD color_blood = 0x44;//Ѫ������ɫ
	WORD color_empty = 0x00;//����ɫ
	string sentence = "BLOOD";
	for (int i = 0; i < map_width; ++i)//���»�����ɫ��
	{
		output_buffer(i + 2, 1, color_empty, ' ');//����ɫ���滻Ϊ�հ���
	}
	for (int i = 0; i < sentence.size(); ++i)
		output_buffer(i + 2, 1, color_text, sentence[i]);//����blood����ʾ

	for (int i = 0; i < player.get_blood()/100; ++i)//���Ƶ�ͼ������ɫ��
	{
		output_buffer(i + 2+sentence.size(), 1, color_blood, ' ');//����Ѫ������ɫ����
	}
}


//19 ������Ϸʧ�ܲ˵� ����ʾ��
void control::write_menu_lose()
{
	write_tip_clear();//�����
	WORD color = 0x06;
	string sentence = "YOU LOSE";
	write_tip_sentence(6, sentence, color);
	sentence = "CONTINUE PLAY";
	write_tip_sentence(12, sentence, color);
	sentence = "QUIT GAME";
	write_tip_sentence(15, sentence, color);
}

//20 ������Ϸʧ�ܲ˵� ����ʾ��
void control::write_map_dead()
{
	WORD color0 = 0x04;
	string sentence = "YOU DIED...";
	write_map_sentence(14, sentence, color0);//��ͼ������ʾ
}


//21 ����ʾ�����Ʊ������ѡ��˵� ����ʾ��
void control::write_menu_bag()
{
	write_tip_clear();//�������ʾ������
	WORD color0 = 0x06;
	WORD color1 = 0x04;
	WORD color2 = 0x09;
	string sentence = "YOUR BAG";
	write_tip_sentence(6, sentence, color1);
	sentence = "ENTER ESC RETURN";
	write_tip_sentence(8, sentence, color0);
	if (buff_bag.empty())//�������Ϊ�� ������Ϸ���
	{
		sentence = "YOUR BAG IS EMPTY";
		write_tip_sentence(14, sentence, color1);
		sentence = "CLOWN!";
		write_tip_sentence(16, sentence, color1);
		return;
	}
	int size = buff_bag.size();//����bag������ 
	for (int i = 0; i < size; ++i)
	{
		sentence = buff_bag[i]->information();
		write_tip_sentence(12+2*i, sentence, color2);//����ʾ����ӡÿһ�����ߵ���״
	}
}





//���Ŷ����ĺ��� play_----------------------------------------------------------------------------------
//1 ���ſ�ʼ��������
void control::play_start()
{
	int frame1 = 0;//���ڲ��ŵ�һ�ζ���
	int frame2 = 0;//���ڲ��ŵڶ��ζ���
	int frame3 = 0;//���ڲ��ŵ����ζ���
	COORD initial_pos1 = { 0,16 };
	COORD initial_pos2 = { 46,0 };
	COORD initial_pos3 = { 23,14 };
	COORD initial_pos4 = { 23,13 };
	while (frame1 <= 40)
	{
		//�����ӡ
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
		show_doublebuffer();//�л����������д��
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
	reset_screen();//�����Ļ
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
	reset_screen();//�����Ļ
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
	reset_screen();//�����Ļ
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
	reset_screen();//�����Ļ
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


//2 ���Ž�����������
void control::play_end()
{
	//�պ�����
}

//������Ϸ�������� game_--------------------------------------------------------------------------------------------------------------------
//1 ���庯��
void control::game()
{
	play_start();//���ſ�ʼ����
	game_start();//��ʼ������Ϸ
}

//2 ��ʼ�˵�ѡ��������� 
void control::game_start()
{
	reset_screen();//�����Ļ����
	int sign = 0;
	WORD color = 0x00;//ѡ��highlight����ɫ
	write_menu_start();//��ʾ��ʾ���˵�
	string sentence = "PLAY GAME";
	write_tip_sentence(12, sentence, color);//highlightһ��
	write_interface();//�������
	write_map_guide();//�����ͼ����ʾ
	show_doublebuffer();//�л�����

	//������һ���������γ���˸Ч��
	reset_screen();
	write_map_guide();//�����ͼ����ʾ
	write_menu_start();
	write_interface();
	while (true)//ѭ���ȴ�ѡ��
	{
		show_doublebuffer();//�л������г̶���
		if (_kbhit())//�ж��Ƿ��м���
		{
			int key = _getch();
			if (key == 13)//����Ϊ�س�
			{
				if (!sign)//ѡ�������Ϸ
				{
					game_select();//����ѡ��˵�
				}
				else//�˳���Ϸ
				{
					//game_end();//������Ϸ��������
					exit(1);//���˳�
				}
			}
			if (key == 0xE0)//��ֹ�������ĸ���
			{
				key = _getch();
				if (key == 72 || key == 80)//�����Ƿ����
				{//�л�ѡ��
					if (sign == 1)//ѡ��Ϊ������Ϸ
					{
						sign = 0;
						write_menu_start();
						sentence = "PLAY GAME";
						write_tip_sentence(12, sentence, color);//hightlight
					}
					else//ѡ��Ϊ�˳���Ϸ
					{
						sign = 1;
						write_menu_start();
						sentence = "QUIT GAME";
						write_tip_sentence(15, sentence, color);//hightlight
					}
					show_doublebuffer();//�л���Ļ
					write_menu_start();//����������һ��������
				}
			}
			Sleep(200);
		}
		Sleep(300);
	}
}

//3 ѡ��˵���������
void control::game_select()
{
	int sign = 0;//ѡ���־
	WORD color = 0x00;//highlight����ɫ��־
	reset_screen();//�����Ļ
	write_menu_select();//����ʾ����ʾѡ��˵�
	string sentence = "SELECT LEVEL";//highlight
	write_tip_sentence(12, sentence, color);
	write_interface();//���ƽ���
	show_doublebuffer();//����
	//������һ���������γ���˸
	reset_screen();//�����Ļ
	write_menu_select();//����ѡ��˵�
	write_interface();//���ƽ���
	//!!!��������������ڵ�ͼ������ʾ��ʾ
	while (true)
	{
		show_doublebuffer();//�л���Ļ
		Sleep(300);
		if (_kbhit())
		{
			int key = _getch();
			if (key == 13)
			{
				if (sign==0)//����ؿ�ѡ��˵�
				{
					game_select_map();
				}
				else if(sign==1)//������Ұѡ��˵�
				{
					game_select_vision();
				}
				else//������һ�� ���˵�
				{
					game_start();
				}
			}
			if (key == 0xE0)//��ֹ�������ĸ���
			{
				key = _getch();
				if (key == 72 || key == 80)
				{
					if (key == 72)
						sign = (sign + 2) % 3;//�����ƶ�
					else
						sign = (sign + 1) % 3;//�����ƶ�

					switch (sign)
					{
					case 0://ѡ��Ϊѡ��ؿ�
						write_menu_select();
						sentence = "SELECT LEVEL";
						write_tip_sentence(12, sentence, color);//hightlight
						show_doublebuffer();

						write_menu_select();//������һ��������
						break;
					case 1://ѡ��Ϊѡ���Ѷ�
						write_menu_select();
						sentence = "SET VISION";
						write_tip_sentence(15, sentence, color);//hightlight
						show_doublebuffer();

						write_menu_select();//������һ��������
						break;
					case 2://ѡ��Ϊѡ�񷵻�
						write_menu_select();
						sentence = "RETURN";
						write_tip_sentence(18, sentence, color);//hightlight
						show_doublebuffer();

						write_menu_select();//������һ��������
						break;
					}
					Sleep(200);//��ֹ����
				}
			}
		}
	}
}

//4 ѡ��ؿ������˵�����
void control::game_select_map()
{
	int sign = 0;
	WORD color = 0x00;
	//�������������
	reset_screen();
	write_interface();
	write_menu_select_level();
	string sentence = "LEVEL   1";//hightlight
	write_tip_sentence(12, sentence, color);
	show_doublebuffer();
	//������һ���������γ���˸
	reset_screen();
	write_menu_select_level();
	write_interface();
	//������������ӵ�ͼ��ʾ����
	while (true)
	{
		show_doublebuffer();
		Sleep(300);
		if (_kbhit())
		{
			int key = _getch();
			if (key == 13)
			{
				if (sign == 3)//ѡ�񷵻���һ���˵�
					game_select();
				else
				{
					if (sign == 0)//����ؿ�1
					{
						set_game_level_1();
					}
					else if (sign == 1)//����ؿ�2
					{
						set_game_level_2();
					}
					else if (sign == 2)//����ؿ�3
					{
						set_game_level_3();
					}
					game_control();//������Ϸ���ƻ���
				}
			}
			if (key == 0xE0)//��ֹ�������ĸ���
			{
				key = _getch();
				if (key == 72 || key == 80)
				{
					if (key == 72)
						sign = (sign + 3) % 4;//�����ƶ�
					else
						sign = (sign + 1) % 4;//�����ƶ�
					write_menu_select_level();
					switch (sign)
					{
					case 0://ѡ��ؿ�1
						sentence = "LEVEL   1";
						write_tip_sentence(12, sentence, color);//hightlight
						break;
					case 1://ѡ��ؿ�2
						sentence = "LEVEL   2";
						write_tip_sentence(15, sentence, color);//hightlight
						break;
					case 2://ѡ��ؿ�3
						sentence = "LEVEL   3";
						write_tip_sentence(18, sentence, color);//hightlight
						break;
					case 3://ѡ��ؿ�4
						sentence = "RETURN";
						write_tip_sentence(21, sentence, color);//highlight
						break;
					}
					show_doublebuffer();
					write_menu_select_level();//������һ��������
				}
			}
			Sleep(200);//��ֹ����
		}
	}
}

//5 ѡ���ѶȲ�������
void control::game_select_vision()
{
	int sign = 0;
	WORD color = 0x00;
	write_map_clear();//�����Ż� Ϊ�˿����γ���˸
	reset_foggy_data();//������������
	write_menu_select_vision();
	foggy_light_around(4);//������������
	write_interface();
	write_foggy();
	show_doublebuffer();
	//������һ���������γ���˸

	write_map_clear();
	reset_foggy_data();//������������
	write_menu_select_vision();
	foggy_light_around(4);//������������
	foggy_light_for_show_center(1);//���ú���������������
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
				if (sign == 0)//�����Ѷ�1 ������Ϸѡ��˵�
					set_player_vision(3);
				else if (sign == 1)//�����Ѷ�2 ������Ϸѡ��˵�
					set_player_vision(5);
				else if (sign == 2)//�����Ѷ�3 ������Ϸѡ��˵�
					set_player_vision(7);
				game_select();//������Ϸѡ��˵�
			}
			if (key == 0xE0)//��ֹ�������ĸ���
			{
				key = _getch();
				if (key == 72 || key == 80)
				{
					if (key == 72)
						sign = (sign + 3) % 4;//�����ƶ�
					else
						sign = (sign + 1) % 4;//�����ƶ�
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
					Sleep(200);//��ֹ����
				}
			}
		}
	}
}


//6 ��Ϸ���Ʋ�������
void control::game_control()
{
	while (!winFlag&&!loseFlag)//��Ϸû�н���
	{
		reset_game_screen();//ˢ����Ϸ�����Լ������������Ļ��
		if (_kbhit())//�������
		{
			int key = _getch();
			if (key == 27)//�������esc
			{
				game_pause();//����pause��������Ϸ��ͣ
				continue;
			}
			else if (key == 66 || key == 98)//������� B��b
			{
				game_bag();//�򿪱������в�����
			}
			else if(key==0xE0)//��ֹ����������
			{
				key = _getch();
				switch (key)
				{
				case 72://�����ƶ�
					player_move_up();
					break;
				case 75://�����ƶ�
					player_move_left();
					break;
				case 77://�����ƶ�
					player_move_right();
					break;
				case 80://�����ƶ�
					player_move_down();
					break;
				}
			}
		}//������� 
	}
	if (winFlag)
		game_win();//��Ϸʤ��
	else
		game_lose();//��Ϸʧ��
	reset_game_start();//��Ϸ���� ������Ϸ����
}


//7 ��Ϸ��ͣ��������
void control::game_pause()
{
	int sign = 0;//ѡ���־
	WORD color = 0x00;

	//�������������
	write_tip_clear();
	write_menu_pause();//��ʾ��ͣ�˵�
	string sentence = "CONTINUE";
	write_tip_sentence(12, sentence, color);
	show_doublebuffer();
	//������һ���������γ���˸
	write_tip_clear();
	write_menu_pause();//��ʾ��ͣ�˵�
	while (true)
	{
		show_doublebuffer();
		Sleep(300);
		if (_kbhit())
		{
			int key = _getch();
			if (key == 13)
			{
				if (sign == 0)//ѡ�������Ϸ
					return;
				else if (sign == 1)//���ؿ�ʼ�˵�
				{
					reset_game_start();//������Ϸ����
					game_start();//���ؿ�ʼ�˵�
				}
				else//�˳���Ϸ
				{
					//!!!����ɺ���
					//playe_end
					exit(1);//�˳���Ϸ����
				}
			}
			if(key==0xE0)//��ֹ����������
			{ 
				key = _getch();
				if (key == 72 || key == 80)
				{
					if (key == 72)
						sign = (sign + 2) % 3;//�����ƶ�
					else
						sign = (sign + 1) % 3;//�����ƶ�
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
					show_doublebuffer();//����Ļ
					write_menu_pause();//������һ��������
				}
				Sleep(200);//��ֹ����
			}
		}
	}
}


//8 ��Ϸʤ����������
void control::game_win()
{
	int sign = 0;//ѡ���־
	WORD color = 0x00;
	//�������������
	reset_screen();
	write_interface();
	write_map();//չʾ��Ϸ��ͼ
	write_menu_win();//ʤ���˵�����
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
				if (!sign)//������Ϸѡ��˵�
					game_select();
				else//�˳���Ϸ
					//game_end
					exit(1);
			}
			if (key == 0xE0)//��ֹ�������ĸ���
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
					write_menu_win();//������һ��������
				}
			}
			Sleep(200);//��ֹ����
		}
	}
}


//9 ��Ϸʧ�ܲ�������
void control::game_lose()
{
	int sign = 0;//ѡ���־
	WORD color = 0x00;
	//�������������
	reset_buffer();
	write_interface();
	write_menu_lose();
	write_map_clear();
	write_map_dead();
	string sentence = "CONTINUE PLAY";
	write_tip_sentence(12, sentence, color);

	show_doublebuffer();
	//������һ���������γ���˸
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
				if (sign == 0)//ѡ�������Ϸ
					game_select();
				else if (sign == 1)//�˳���Ϸ
				{
					exit(1);
				}
			}
			if (key == 0xE0)//��ֹ�������ĸ���
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
					write_menu_lose();//������һ��������
					Sleep(200);//��ֹ����
				}
			}
		}
	}
}


//10 ��Ϸ�򿪱�����������
void control::game_bag()
{
	WORD color0 = 0x00;
	int sign = 0;
	int size = buff_bag.size();
	string sentence;
	reset_game_screen_unshow();
	write_menu_bag();//���ƻ������
	if (!buff_bag.empty())//���Ʊ�����ʾ����˸Ч��
	{
		sentence = buff_bag[sign]->information();
		write_tip_sentence(12 + 2 * sign, sentence, color0);
	}
	show_doublebuffer();//�л�����

	reset_game_screen_unshow();
	write_menu_bag();//�г�������ʾ���γ���˸Ч��
	while (true)
	{
		show_doublebuffer();//�л�����
		Sleep(300);//��˸Ч��
		if (_kbhit())
		{
			int key = _getch();
			if (key == 13&&size!=0)//ֻ�б�����Ϊ�յ�ʱ��ſ���ʹ��enter
			{
				if (buff_bag[sign]->use(*this))//�ж��Ƿ�ʹ�õ��߳ɹ� ���е��ߵ�ʹ��
				{
					vector<buff_base*>::iterator it = buff_bag.begin()+sign;//�ɹ�ʹ�õ��� 
					delete (*it);//�ͷ�newָ���ڴ�
					buff_bag.erase(it);//����ӵ��������Ƴ�
					return;//���ص���Ϸ����
				}
				else
				{
					reset_game_screen_unshow();//ˢ�µ�ͼ����
					write_menu_bag();//���ƻ������
					if (!buff_bag.empty())//���������������
					{
						sentence = buff_bag[sign]->information();
						write_tip_sentence(12 + 2 * sign, sentence, color0);
					}
					show_doublebuffer();
					reset_game_screen_unshow();
					write_menu_bag();
				}
			}
			else if (key == 27)//����esc����
				return;
			//��������Ϊ�յ�ʱ��ʹ�����¼����е���ѡ��
			else if (key == 0xE0&&size!=0)//��ֹ�������ĸ��� ѡ�����
			{
				key = _getch();
				if (key == 72 || key == 80)
				{
					if (key == 72)
						sign = (sign - 1) % size;//�Ϲ�
					else
						sign = (sign + 1) % size;//�¹�
					sentence = buff_bag[sign]->information();
					write_menu_bag();
					write_tip_sentence(12 + 2 * sign, sentence, color0);
					write_player_blood();
					show_doublebuffer();
					write_menu_bag();//������һ��������
					Sleep(200);//��ֹ����
				}
			}
		}
	}
}



//����Ĳ������� player_------------------------------------------------------------------------------
//1 �ƶ��ຯ��
void control::player_move_up()
{
	if (player_coord.Y != 0 && !map[player_coord.Y - 1][player_coord.X])//�ж�Խ��
		player.move_up();
}
void control::player_move_down()
{
	if (player_coord.Y != 29 && !map[player_coord.Y+1][player_coord.X])//�ж�Խ��
		player.move_down();
}
void control::player_move_right()
{
	if (player_coord.X != 29 && !map[player_coord.Y][player_coord.X+1])//�ж�Խ��
		player.move_right();
}
void control::player_move_left()
{
	if (player_coord.X != 0 && !map[player_coord.Y][player_coord.X-1])//�ж�Խ��
		player.move_left();
}

//2 ��ɫ��������ĺ���
void control::player_light_foggy()
{
	int x = player_coord.X-player_vision_l;
	int y = player_coord.Y-player_vision_l;
	for(int i=0;i<2*player_vision_l+1;++i)
		for(int j=0;j<2*player_vision_l+1;++j)
			set_foggy(x+i, y+j, false);//��������Ϊ���ĵ�����player_vision_l*2+1��ƽ������
}


//������Ĳ������� foggy_----------------------------------------------------------------------------
//1 ����������ʾ����չʾ��Ұ��С
void control::foggy_light_for_show_center(int x)
{
	for (int i = 0; i < 2 *x + 1; ++i)
		for (int j = 0; j < 2 * x + 1; ++j)
			set_foggy(15-x+i, 15-x+j, false);//��������Ϊ���ĵ�(2*x+1)��ƽ������
}

//2 �����յ�(2*i+1)ƽ��������
void control::foggy_light_end(int i)
{
	int x = end.X - i;
	int y = end.Y - i;
	for (int k = 0; k < 2 * i + 1; ++k)
		for (int j = 0; j < 2 * i + 1; ++j)
			set_foggy(x + k, y + j, false);//�����յ����ܵ�����
}


//3 ����������Χ����i��ȵ�����
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