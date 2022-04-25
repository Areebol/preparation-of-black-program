//file: buff.cpp

#include"buff.h"
#include<stdexcept>
#include<string>
using std::string;
using std::out_of_range;

void buff_base::print(control& rhs)
{
    rhs.output_buffer(2 * coord_x + 2, coord_y+ 3 , 0x55, '&');
    rhs.output_buffer(2 * coord_x + 3, coord_y + 3, 0x55, '&');
}
//��������λ��
void buff_base::set_coord(int x, int y)
{
    if (x < 0 || y < 0 || x>29 || y>29)
        throw out_of_range("coord is default");
    coord_x = x;
    coord_y = y;
}

//����Ƿ���character����
bool buff_base::operator==(character& rhs)
{
    COORD coord_player = rhs.get_coord();
    if (coord_player.X == this->coord_x && coord_player.Y == this->coord_y)
        return true;
    else
        return false;
}




//�����--------------------------------------������Ұ
bool torch::use(control& rhs)
{
    int key=0;
    show_select_menu(rhs);
    while(key!=13&&key!=27)
        key = _getch();
    if (key == 27)//����esc����
        return false;
    else if (key == 13)//����س���ȷ��
        rhs.set_player_vision((rhs.player_vision_l + 1) * 2 + 1);
    return true;
}

//���ص�����Ϣ
string torch::information()
{
    return "̽�յ�";
}

//��ʼ������
torch::torch(int x, int y)
{
    set_coord(x, y);
}

//��������
void torch::occur(control& rhs)
{
    WORD color = 0x05;
    //��ʾ������̽�յ�
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "���������̽�յ�";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "�򿪱������Խ���ʹ��";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//չʾ��ʾ��Ϣ2500ms
}

void torch::show_select_menu(control& rhs)
{
    WORD color0 = 0x06;
    WORD color1 = 0x04;
    rhs.write_tip_clear();
    string sentence = "̽�յƵ�ʹ��";
    rhs.write_tip_sentence(7, sentence, color0);

    sentence = "Esc ����ʹ��";
    rhs.write_tip_sentence(14, sentence, color1);
    sentence = "Enter ѡ��ʹ������ǰ��Ұ";
    rhs.write_tip_sentence(16, sentence, color1);
    rhs.show_doublebuffer();
}


//ˮ������------------------------------------����������Ұ

crystalBall::crystalBall(int x, int y)
{
    set_coord(x, y);
}

bool crystalBall::use(control& rhs)
{
    return false;
}


string crystalBall::information()
{
    return "ˮ����";
}





//������--------------------------------------��ǽ
bool shovel::use(control& rhs)
{
    int key = 0;
    int sign = 0;
    int coord_x = rhs.player_coord.X;
    int coord_y = rhs.player_coord.Y;
    WORD color1 = 0x06;
    string sentence;
    rhs.reset_game_screen_unshow();
    show_select_menu(rhs);
    rhs.show_doublebuffer();

    rhs.reset_game_screen_unshow();
    show_select_menu(rhs); 
    while (true)
    {
        rhs.show_doublebuffer();
        key = _getch();
        if (key == 27)//����esc����
            return false;
        else if (key == 13)//����س���ȷ��
        {
            switch (sign)
            {
            case 72://ѡ����ǽ
                rhs.set_map_dot(coord_x, coord_y - 1, false);
                break;
            case 75://ѡ����ǽ
                rhs.set_map_dot(coord_x - 1, coord_y, false);
                break;
            case 77://ѡ����ǽ
                rhs.set_map_dot(coord_x + 1, coord_y, false);
                break;
            case 80://ѡ����ǽ
                rhs.set_map_dot(coord_x, coord_y + 1, false);
                break;
            default:
                WORD color = 0x05;
                //��ʾ������̽�յ�
                rhs.write_map_clear();
                rhs.write_tip_clear();
                rhs.write_interface();
                string sentence = "ʹ��ʧ��";
                rhs.write_map_sentence(14, sentence, color);
                sentence = "��û��ѡ�������ѡ������û��ǽ��";
                rhs.write_map_sentence(16, sentence, color);
                rhs.show_doublebuffer();
                Sleep(2500);//չʾ��ʾ��Ϣ2500ms//չʾʧ�ܴ���

                rhs.reset_game_screen_unshow();
                rhs.write_menu_bag();
                rhs.show_doublebuffer();
                return false;
            }
            return true;
        }
        else if(key==0xE0)
        {
            show_select_menu(rhs);
            switch (key=_getch())
            {
            case 72://ѡ����ǽ
                if (is_wall(rhs, coord_x, coord_y - 1))
                {
                    sign = key;
                    sentence = "UP";
                    show_select_menu(rhs); 
                    rhs.write_tip_sentence(22, sentence, color1);
                    rhs.show_doublebuffer();
                }
                else
                {
                    sign = 0;
                    sentence = "NULL";
                    show_select_menu(rhs);
                    rhs.show_doublebuffer();
                }
                break;
            case 75://ѡ����ǽ
                if (is_wall(rhs, coord_x-1, coord_y))
                {
                    sign = key;
                    show_select_menu(rhs);
                    sentence = "LEFT";
                    rhs.write_tip_sentence(22, sentence, color1);
                    rhs.show_doublebuffer();
                }
                else
                {
                    sign = 0;
                    sentence = "NULL";
                    show_select_menu(rhs);
                    rhs.show_doublebuffer();
                }
                break;
            case 77://ѡ����ǽ
                if (is_wall(rhs, coord_x +1 , coord_y))
                {
                    sign = key;
                    sentence = "RIGHT";
                    show_select_menu(rhs); 
                    rhs.write_tip_sentence(22, sentence, color1);
                    rhs.show_doublebuffer();
                }
                else
                {
                    sign = 0;
                    sentence = "NULL";
                    show_select_menu(rhs);
                    rhs.show_doublebuffer();
                }
                break;
            case 80://ѡ����ǽ
                if (is_wall(rhs, coord_x, coord_y+1))
                {
                    sign = key;
                    sentence = "DOWN";
                    show_select_menu(rhs);
                    rhs.write_tip_sentence(22, sentence, color1);
                    rhs.show_doublebuffer();
                }
                else
                {
                    sign = 0;
                    sentence = "NULL";
                    show_select_menu(rhs);
                    rhs.show_doublebuffer();
                }
                break;
            }
            show_select_menu(rhs);
            rhs.write_tip_sentence(22, sentence, color1);
            rhs.show_doublebuffer();
        }
    }
}


