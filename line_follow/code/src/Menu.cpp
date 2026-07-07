//
// Created by Taseny on 25-4-11.
//

#include "../inc/Menu.h"
#include "../inc/key.hpp"

extern menuNavigator* Menu;

float systemValue = 0;
extern float p_l;
extern float i_l;
extern float d_l;
extern float p_r;
extern float i_r;
extern float d_r;
extern float v_target;
extern float p_position;
extern float d_position;
extern float duty_left_press;
extern float duty_right_press;
extern float overspeed;

zf_device_ips200 ips200;

void vPrintf(void *arg)
{
	ips200.full(RGB565_PINK);
	printf("Hello Wolrd! value = %.2f\r\n", systemValue);
	Menu->refreshDisplay();
}

menuItem* pidItems[] = {
	menuItem::createChangeable("v_target", v_target, 0, 1000, 10, v_target, [](const float value){ v_target = value; }),
	menuItem::createChangeable("p_position", p_position, 0, 50, 0.1, p_position, [](const float value){ p_position = value; }),
	menuItem::createChangeable("d_position", d_position, 0, 50, 0.1, d_position, [](const float value){ d_position = value; }),
	menuItem::createChangeable("duty_l_press", duty_left_press, 500, 1000, 10, duty_left_press, [](const float value){ duty_left_press = value; }),
	menuItem::createChangeable("duty_r_press", duty_right_press, 500, 1000, 10, duty_right_press, [](const float value){ duty_right_press = value; }),
	menuItem::createChangeable("overspeed", overspeed, 50, 500, 5, overspeed, [](const float value){ overspeed = value; }),
};

menuItem* appItems[] = {
	menuItem::createChangeable("System Value", systemValue, 0, 10, 0.01, systemValue, [](const float value){ systemValue = value; }),
	new menuItem("Hello World", nullptr, 0, vPrintf),
};

menuItem* mainItems[] = {
    new menuItem("PID", pidItems, 6, nullptr),
    new menuItem("App", appItems, 2, nullptr),
};

menuItem mainMenu("mainMenu", mainItems, 2, nullptr);

#ifdef __cplusplus

/**
 * menuItem 类实现
 */
menuItem::menuItem(const char* name, menuItem** children, uint8_t count, void (*entry)(void*), void* args)
    : name(name), children(children), childrenCount(count), appEntry(entry), appArgs(args),
      appRunning(false), appExitCallback(nullptr) {
    if (children)
    {
        for (uint8_t i = 0; i < childrenCount; i++)
        {
            children[i]->parent = this;
        }
    }
}

/**
 * menuNavigator 类实现
 */
menuNavigator::menuNavigator(zf_device_ips200& display)
	: display(display), currentMenu(&mainMenu), inAppMode(false) {}

