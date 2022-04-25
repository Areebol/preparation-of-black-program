//file: debuff.cpp

#include"debuff.h"
#include<stdexcept>
#include<string>
using std::string;
using std::out_of_range;

//����Ƿ���character����
bool debuff_base::operator==(character& rhs)
{
    COORD coord_player = rhs.get_coord();
    if (coord_player.X == this->coord_x && coord_player.Y == this->coord_y)
        return true;
    else
        return false;
}

//��������λ��
void debuff_base::set_coord(int x, int y)
{
    if (x < 0 || y < 0 || x>29 || y>29)
        throw out_of_range("coord is default");
    coord_x = x;
    coord_y = y;
}

void debuff_base::print(control& rhs)
{
    rhs.output_buffer(2 * coord_x + 2, coord_y + 3, 0x55, '&');
    rhs.output_buffer(2 * coord_x + 3, coord_y + 3, 0x55, '&');
}




//������--------------------------------------------------------------------------------------------
//������Ĺ��캯��
teleportationArray::teleportationArray(int x, int y)
{
    set_coord(x, y);
    srand(unsigned(time(0)));//���������
}


//debuff����Ч����
void teleportationArray::occur(control& rhs)
{
    WORD color = 0x05;
    int x = rand() % 30;
    int y = rand() % 30;
    while (rhs.map[y][x])//ѡ��һ��յ�
    {
        x = rand() % 30;
        y = rand() % 30;
    }
    rhs.set_player_coord(x, y);
    //��ʾ�����˴�����
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "�������˴�����";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "����������͵���ͼ��ĳһ������";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//չʾ��ʾ��Ϣ2500ms
}

//��ӡ���ߵ���״
void teleportationArray::print(control& rhs)
{
    rhs.output_buffer(2 * coord_x + 2, coord_y + 3, 0x55, '&');
    rhs.output_buffer(2 * coord_x + 3, coord_y + 3, 0x55, '&');
}

//����Ģ��-----------------------------------------------------------------------------------------

stunMushrooms::stunMushrooms(int x, int y)
{
    set_coord(x, y);
}

void stunMushrooms::occur(control& rhs)
{
    WORD color = 0x06;
    rhs.set_player_vision((rhs.player_vision_l - 1) * 2 + 1);
    //��ʾ�Ե���Ģ��
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "����ʳ������Ģ��";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "�����ܿ�������Ұ��С��";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//չʾ��ʾ��Ϣ2500ms
}

void stunMushrooms::print(control& rhs)
{
    rhs.output_buffer(2 * coord_x + 2, coord_y + 3, 0x55, '&');
    rhs.output_buffer(2 * coord_x + 3, coord_y + 3, 0x55, '&');
}


//�˶���ħ��-----------------------------------------------------------------------------------------
pandoraBox::pandoraBox(int x, int y)
{
    set_coord(x, y);
}


void pandoraBox::occur(control& rhs)
{
    WORD color = 0x06;
    rhs.set_foggy_level(true);
    //����ħ��
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "������˶���ħ��";
    rhs.write_map_sentence(13, sentence, color);
    sentence = "�����ɰ�ɫ���Ѫɫ����";
    rhs.write_map_sentence(15, sentence, color);
    sentence = "��ʴ�����ٶȸ�����";
    rhs.write_map_sentence(17, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//չʾ��ʾ��Ϣ2500ms
}

void pandoraBox::print(control& rhs)
{
    rhs.output_buffer(2 * coord_x + 2, coord_y + 3, 0x55, '&');
    rhs.output_buffer(2 * coord_x + 3, coord_y + 3, 0x55, '&');
}


//˹�ҿ�˹֮��-----------------------------------------------------------------------------------------
sphinxAsk::sphinxAsk(int x, int y)
{
    srand(unsigned(time(0)));//���������
    set_coord(x, y);
}

void sphinxAsk::occur(control& rhs)
{
    WORD color = 0x06;
    int x = rand() % 30;
    int y = rand() % 30;
    while (rhs.map[y][x])//ѡ��һ��յ�
    {
        x = rand() % 30;
        y = rand() % 30;
    }
    rhs.set_end(x, y);//�ı��յ�
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "����˹�ҿ�˹֮��";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "�����еĳ��ڱ��ı���";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//չʾ��ʾ��Ϣ2500ms
}

void sphinxAsk::print(control& rhs)
{
    rhs.output_buffer(2 * coord_x + 2, coord_y + 3, 0x55, '&');
    rhs.output_buffer(2 * coord_x + 3, coord_y + 3, 0x55, '&');
}
