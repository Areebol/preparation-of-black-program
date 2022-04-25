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
//设置坐标位置
void buff_base::set_coord(int x, int y)
{
    if (x < 0 || y < 0 || x>29 || y>29)
        throw out_of_range("coord is default");
    coord_x = x;
    coord_y = y;
}

//检查是否与character相遇
bool buff_base::operator==(character& rhs)
{
    COORD coord_player = rhs.get_coord();
    if (coord_player.X == this->coord_x && coord_player.Y == this->coord_y)
        return true;
    else
        return false;
}




//火把类--------------------------------------照亮视野
bool torch::use(control& rhs)
{
    int key=0;
    show_select_menu(rhs);
    while(key!=13&&key!=27)
        key = _getch();
    if (key == 27)//输入esc返回
        return false;
    else if (key == 13)//输入回车键确认
        rhs.set_player_vision((rhs.player_vision_l + 1) * 2 + 1);
    return true;
}

//返回道具信息
string torch::information()
{
    return "探照灯";
}

//初始化函数
torch::torch(int x, int y)
{
    set_coord(x, y);
}

//触发提醒
void torch::occur(control& rhs)
{
    WORD color = 0x05;
    //提示遇到了探照灯
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "你意外捡到了探照灯";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "打开背包可以进行使用";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//展示提示信息2500ms
}

void torch::show_select_menu(control& rhs)
{
    WORD color0 = 0x06;
    WORD color1 = 0x04;
    rhs.write_tip_clear();
    string sentence = "探照灯的使用";
    rhs.write_tip_sentence(7, sentence, color0);

    sentence = "Esc 放弃使用";
    rhs.write_tip_sentence(14, sentence, color1);
    sentence = "Enter 选择使用扩大当前视野";
    rhs.write_tip_sentence(16, sentence, color1);
    rhs.show_doublebuffer();
}


//水晶球类------------------------------------照亮部分视野

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
    return "水晶球";
}





//铲子类--------------------------------------开墙
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
        if (key == 27)//输入esc返回
            return false;
        else if (key == 13)//输入回车键确认
        {
            switch (sign)
            {
            case 72://选择上墙
                rhs.set_map_dot(coord_x, coord_y - 1, false);
                break;
            case 75://选择左墙
                rhs.set_map_dot(coord_x - 1, coord_y, false);
                break;
            case 77://选择右墙
                rhs.set_map_dot(coord_x + 1, coord_y, false);
                break;
            case 80://选择下墙
                rhs.set_map_dot(coord_x, coord_y + 1, false);
                break;
            default:
                WORD color = 0x05;
                //提示遇到了探照灯
                rhs.write_map_clear();
                rhs.write_tip_clear();
                rhs.write_interface();
                string sentence = "使用失败";
                rhs.write_map_sentence(14, sentence, color);
                sentence = "你没有选择区域或选择区域没有墙壁";
                rhs.write_map_sentence(16, sentence, color);
                rhs.show_doublebuffer();
                Sleep(2500);//展示提示信息2500ms//展示失败错误

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
            case 72://选择上墙
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
            case 75://选择左墙
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
            case 77://选择右墙
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
            case 80://选择下墙
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
    return "破墙锤";
}

void shovel::show_select_menu(control& rhs)
{
    WORD color0 = 0x06;
    WORD color1 = 0x04;
    rhs.write_tip_clear();
    string sentence = "破墙锤的使用";
    rhs.write_tip_sentence(7, sentence, color0);
    sentence = "Esc 放弃使用";
    rhs.write_tip_sentence(14, sentence, color1);
    sentence = "使用 ← ↑ → ↓选择一面墙";
    rhs.write_tip_sentence(16, sentence, color1);
    sentence = "Enter 拆除墙";
    rhs.write_tip_sentence(18, sentence, color1);
    sentence = "The Wall to destroy:";
    rhs.write_tip_sentence(20, sentence, color1);
    sentence = "NULL";
    rhs.write_tip_sentence(22, sentence, color0);
}

void shovel::occur(control& rhs)
{
    WORD color = 0x05;
    //提示遇到了破墙锤子
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "你意外捡到了破墙锤";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "打开背包可以进行使用";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//展示提示信息2500ms
}

shovel::shovel(int x, int y)
{
    set_coord(x, y);
}




//缓毒剂--------------------------------------增加血量
bool moderator::use(control& rhs)
{
    int key = 0;
    show_select_menu(rhs);
    while (key != 13 && key != 27)
        key = _getch();
    if (key == 27)//输入esc返回
        return false;
    else if (key == 13)//输入回车键确认
        rhs.player.blood_add(1100);
    return true;
}

moderator::moderator(int x, int y)
{
    set_coord(x, y);
}


string moderator::information()
{
    return "解毒剂";
}

void moderator::show_select_menu(control& rhs)
{
    WORD color0 = 0x06;
    WORD color1 = 0x04;
    rhs.write_tip_clear();
    string sentence = "解毒剂的服用";
    rhs.write_tip_sentence(7, sentence, color0);

    sentence = "Esc 放弃服用";
    rhs.write_tip_sentence(14, sentence, color1);
    sentence = "Enter 选择服用后恢复20%血量";
    rhs.write_tip_sentence(16, sentence, color1);
    rhs.show_doublebuffer();
}
void moderator::occur(control& rhs)
{
    WORD color = 0x05;
    //提示遇到了解毒剂
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "你意外捡到了解毒剂";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "打开背包可以进行服用";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//展示提示信息2500ms
}



//圣光----------------------------------------改变迷雾的等级
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
    if (key == 27)//输入esc返回
        return false;
    else if (key == 13)//输入回车键确认
        rhs.set_foggy_level(false);
    return true;
}


string holyLight::information()
{
    return "圣光";
}


//触发提醒
void holyLight::occur(control& rhs)
{
    WORD color = 0x05;
    //提示遇到了圣光
    rhs.write_map_clear();
    rhs.write_interface();
    string sentence = "你意外沐浴到了圣光";
    rhs.write_map_sentence(14, sentence, color);
    sentence = "打开背包可以进行释放圣光的力量";
    rhs.write_map_sentence(16, sentence, color);
    rhs.show_doublebuffer();
    Sleep(2500);//展示提示信息2500ms
}

void holyLight::show_select_menu(control& rhs)
{
    WORD color0 = 0x06;
    WORD color1 = 0x04;
    rhs.write_tip_clear();
    string sentence = "圣光的释放";
    rhs.write_tip_sentence(7, sentence, color0);

    sentence = "Esc 放弃释放";
    rhs.write_tip_sentence(14, sentence, color1);
    sentence = "Enter 选择释放净化迷雾";
    rhs.write_tip_sentence(16, sentence, color1);
    sentence = "迷雾将又红色转化为白色";
    rhs.write_tip_sentence(18, sentence, color1);
    rhs.show_doublebuffer();
}