void menuNavigator::handleInput(uint8_t key) {
    if (inAppMode) {
        if (key == KEY_LEFT) {
            inAppMode = false;
            selectedIndex = appSavedSelectedIndex;

            // 确保索引有效
            if (selectedIndex < currentMenu->childrenCount && currentMenu->children != nullptr) {
                if (currentMenu->children[selectedIndex]->appExitCallback) {
                    currentMenu->children[selectedIndex]->appExitCallback(currentMenu->children[selectedIndex]->appArgs);
                }

                currentMenu->children[selectedIndex]->appRunning = false;
            }

            updateDisplay();
            return;
        }

        if (selectedIndex < currentMenu->childrenCount &&
            currentMenu->children != nullptr &&
            currentMenu->children[selectedIndex]->appRunning &&
            currentMenu->children[selectedIndex]->appEntry) {
            // 这里应该传递按键到应用，但我们没有定义这个接口，所以只是再次调用应用
            currentMenu->children[selectedIndex]->appEntry(currentMenu->children[selectedIndex]->appArgs);
        }
        return;
    }

	menuItem* currentItem = currentMenu->children[selectedIndex];

    // 菜单导航模式
    switch (key) {
        case KEY_UP: // 上键
	        if (currentItem->type == menuItemType::CHANGEABLE && currentItem->itemData.changeable.is_locked)
	        {
		        auto& data = currentItem->itemData.changeable;
		        float newValue = data.currentValue + data.step;
		        if (newValue <= data.max)
		        {
			        data.currentValue = newValue;
			        if (data.onChange)
				        data.onChange(newValue);
		        }
	        }
	        else if (currentItem->type == menuItemType::TOGGLE && currentItem->itemData.toggle.is_locked)
	        {
		        auto& data = currentItem->itemData.toggle;
		        data.state = !data.state;
		        if (data.onToggle)
			        data.onToggle(data.state);
	        }
	        else
	        {
		        selectedIndex = (selectedIndex == 0) ? (currentMenu->childrenCount - 1) : (selectedIndex - 1);
		        if (selectedIndex == currentMenu->childrenCount - 1)
		        {
			        if (currentMenu->childrenCount > MAX_DISPLAY_ITEM)
				        screenFull();
			        firstVisibleItem = selectedIndex - (selectedIndex % MAX_DISPLAY_ITEM);
		        }
	        }
	        break;

        case KEY_DOWN: // 下键
	        if (currentItem->type == menuItemType::CHANGEABLE && currentItem->itemData.changeable.is_locked)
	        {
		        auto& data = currentItem->itemData.changeable;
		        float newValue = data.currentValue - data.step;
		        if (newValue >= data.min)
		        {
			        data.currentValue = newValue;
			        if (data.onChange)
				        data.onChange(newValue);
		        }
	        }
	        else if (currentItem->type == menuItemType::TOGGLE && currentItem->itemData.toggle.is_locked)
	        {
		        auto& data = currentItem->itemData.toggle;
		        data.state = !data.state;
		        if (data.onToggle)
			        data.onToggle(data.state);
	        }
	        else
	        {
		        selectedIndex = (selectedIndex + 1) % currentMenu->childrenCount;
		        if (selectedIndex == 0)
		        {
			        if (currentMenu->childrenCount > MAX_DISPLAY_ITEM)
				        screenFull();
			        firstVisibleItem = 0;
		        }
	        }
	        break;

        case KEY_LEFT: // 左键
	        if (currentItem->type == menuItemType::CHANGEABLE && currentItem->itemData.changeable.is_locked)
		        currentItem->itemData.changeable.is_locked = !currentItem->itemData.changeable.is_locked;

	        else if (currentItem->type == menuItemType::TOGGLE && currentItem->itemData.toggle.is_locked)
		        currentItem->itemData.toggle.is_locked = !currentItem->itemData.toggle.is_locked;

	        else if (currentMenu->parent)
	        {
		        currentMenu = currentMenu->parent;
		        selectedIndex = currentMenu->savedSelectedIndex;
		        firstVisibleItem = currentMenu->savedFirstVisibleItem;
		        screenFull();
	        }
	        break;

        case KEY_RIGHT: // 右键
	        if (currentItem->childrenCount > 0)
	        {
		        screenFull();

		        currentMenu->savedSelectedIndex = selectedIndex;
		        currentMenu->savedFirstVisibleItem = firstVisibleItem;

		        currentMenu = currentItem;
		        selectedIndex = 0;
		        firstVisibleItem = 0;
	        }
	        else if (currentItem->type == menuItemType::CHANGEABLE)
		        currentItem->itemData.changeable.is_locked = true;
	        else if (currentItem->type == menuItemType::TOGGLE)
		        currentItem->itemData.toggle.is_locked = true;

	        else if (currentItem->appEntry)
	        {
		        screenFull();

		        inAppMode = true;
		        appSavedSelectedIndex = selectedIndex;
		        currentMenu->children[selectedIndex]->appRunning = true;
		        currentMenu->children[selectedIndex]->appEntry(currentMenu->children[selectedIndex]->appArgs);
	        }
	        break;
    }

	if (currentMenu->childrenCount > MAX_DISPLAY_ITEM)
	{
		if (selectedIndex >= firstVisibleItem + MAX_DISPLAY_ITEM)
		{
			screenFull();
			firstVisibleItem += MAX_DISPLAY_ITEM;
		}
		else if (selectedIndex < firstVisibleItem && selectedIndex != 0)
		{
			screenFull();
			firstVisibleItem -= MAX_DISPLAY_ITEM;
		}
	}
}

