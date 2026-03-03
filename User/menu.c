#include "menu.h"
#include "OLED.h"
#include <string.h>
#include "Action.h"  // 添加Action头文件

MenuManager menu;
typedef void (*MenuAction)(void);

// 静态函数声明
static MenuItem* create_item(uint8_t id, const char* text, MenuAction action);
static void link_siblings(MenuItem* items[], uint8_t count);
static void set_child(MenuItem* parent, MenuItem* child);
static void update_display_items(void);

// 创建菜单项
static MenuItem* create_item(uint8_t id, const char* text, MenuAction action)
{
    static MenuItem items[30];
    static uint8_t index = 0;
    
    if (index >= 30) return NULL;
    
    MenuItem* item = &items[index++];
    item->id = id;
    
    // 复制文本
    uint8_t i = 0;
    while (text[i] != '\0' && i < 15) {
        item->text[i] = text[i];
        i++;
    }
    item->text[i] = '\0';
    
    item->parent = NULL;
    item->child = NULL;
    item->next = NULL;
    item->prev = NULL;
    item->action = action;
    
    return item;
}

// 链接兄弟菜单项
static void link_siblings(MenuItem* items[], uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        if (i > 0) {
            items[i]->prev = items[i-1];
        }
        if (i < count - 1) {
            items[i]->next = items[i+1];
        }
    }
}

// 设置子菜单
static void set_child(MenuItem* parent, MenuItem* child)
{
    if (!parent || !child) return;
    parent->child = child;
    child->parent = parent;
}

// 菜单初始化 
void Menu_Init(void)
{
    // 1. 创建主菜单项（4个）
    MenuItem* main_items[4];
    main_items[0] = create_item(1, "View Data", NULL);        // 查看数据（有子菜单）
    main_items[1] = create_item(2, "Control Fan", NULL);      // 控制风扇（有子菜单）
    main_items[2] = create_item(3, "View Threshold", NULL);   // 查看阈值（有子菜单）- 改名
    main_items[3] = create_item(4, "System Status", NULL);    // 系统状态（有子菜单）
    
    // 2. 链接主菜单项
    link_siblings(main_items, 4);
    
    // 3. 创建子菜单项
    
    // 3.1 查看数据子菜单 - 每个子菜单项对应一个功能函数
    MenuItem* data_items[4];
    data_items[0] = create_item(1, "Light", Light_View);           // 光照查看
    data_items[1] = create_item(2, "Temp&Hum", TempHum_View);      // 温湿度查看
    data_items[2] = create_item(3, "Gas", Gas_View);               // 气体查看
    data_items[3] = create_item(4, "<- Back", NULL);               // 返回
    link_siblings(data_items, 4);
    set_child(main_items[0], data_items[0]);  // 主菜单1有子菜单
    
    // 3.2 风扇控制子菜单 - 直接操作
    MenuItem* fan_items[4];
    fan_items[0] = create_item(1, "Fan ON", Fan_ON);            // 打开风扇
    fan_items[1] = create_item(2, "Fan OFF", Fan_OFF);          // 关闭风扇
    fan_items[2] = create_item(3, "Auto Mode", Fan_Auto);       // 自动模式
    fan_items[3] = create_item(4, "<- Back", NULL);             // 返回
    link_siblings(fan_items, 4);
    set_child(main_items[1], fan_items[0]);  // 主菜单2有子菜单
    
    // 3.3 查看阈值子菜单 - 仅查看，无设置功能
    MenuItem* thresh_items[3];
    thresh_items[0] = create_item(1, "Gas Threshold", Gas_Threshold_View);     // 气体阈值查看
    thresh_items[1] = create_item(2, "Temp Threshold", Temp_Threshold_View);   // 温度阈值查看
    thresh_items[2] = create_item(3, "<- Back", NULL);                         // 返回主菜单
    link_siblings(thresh_items, 3);
    set_child(main_items[2], thresh_items[0]);  // 主菜单3有子菜单
    
    // 3.4 系统状态子菜单 - 信息显示
    MenuItem* status_items[4];
    status_items[0] = create_item(1, "Uptime", Uptime_View);           // 运行时间查看
    status_items[1] = create_item(2, "Alarms", Alarms_View);           // 报警次数查看
    status_items[2] = create_item(3, "Bluetooth", Bluetooth_Status);   // 蓝牙状态查看
    status_items[3] = create_item(4, "<- Back", NULL);                 // 返回主菜单
    link_siblings(status_items, 4);
    set_child(main_items[3], status_items[0]);  // 主菜单4有子菜单
    
    // 4. 初始化菜单管理器
    menu.current_menu = NULL;           // 当前在主菜单
    menu.current_item = main_items[0];  // 当前选中第一个主菜单项
    menu.cursor_pos = 0;                // 光标在第0行
    
    // 5. 更新显示
    update_display_items();
}

