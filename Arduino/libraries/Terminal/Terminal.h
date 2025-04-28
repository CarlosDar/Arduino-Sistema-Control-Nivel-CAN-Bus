/**
 * @file Terminal.h
 * @brief Brief of Terminal.h
 * @authors Alberto Ballesteros, Miller Espinosa and Manuel Barranco 
 * @date December 2016 - March 2017
 *
 * @details Detailed description of this file.
 */

#include <Arduino.h>

class Terminal
{
public:

	//******* CONSTANTS *******

	const char ESC = 27;
	const char SPACE = 32;

	const char TERM_ROWS = 24;
	const char TERM_COLS = 80;
	const char NO_CHAR_RX_UART = -1;


	/**************************************************************************/
	/** @name Terminal ********************************************************/
	/**************************************************************************/
	//@{


	/**
	 * @brief Constructor
	 */

	Terminal();
 
	/**
	 * @brief Initilizacion
	 * @param [in] baudRate Transmission/Reception speed in bit per second
	 * @see NUM_LEDS, ledOff, ledToggle
	 */

	void begin(unsigned long baudRate);


	/**
	 * @brief Read byte received from the UART
	 *
	 * @param [in] block Block the execution until a byte is received.
	 *
	 * @return The value of the received byt. If block is <i>false</i> and no byte
	 * has been received, NO_CHAR_RX_UART returned.
	 * 
	 * @see NO_CHAR_RX_UART
	 */

	char getChar(bool block);


	/**
	 * @brief Functions for transmitting through the UART
	 *
	 * @param [in] block Block the execution until a byte is received.
	 *
	 * @details Most of the methods are named as "print", so they are overloaded functions.
	 * Note that the last byte println transmits is a "new line" ASCII character
	 * 
	 * @see NO_CHAR_RX_UART
	 */

	// print
	void print(char c) { Serial.write(c); }
	
	void print(int  i) { Serial.print(i); }
	void print(long i) { Serial.print(i); }
	void print(uint8_t  i) { Serial.print(i); }
	void print(uint16_t i) { Serial.print(i); }
	void print(uint32_t i) { Serial.print(i); }
	
	void print(int  i, int base) { Serial.print(i, base); }
	void print(long i, int base) { Serial.print(i, base); }
	void print(uint8_t  i, int base) { Serial.print(i, base); }
	void print(uint16_t i, int base) { Serial.print(i, base); }
	void print(uint32_t i, int base) { Serial.print(i, base); }
	
	void print(float  f) { Serial.print(f); }
	void print(double d) { Serial.print(d); }
	
	void print(char *s) { Serial.print(s); }
	
	// println
	void println(int  i) { Serial.println(i); }
	void println(long i) { Serial.println(i); }
	void println(uint8_t  i) { Serial.println(i); }
	void println(uint16_t i) { Serial.println(i); }
	void println(uint32_t i) { Serial.println(i); }
	
	void println(int  i, int base) { Serial.println(i, base); }
	void println(long i, int base) { Serial.println(i, base); }
	void println(uint8_t  i, int base) { Serial.println(i, base); }
	void println(uint16_t i, int base) { Serial.println(i, base); }
	void println(uint32_t i, int base) { Serial.println(i, base); }
	
	void println(float  f) { Serial.println(f); }
	void println(double d) { Serial.println(d); }
	
	void println(char *s) { Serial.println(s); }


	/**
	 * @brief Each function transmits an Scape Sequence through the UART
	 * in order to implement a given command that moves the cursor to a given
	 * position of the terminal
	 */

	void moveHome(void);
	void moveUp(void);
	void moveDown(void);
	void moveRight(void);
	void moveLeft(void);
	void newLine(void);


	/**
	 * @brief Transmit Scape Sequence to move cursor to a given row and column
	 * of the terminal.
	 * @details Since the Terminal has TERM_ROWS+1 lines and TERM_COLS+1 columns 
	 * the values of the coordinates are restricted to those values.
	 * @param [in] row Line. Valid value between 0 and TERM_ROWS.
	 * @param [in] col Column. Valid value between 0 and TERM_COLS.
	 * @see TERM_ROWS, TERM_COLS
	 */
	void move(char row, char col);


	/**
	 * @brief Each function transmits an Scape Sequence through the UART
	 * in order to implement a given command to delete text from the terminal
	 */

	void clear(void);
	void deleteToEndLine(void);


	/**
	 * @brief Transmit byte value through the UART that represents the bell
	 * character, so that the terminal produces a 'bell' ring sound
	 */
	void bell(void);


	/**
	 * @brief Wait until the UART finishes all pending transmissions
 	 */
	void waitTxComplete();

private:
	int pow10(int exp);
};