bool shovel::is_wall(control& rhs, int x, int y)
{
    if (x < 0 || y < 0 || x>29 || y>29)
        return false;
    if (rhs.map[y][x] == true)
        return true;
    else
        return false;
}
string shovel::information()
{
    return "��ǽ��";
}

void shovel::show_select_menu(control& rhs)
{
    WORD color0 = 0x06;
    WORD color1 = 0x04;
    rhs.write_tip_clear();
    string sentence = "��ǽ����ʹ��";
    rhs.write_tip_sentence(7, sentence, color0);
    sentence = "Esc ����ʹ��";
    rhs.write_tip_sentence(14, sentence, color1);
    sentence = "ʹ�� �� �� �� ��ѡ��һ��ǽ";
    rhs.write_tip_sentence(16, sentence, color1);
    sentence = "Enter ���ǽ";
    rhs.write_tip_sentence(18, sentence, color1);
    sentence = "The Wall to destroy:";
    rhs.write_tip_sentence(20, sentence, color1);
    sentence = "NULL";
    rhs.write_tip_sentence(22, sentence, color0);
}

void shovel::occur(control& rhs)
{
    WORD color = 0x05;
    //��ʾ��������ǽ����
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "�����������ǽ��";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "�򿪱������Խ���ʹ��";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//չʾ��ʾ��Ϣ2500ms
}

shovel::shovel(int x, int y)
{
    set_coord(x, y);
}




//������--------------------------------------����Ѫ��
bool moderator::use(control& rhs)
{
    int key = 0;
    show_select_menu(rhs);
    while (key != 13 && key != 27)
        key = _getch();
    if (key == 27)//����esc����
        return false;
    else if (key == 13)//����س���ȷ��
        rhs.player.blood_add(1100);
    return true;
}

moderator::moderator(int x, int y)
{
    set_coord(x, y);
}


string moderator::information()
{
    return "�ⶾ��";
}

void moderator::show_select_menu(control& rhs)
{
    WORD color0 = 0x06;
    WORD color1 = 0x04;
    rhs.write_tip_clear();
    string sentence = "�ⶾ���ķ���";
    rhs.write_tip_sentence(7, sentence, color0);

    sentence = "Esc ��������";
    rhs.write_tip_sentence(14, sentence, color1);
    sentence = "Enter ѡ����ú�ָ�20%Ѫ��";
    rhs.write_tip_sentence(16, sentence, color1);
    rhs.show_doublebuffer();
}
void moderator::occur(control& rhs)
{
    WORD color = 0x05;
    //��ʾ�����˽ⶾ��
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "��������˽ⶾ��";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "�򿪱������Խ��з���";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//չʾ��ʾ��Ϣ2500ms
}



//ʥ��----------------------------------------�ı�����ĵȼ�
holyLight::holyLight(int x, int y)
{
    set_coord(x, y);
}


bool holyLight::use(control& rhs)
{
    int key = 0;
    show_select_menu(rhs);
    while (key != 13 && key != 27)
        key = _getch();
    if (key == 27)//����esc����
        return false;
    else if (key == 13)//����س���ȷ��
        rhs.set_foggy_level(false);
    return true;
}


string holyLight::information()
{
    return "ʥ��";
}


//��������
void holyLight::occur(control& rhs)
{
    WORD color = 0x05;
    //��ʾ������ʥ��
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "��������ԡ����ʥ��";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "�򿪱������Խ����ͷ�ʥ�������";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//չʾ��ʾ��Ϣ2500ms
}

void holyLight::show_select_menu(control& rhs)
{
    WORD color0 = 0x06;
    WORD color1 = 0x04;
    rhs.write_tip_clear();
    string sentence = "ʥ����ͷ�";
    rhs.write_tip_sentence(7, sentence, color0);

    sentence = "Esc �����ͷ�";
    rhs.write_tip_sentence(14, sentence, color1);
    sentence = "Enter ѡ���ͷž�������";
    rhs.write_tip_sentence(16, sentence, color1);
    sentence = "�����ֺ�ɫת��Ϊ��ɫ";
    rhs.write_tip_sentence(18, sentence, color1);
    rhs.show_doublebuffer();
}