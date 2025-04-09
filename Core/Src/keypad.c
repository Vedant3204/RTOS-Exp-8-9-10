#include "keypad.h"

/* Define the actual keypad mapping */
const char keypad_map[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

/**
 * @brief Initializes the keypad GPIOs
 */
void keypad_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configure ROWS as OUTPUT (PA4 - PA7)
    GPIO_InitStruct.Pin = ROW1_PIN | ROW2_PIN | ROW3_PIN | ROW4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(KEYPAD_ROW_PORT, &GPIO_InitStruct);

    // Configure COLUMNS as INPUT with Pull-Up (PA0 - PA3)
    GPIO_InitStruct.Pin = COL1_PIN | COL2_PIN | COL3_PIN | COL4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEYPAD_COL_PORT, &GPIO_InitStruct);
}

/**
 * @brief Scans the keypad and returns the pressed key
 * @retval char: Pressed key character or '\0' if no key is pressed
 */
char scan_keypad(void)
{
    for (uint8_t row = 0; row < 4; row++)
    {
        // Set all rows HIGH
        HAL_GPIO_WritePin(KEYPAD_ROW_PORT, ROW1_PIN | ROW2_PIN | ROW3_PIN | ROW4_PIN, GPIO_PIN_SET);

        // Pull only one row LOW at a time
        HAL_GPIO_WritePin(KEYPAD_ROW_PORT, (ROW1_PIN << row), GPIO_PIN_RESET);
        HAL_Delay(5); // Small delay for stability

        // Check columns
        for (uint8_t col = 0; col < 4; col++)
        {
            if (HAL_GPIO_ReadPin(KEYPAD_COL_PORT, (COL1_PIN << col)) == GPIO_PIN_RESET)
            {
                // Debounce delay
                HAL_Delay(50);
                // Check again to confirm
                if (HAL_GPIO_ReadPin(KEYPAD_COL_PORT, (COL1_PIN << col)) == GPIO_PIN_RESET)
                {
                    // Wait until key is released
                    while (HAL_GPIO_ReadPin(KEYPAD_COL_PORT, (COL1_PIN << col)) == GPIO_PIN_RESET);
                    HAL_Delay(50); // Additional debounce

                    return keypad_map[row][col]; // Return the pressed key
                }
            }
        }
    }
    return '\0'; // No key pressed
}

char keypad_get_key()
{
	for (int row = 0; row < 4; row++)
	    {
	        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 << row, GPIO_PIN_RESET); // Pull row LOW

	        for (int col = 0; col < 4; col++)
	        {
	            if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0 << col) == GPIO_PIN_RESET)
	            {
	                while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0 << col) == GPIO_PIN_RESET); // Wait for release
	                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 << row, GPIO_PIN_SET); // Reset row
	                return keypad_map[row][col];
	            }
	        }

	        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 << row, GPIO_PIN_SET); // Reset row
	    }
	    return 0; // No key pressed
}
