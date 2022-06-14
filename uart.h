#ifndef UART_H
#define UART_H
#include <stdint.h>
#define MAX_LINE 50

/*
 * Program UART will use 8 databits with single stop bit, single parity bit. This should give reasonable communication in most circumstance especially
 * when connection is converted to straight USB from current default mini-USB on board.
 * This is important to remember when making the driver. Official driver will be python for ease of dev of GUI.
 */


unsigned char  uart0_RxCount(void);
/**
 * @brief Initialize the required registers for using UART
 * 
 * @param baud Baud rate or byte rate of data transfer
 * @return uint8_t Boolean indication of the success of the initialization
 */
uint8_t init_uart(uint32_t baud);

/**
 * @brief Handshake function between device and driver to indicate that the channel is open and functioning
 * 
 */
void uart_handshake(void);

/**
 * @brief Synchronous transfer of one byte
 * 
 * @param c Character to pass, passed by value. 0 will give unexpected results
 * @return uint8_t Boolean indication of the success of the function
 */
uint8_t send_char(const uint8_t c);

/**
 * @brief Pointer to 16 character block of bytes to send
 * 
 * @param blk 16 byte block of characters to send
 * @return uint8_t Boolean indication of the success of the function
 */
uint8_t send_block(const uint8_t blk[16]);

/**
 * @brief Line of characters to send, terminated with a newline character '\n'
 * 
 * @param ln Pointer to line of text terminated with '\n'
 * @return uint8_t Boolean indication of the success of the function
 */
uint8_t send_line(const uint8_t *ln);

/**
 * @brief Synchronous receive of a single character from the buffer.
 * 
 * @param c Pointer to location in which to receive character.
 * @return uint8_t Boolean indication of the success of the function.
 */
uint8_t recv_char(uint8_t *c);

/**
 * @brief Synchronous receive of a 16 character block of data
 * 
 * @param blk 
 * @return uint8_t 
 */
uint8_t recv_block(uint8_t blk[16]);

/**
 * @brief Synchronus receive of a line of characters terminated by a newline '\n'
 * 
 * @param ln Pointer to memory to receive the line of characters into
 * @return uint8_t Boolean indication of the success of the function
 */
uint8_t recv_line(uint8_t *ln);

/**
 * @brief XOR based cyclic redundancy check at the end of each transmitted block. Single byte that should result in the XOR 
 * of the entire received message to be 0 (or 1 cant remember)
 * 
 * @param ln Pointer to line of text
 * @param len Length of the line
 * @return uint8_t Boolean indication of the success of the function.
 */
uint8_t compute_crc8(const uint8_t *ln, uint16_t len);

#endif