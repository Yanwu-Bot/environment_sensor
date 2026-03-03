// menu.h
#ifndef __MENU_H
#define __MENU_H

#include <stdint.h>
#include "Delay.h"  // 添加Delay头文件

#define DISPLAY_ITEMS    4    // 每页显示4项
typedef void (*MenuAction)(void);  // 增加：功能回调函数类型
// 菜单项结构
typedef struct MenuItem {
    uint8_t id;                 // 菜单项ID
    char text[16];              // 显示文本
    struct MenuItem *parent;    // 父菜单
    struct MenuItem *child;     // 子菜单
    struct MenuItem *next;      // 下一个兄弟
    struct MenuItem *prev;      // 上一个兄弟
    MenuAction action;  // 新增：功能回调函数指针
} MenuItem;

// 菜单管理器
typedef struct {
    MenuItem *current_menu;     // 当前所在菜单
    MenuItem *current_item;     // 当前选中项
    MenuItem *display_items[DISPLAY_ITEMS]; // 当前显示项
    uint8_t cursor_pos;         // 光标位置 (0-3)
    uint8_t item_count;         // 当前菜单项数
} MenuManager;

// 函数声明
void Menu_Init(void);
void Menu_Display(void);
void Menu_Up(void);
void Menu_Down(void);
void Menu_Enter(void);
void Menu_Back(void);

extern uint32_t System_Run_Time;    // 系统运行时间（秒）
extern uint32_t Alarm_Count;        // 报警次数
extern uint8_t Bluetooth_Connected; // 蓝牙连接状态


extern MenuManager menu;

#endif