void menuNavigator::getDisplayInfo(char* buffer, uint8_t lineCount) {
    if (buffer == nullptr || lineCount == 0 || currentMenu == nullptr) {
        return;
    }

    // 使用安全的清零方式
    memset(buffer, 0, MAX_DISPLAY_CHAR * lineCount);

    for (uint8_t i = 0; i < lineCount; i++)
    {
        menuItem* currentItem = currentMenu->children[i + firstVisibleItem];
        if (i < currentMenu->childrenCount)
        {
            if (currentItem->type == menuItemType::NORMAL)
                sprintf(&buffer[i * MAX_DISPLAY_CHAR], "%s%s",
                        (i == selectedIndex - firstVisibleItem) ? "->" : "  ",
                        currentItem->name
                       );
            else if (currentItem->type == menuItemType::CHANGEABLE)
            {
                if (static_cast<int>(currentItem->itemData.changeable.currentValue) / 1 == currentItem->itemData.
                    changeable.currentValue)
                {
                    sprintf(&buffer[i * MAX_DISPLAY_CHAR], "%s%s : %2d   ",
                            (i == selectedIndex - firstVisibleItem) ? "->" : "  ",
                            currentItem->name,
                            static_cast<int>(currentItem->itemData.changeable.currentValue)
                           );
                }
                else
                {
                    sprintf(&buffer[i * MAX_DISPLAY_CHAR], "%s%s : %.3f",
                            (i == selectedIndex - firstVisibleItem) ? "->" : "  ",
                            currentItem->name,
                            currentItem->itemData.changeable.currentValue
                           );
                }
            }
            else if (currentItem->type == menuItemType::TOGGLE)
            {
                sprintf(&buffer[i * MAX_DISPLAY_CHAR], "%s%s :%3s",
                        (i == selectedIndex - firstVisibleItem) ? "->" : "  ",
                        currentItem->name,
                        currentItem->itemData.toggle.state ? " ON" : "OFF"
                       );
            }
        }
    }
}

void menuNavigator::updateDisplay() {
    if (currentMenu)
	{
		for (uint8_t i = 0; i < currentMenu->childrenCount; i++)
		{
			if (currentMenu->children[i])
			{
				currentMenu->children[i]->refresh();
			}
		}
	}

	if (inAppMode)
		return;
	if (currentMenu->childrenCount - firstVisibleItem < MAX_DISPLAY_ITEM)
	{
		this->getDisplayInfo(displayBuffer,
							 currentMenu->childrenCount % MAX_DISPLAY_ITEM
							);
		for (uint8_t i = 0; i < (currentMenu->childrenCount % MAX_DISPLAY_ITEM); i++)
		{
			menuItem* currentItem = currentMenu->children[i + firstVisibleItem];
			if (currentItem->type == menuItemType::CHANGEABLE
				&& currentItem->itemData.changeable.is_locked)
				displayString(0, i * DISPLAY_LINE_HEIGHT,
						  &displayBuffer[i * MAX_DISPLAY_CHAR],
						  RGB565_BLACK, RGB565_YELLOW);
			else if (currentItem->type == menuItemType::TOGGLE
				&& currentItem->itemData.toggle.is_locked)
			{
				if (currentItem->itemData.toggle.state == 0)
					displayString(0, i * DISPLAY_LINE_HEIGHT,
							  &displayBuffer[i * MAX_DISPLAY_CHAR],
							  RGB565_BLACK, RGB565_RED);
				else
					displayString(0, i * DISPLAY_LINE_HEIGHT,
							  &displayBuffer[i * MAX_DISPLAY_CHAR],
							  RGB565_BLACK, RGB565_GREEN);
			}
			else
				displayString(0, i * DISPLAY_LINE_HEIGHT,
						  &displayBuffer[i * MAX_DISPLAY_CHAR],
						  RGB565_BLACK, RGB565_WHITE);
		}
	}
	else
	{
		this->getDisplayInfo(displayBuffer,
							 currentMenu->childrenCount > MAX_DISPLAY_ITEM
								 ? MAX_DISPLAY_ITEM
								 : currentMenu->childrenCount
							);
		for (uint8_t i = 0; i < (currentMenu->childrenCount > MAX_DISPLAY_ITEM
									 ? MAX_DISPLAY_ITEM
									 : currentMenu->childrenCount); i++)
		{
			menuItem* currentItem = currentMenu->children[i + firstVisibleItem];
			if (currentItem->type == menuItemType::CHANGEABLE
				&& currentItem->itemData.changeable.is_locked)
				displayString(0, i * DISPLAY_LINE_HEIGHT,
						  &displayBuffer[i * MAX_DISPLAY_CHAR],
						  RGB565_BLACK, RGB565_YELLOW);
			else if (currentItem->type == menuItemType::TOGGLE
				&& currentItem->itemData.toggle.is_locked)
			{
				if (currentItem->itemData.toggle.state == 0)
					displayString(0, i * DISPLAY_LINE_HEIGHT,
							  &displayBuffer[i * MAX_DISPLAY_CHAR],
							  RGB565_BLACK, RGB565_RED);
				else
					displayString(0, i * DISPLAY_LINE_HEIGHT,
							  &displayBuffer[i * MAX_DISPLAY_CHAR],
							  RGB565_BLACK, RGB565_GREEN);
			}
			else
				displayString(0, i * DISPLAY_LINE_HEIGHT,
						  &displayBuffer[i * MAX_DISPLAY_CHAR],
						  RGB565_BLACK, RGB565_WHITE);
		}
	}
}