// 更新显示项数组
static void update_display_items(void)
{
    MenuItem* start_item = NULL;
    
    // 确定从哪个菜单项开始显示
    if (menu.current_menu) {
        // 在子菜单中：从子菜单的第一个子项开始
        start_item = menu.current_menu->child;
    } else {
        // 在主菜单中：从当前选中项开始，往前找到第一个
        start_item = menu.current_item;
        while (start_item && start_item->prev) {
            start_item = start_item->prev;
        }
    }
    
    // 清空显示数组
    for (uint8_t i = 0; i < DISPLAY_ITEMS; i++) {
        menu.display_items[i] = NULL;
    }
    menu.item_count = 0;
    
    // 填充显示数组（最多4项）
    MenuItem* current = start_item;
    for (uint8_t i = 0; i < DISPLAY_ITEMS && current != NULL; i++) {
        menu.display_items[i] = current;
        menu.item_count++;
        current = current->next;
    }
}

// 显示菜单
void Menu_Display(void)
{
    OLED_Clear();
    
    // 显示4行菜单项
    for (uint8_t i = 0; i < menu.item_count; i++) {
        if (menu.display_items[i]) {
            char line[17];
            
            // 构建显示内容
            if (i == menu.cursor_pos) {
                line[0] = '>';
                line[1] = ' ';
            } else {
                line[0] = ' ';
                line[1] = ' ';
            }
            
            // 复制菜单文本
            uint8_t j;
            for (j = 0; menu.display_items[i]->text[j] != '\0' && j < 14; j++) {
                line[j + 2] = menu.display_items[i]->text[j];
            }
            line[j + 2] = '\0';
            
            OLED_ShowString(i + 1, 1, line);
        }
    }
}

// 上移光标
void Menu_Up(void)
{
    if (menu.cursor_pos > 0) {
        menu.cursor_pos--;
        Menu_Display();
    } else {
        MenuItem* first = menu.display_items[0];
        if (first && first->prev) {
            update_display_items();
            menu.cursor_pos = 0;
            Menu_Display();
        }
    }
}

// 下移光标
void Menu_Down(void)
{
    if (menu.cursor_pos < menu.item_count - 1 && menu.item_count > 0) {
        menu.cursor_pos++;
        Menu_Display();
    } else {
        MenuItem* last = menu.display_items[menu.item_count - 1];
        if (last && last->next) {
            menu.current_item = last->next;
            update_display_items();
            menu.cursor_pos = menu.item_count - 1;
            Menu_Display();
        }
    }
}

// 进入菜单
void Menu_Enter(void)
{
    if (menu.item_count == 0) return;
    
    MenuItem* selected = menu.display_items[menu.cursor_pos];
    if (!selected) return;
    
    // 检查是否有功能回调
    if (selected->action != NULL) {
        selected->action();  // 执行对应的功能函数
        return;
    }
    
    // 检查是否是"返回"项
    if (strstr(selected->text, "<- Back") != NULL) {
        Menu_Back();
        return;
    }
    
    // 检查是否有子菜单
    if (selected->child) {
        // 进入子菜单
        menu.current_menu = selected;
        menu.current_item = selected->child;
        menu.cursor_pos = 0;
        update_display_items();
        Menu_Display();
    } else {
        // 没有子菜单也没有功能，显示提示
        OLED_Clear();
        OLED_ShowString(2, 1, "View Only");
        OLED_ShowString(3, 1, selected->text);
        Delay_ms(1000);
        Menu_Display();
    }
}

// 返回上一级
void Menu_Back(void)
{
    if (menu.current_menu) {
        MenuItem* parent_menu = menu.current_menu;
        
        menu.current_menu = menu.current_menu->parent;
        
        if (menu.current_menu) {
            menu.current_item = menu.current_menu->child;
        } else {
            menu.current_item = parent_menu;
            while (menu.current_item && menu.current_item->prev) {
                menu.current_item = menu.current_item->prev;
            }
        }
        
        menu.cursor_pos = 0;
        update_display_items();
        Menu_Display();
    }
}