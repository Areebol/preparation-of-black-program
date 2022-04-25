//file: debuff.cpp

#include"debuff.h"
#include<stdexcept>
#include<string>
using std::string;
using std::out_of_range;

//检查是否与character相遇
bool debuff_base::operator==(character& rhs)
{
    COORD coord_player = rhs.get_coord();
    if (coord_player.X == this->coord_x && coord_player.Y == this->coord_y)
        return true;
    else
        return false;
}

//设置坐标位置
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




//传送阵--------------------------------------------------------------------------------------------
//传送阵的构造函数
teleportationArray::teleportationArray(int x, int y)
{
    set_coord(x, y);
    srand(unsigned(time(0)));//设置随机数
}


//debuff的生效环节
void teleportationArray::occur(control& rhs)
{
    WORD color = 0x05;
    int x = rand() % 30;
    int y = rand() % 30;
    while (rhs.map[y][x])//选择一块空地
    {
        x = rand() % 30;
        y = rand() % 30;
    }
    rhs.set_player_coord(x, y);
    //提示遇到了传送阵
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "你遇到了传送阵";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "将被随机传送到地图的某一个角落";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//展示提示信息2500ms
}

//打印道具的形状
void teleportationArray::print(control& rhs)
{
    rhs.output_buffer(2 * coord_x + 2, coord_y + 3, 0x55, '&');
    rhs.output_buffer(2 * coord_x + 3, coord_y + 3, 0x55, '&');
}

//晕晕蘑菇-----------------------------------------------------------------------------------------

stunMushrooms::stunMushrooms(int x, int y)
{
    set_coord(x, y);
}

void stunMushrooms::occur(control& rhs)
{
    WORD color = 0x06;
    rhs.set_player_vision((rhs.player_vision_l - 1) * 2 + 1);
    //提示吃到了蘑菇
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "你误食了晕晕蘑菇";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "你所能看到的视野变小了";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//展示提示信息2500ms
}

void stunMushrooms::print(control& rhs)
{
    rhs.output_buffer(2 * coord_x + 2, coord_y + 3, 0x55, '&');
    rhs.output_buffer(2 * coord_x + 3, coord_y + 3, 0x55, '&');
}


//潘多拉魔盒-----------------------------------------------------------------------------------------
pandoraBox::pandoraBox(int x, int y)
{
    set_coord(x, y);
}


void pandoraBox::occur(control& rhs)
{
    WORD color = 0x06;
    rhs.set_foggy_level(true);
    //打开了魔盒
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "你打开了潘多拉魔盒";
    rhs.write_map_sentence(13, sentence, color);
    sentence = "迷雾将由白色变成血色迷雾";
    rhs.write_map_sentence(15, sentence, color);
    sentence = "腐蚀灵魂的速度更快了";
    rhs.write_map_sentence(17, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//展示提示信息2500ms
}

void pandoraBox::print(control& rhs)
{
    rhs.output_buffer(2 * coord_x + 2, coord_y + 3, 0x55, '&');
    rhs.output_buffer(2 * coord_x + 3, coord_y + 3, 0x55, '&');
}


//斯芬克斯之问-----------------------------------------------------------------------------------------
sphinxAsk::sphinxAsk(int x, int y)
{
    srand(unsigned(time(0)));//设置随机数
    set_coord(x, y);
}

void sphinxAsk::occur(control& rhs)
{
    WORD color = 0x06;
    int x = rand() % 30;
    int y = rand() % 30;
    while (rhs.map[y][x])//选择一块空地
    {
        x = rand() % 30;
        y = rand() % 30;
    }
    rhs.set_end(x, y);//改变终点
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "聆听斯芬克斯之问";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "迷雾中的出口被改变了";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//展示提示信息2500ms
}

void sphinxAsk::print(control& rhs)
{
    rhs.output_buffer(2 * coord_x + 2, coord_y + 3, 0x55, '&');
    rhs.output_buffer(2 * coord_x + 3, coord_y + 3, 0x55, '&');
}
