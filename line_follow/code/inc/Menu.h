//
// Created by Taseny on 25-4-11.
//

#ifndef MENU_H
#define MENU_H
#include "zf_common_font.hpp"
#include "zf_device_ips200_fb.hpp"

#ifdef __cplusplus
extern "C" {
#include <stdint.h>
#endif

#define DISPLAY_LINE_HEIGHT 18U
#define MAX_DISPLAY_ITEM 20U
#define MAX_DISPLAY_CHAR 27U

#ifdef __cplusplus
}
#endif

/**
 * Display Function Relink Zone
 */
// #define displayString(x, y, str, bgcolor, pencolor) \
// 		ips200_show_string( ( x ) , ( y ) , ( str ), (bgcolor), (pencolor))
//
// #define displayImage(x, y, ptrImage, width, height) \
// 		ips200_show_gray_image( ( x ) , ( y ) , ( ptrImage ) , ( width ) , ( height ))
//
// #define screenFull() \
// 		ips200_full(RGB565_BLACK)
//
// #define screenClear() \
// 		ips200_clear()
// /**
//  * Display Function Relink Zone
//  */

#ifdef __cplusplus

#include <cstring>
#include <string>

// 应用程序任务回调函数类型定义
typedef void (*AppTaskCallback)(void* param);

// 任务退出通知回调函数类型定义
typedef void (*TaskExitCallback)(void* param);

enum class menuItemType
{
	NORMAL,
	CHANGEABLE,
	TOGGLE
};

class menuItem
{
	public:
		const char*  name;
		menuItem*    parent;
		menuItem**   children;
		uint8_t      childrenCount;
		uint8_t      savedSelectedIndex    = 0;
		uint8_t      savedFirstVisibleItem = 0;
		AppTaskCallback appEntry;
		void*        appArgs;
		bool         appRunning;         // 指示应用是否运行中
		TaskExitCallback appExitCallback; // 应用退出回调函数

		menuItem(const char* name, menuItem** children, uint8_t count, void (*entry)(void*), void* args = nullptr);

		void refresh()
		{
			if (type == menuItemType::TOGGLE && itemData.toggle.ref)
			{
				itemData.toggle.state = *itemData.toggle.ref;
			}
			else if (type == menuItemType::CHANGEABLE && itemData.changeable.ref) {
				float refValue = *itemData.changeable.ref;
				// 确保引用值在有效范围内
				if (refValue < itemData.changeable.min) {
					refValue = itemData.changeable.min;
					*itemData.changeable.ref = refValue;
				}
				else if (refValue > itemData.changeable.max) {
					refValue = itemData.changeable.max;
					*itemData.changeable.ref = refValue;
				}
				itemData.changeable.currentValue = refValue;
			}
		};
		menuItemType type = menuItemType::NORMAL;

		union
		{
			struct
			{
				bool   is_locked;
				float  currentValue;
				float  min;
				float  max;
				float  step;
				float* ref;
				void (*onChange)(float);
			} changeable;

			struct
			{
				bool   is_locked;
				bool   state;
				bool   *ref;
				void (*onToggle)(bool);
			} toggle;
		} itemData;

		static menuItem* createChangeable(
				const char* name,
				const float initialValue,
				const float min,
				const float max,
				const float step,
				float&      ref,
				void (*     onChange)(float) = nullptr
				)
		{
			if (name == nullptr) {
				name = "Unnamed";
			}

			// 确保参数合理
			float actualMin = min;
			float actualMax = max;
			float actualStep = step > 0 ? step : 0.1f;
			float actualInitialValue = initialValue;

			// 确保min < max
			if (actualMin > actualMax) {
				float temp = actualMin;
				actualMin = actualMax;
				actualMax = temp;
			}

			// 确保initialValue在有效范围内
			if (actualInitialValue < actualMin) {
				actualInitialValue = actualMin;
			} else if (actualInitialValue > actualMax) {
				actualInitialValue = actualMax;
			}

			// 初始化引用值
			ref = actualInitialValue;

			auto* item                             = new menuItem(name, nullptr, 0, nullptr);
			item->type                             = menuItemType::CHANGEABLE;
			item->itemData.changeable.is_locked    = false;
			item->itemData.changeable.currentValue = actualInitialValue;
			item->itemData.changeable.min          = actualMin;
			item->itemData.changeable.max          = actualMax;
			item->itemData.changeable.step         = actualStep;
			item->itemData.changeable.ref          = &ref;
			item->itemData.changeable.onChange     = onChange;
			return item;
		}

		static menuItem* createToggle(
				const char* name,
				const bool  initialState,
				bool&       ref,
				void (*     onToggle)(bool) = nullptr
				)
		{
			if (name == nullptr) {
				name = "Unnamed";
			}

			// 初始化引用值
			ref = initialState;

			auto* item                      = new menuItem(name, nullptr, 0, nullptr);
			item->type                      = menuItemType::TOGGLE;
			item->itemData.toggle.is_locked = false;
			item->itemData.toggle.state     = initialState;
			item->itemData.toggle.ref       = &ref;
			item->itemData.toggle.onToggle  = onToggle;
			return item;
		}
};

// 菜单导航器类
class menuNavigator {
public:
	explicit menuNavigator(zf_device_ips200& display);
	~menuNavigator() = default;

	// 禁止拷贝
	menuNavigator(const menuNavigator&) = delete;
	menuNavigator& operator=(const menuNavigator&) = delete;

	// 设置根菜单
	void setRootMenu(std::unique_ptr<menuItem> root);

	// 处理输入
	void handleInput(uint8_t key);

	// 显示更新
	void refreshDisplay();
	void updateDisplay();

	// 获取显示信息
	void getDisplayInfo(char* buffer, uint8_t lineCount);

private:
	zf_device_ips200& display;  // ✅ 显示屏对象引用
	menuItem* currentMenu = nullptr;
	std::unique_ptr<menuItem> rootMenu;

	bool inAppMode = false;
	uint8_t selectedIndex = 0;
	uint8_t firstVisibleItem = 0;
	uint8_t appSavedSelectedIndex = 0;

	char displayBuffer[MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM] = {0};

	// ✅ 内联方法：直接调用 display 对象的方法
	void displayString(uint16_t x, uint16_t y, const char* str, uint16_t bgcolor, uint16_t pencolor) {
		display.show_string(x, y, str);
	}

	void displayImage(uint16_t x, uint16_t y, const uint8_t* ptrImage, uint16_t width, uint16_t height) {
		display.show_gray_image(x, y, ptrImage, width, height, width, height, 0);
	}

	void screenFull() {
		display.full(RGB565_BLACK);
	}

	void screenClear() {
		display.clear();
	}
};


#endif

#endif //MENU_H