void menuNavigator::refreshDisplay()
{
	screenClear();
	if (currentMenu)
	{
		for (uint8_t i = 0; i < currentMenu->childrenCount; i++)
		{
			if (currentMenu->children[i])
			{
				currentMenu->children[i]->refresh();
			}
		}
	}

	inAppMode = false;
	if (currentMenu->childrenCount - firstVisibleItem < MAX_DISPLAY_ITEM)
	{
		this->getDisplayInfo(displayBuffer,
							 currentMenu->childrenCount % MAX_DISPLAY_ITEM
							);
		for (uint8_t i = 0; i < (currentMenu->childrenCount % MAX_DISPLAY_ITEM); i++)
		{
			menuItem* currentItem = currentMenu->children[i + firstVisibleItem];
			if (currentItem->type == menuItemType::CHANGEABLE
				&& currentItem->itemData.changeable.is_locked)
				displayString(0, i * DISPLAY_LINE_HEIGHT,
						  &displayBuffer[i * MAX_DISPLAY_CHAR],
						  RGB565_BLACK, RGB565_YELLOW);
			else if (currentItem->type == menuItemType::TOGGLE
				&& currentItem->itemData.toggle.is_locked)
			{
				if (currentItem->itemData.toggle.state == 0)
					displayString(0, i * DISPLAY_LINE_HEIGHT,
							  &displayBuffer[i * MAX_DISPLAY_CHAR],
							  RGB565_BLACK, RGB565_RED);
				else
					displayString(0, i * DISPLAY_LINE_HEIGHT,
							  &displayBuffer[i * MAX_DISPLAY_CHAR],
							  RGB565_BLACK, RGB565_GREEN);
			}
			else
				displayString(0, i * DISPLAY_LINE_HEIGHT,
						  &displayBuffer[i * MAX_DISPLAY_CHAR],
						  RGB565_BLACK, RGB565_WHITE);
		}
	}
	else
	{
		this->getDisplayInfo(
							 displayBuffer,
							 currentMenu->childrenCount > MAX_DISPLAY_ITEM
								 ? MAX_DISPLAY_ITEM
								 : currentMenu->childrenCount
							);
		for (uint8_t i = 0; i < (currentMenu->childrenCount > MAX_DISPLAY_ITEM
									 ? MAX_DISPLAY_ITEM
									 : currentMenu->childrenCount); i++)
		{
			menuItem* currentItem = currentMenu->children[i + firstVisibleItem];
			if (currentItem->type == menuItemType::CHANGEABLE
				&& currentItem->itemData.changeable.is_locked)
				displayString(0, i * DISPLAY_LINE_HEIGHT,
						  &displayBuffer[i * MAX_DISPLAY_CHAR],
						  RGB565_BLACK, RGB565_YELLOW);
			else if (currentItem->type == menuItemType::TOGGLE
				&& currentItem->itemData.toggle.is_locked)
			{
				if (currentItem->itemData.toggle.state == 0)
					displayString(0, i * DISPLAY_LINE_HEIGHT,
							  &displayBuffer[i * MAX_DISPLAY_CHAR],
							  RGB565_BLACK, RGB565_RED);
				else
					displayString(0, i * DISPLAY_LINE_HEIGHT,
							  &displayBuffer[i * MAX_DISPLAY_CHAR],
							  RGB565_BLACK, RGB565_GREEN);
			}
			else
				displayString(0, i * DISPLAY_LINE_HEIGHT,
						  &displayBuffer[i * MAX_DISPLAY_CHAR],
						  RGB565_BLACK, RGB565_WHITE);
		}
	}
}

#endif // __cplusplus
