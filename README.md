# STM32-SQLite-Encrypted-Data
Transfer ADC Data and Time (RTC) to PC or RPi via UART (HEX DATA) And Save it to SQLite Database as Encrypted Data

STM32-Process
1. Read ADC Data
2. Get RTC
3. Convert ADC and RTC Data to HEX
4. Send data using UART communication
5. Do all of above task using RTOS for multi tasking purpose

PC/RPi-Process
1. Receive UART Data from STM32
2. Convert HEX data to Decimal
3. Convert all data to string
4. Encrypt Data
5. Save all data to database (SQLite)
6. View Realtime Plot (gnuplot)
7. Do step 1-5 and step 6 as a different task in multi tasking mode (by using pthread)
