/********************************************************
 * vpd.c
 *
 * This file defines functions that interface with the 
 *  vital product data for an Atmega328p.
 *
 * Author:   Ty Foster
 * Date:    11/20/2020
 * Revision: 1.0
 * Course Number: SER486
 * Project Number: 2
 *
 * Copyright(C) 2020, Arizona State University
 * All rights reserved
 * 
 * functions are:
 *      vpd_token_cmp(void) - a function to compare the
 *          VPD token from EEPROM to the token for the
 *          VPD for the program
 * 
 *      vpd_is_data_valid(void) - a function to check
 *          that the VPD data is valid
 * 
 *      vpd_write_defaults(void) - a function to write
 *          default data to EEPROM after corruption
 * 
 *      vpd_init(void) - a function to initialize VPD
 *          from EEPROM
 */

#include "eeprom.h"
#include "util.h"
#include "vpd.h"

#define VPD_ADDR 0x0000
#define VPD_SIZE 55
#define VPD_TOKEN "SER"

/* 
NOTE:
    I don't have a third letter of my first name,
    I selected space as x3
*/
static struct vpd_struct defaults = {
    .token = VPD_TOKEN,
    .model = "Ty",
    .manufacturer = "Foster",
    .serial_number = "S239B01U76X",
    .manufacture_date = 0,
    .mac_address = {'t'&0xFE, 'y', ' ', 'f', 'o', 's'},
    .country_of_origin = "USA",
    .checksum = 0
};

/**********************************
 * vpd_token_cmp()
 *
 * Compares vpd token to valid token
 *
 * arguments:
 *  nothing
 *
 * returns:
 *  1 if tokens are equal, otherwise 0
 *
 * changes:
 *  nothing
 */
int vpd_token_cmp() {
    unsigned char i;
    int ret = 1;
    /* check for equality character by character */
    for(i=0; vpd.token[i]!='\0' && VPD_TOKEN[i]!='\0' && ret; i++) {
        /* if characters are not equal -> ret = 0 */
        if(vpd.token[i]!=VPD_TOKEN[i]) ret = 0;
    }
    /* if a is done but b is not -> ret = 0 */
    if (vpd.token[i]!=VPD_TOKEN[i]) ret = 0;
    return ret;
}

/**********************************
 * vpd_is_data_valid(void)
 *
 * Checks if VPD is valid data
 *
 * arguments:
 *  nothing
 *
 * returns:
 *  1 if data is valid, otherwise 0
 *
 * changes:
 *  nothing
 */
int vpd_is_data_valid(void) {
    int ret = 1;
    /* if token is invalid -> data invalid */
    if (vpd_token_cmp()) ret = 0;
    /* if checksum is invalid -> data invalid */
    if (!is_checksum_valid((unsigned char *) &vpd, VPD_SIZE)) ret = 0;
    return ret;
}

/**********************************
 * vpd_write_defaults()
 *
 * Sets default VPD on EEPROM when
 *  data has been corrupted
 *
 * arguments:
 *  nothing
 *
 * returns:
 *  nothing
 *
 * changes:
 *  VPD on EEPROM
 */
void vpd_write_defaults() {
    /* update checksum for defaults */
    update_checksum((unsigned char *) &defaults, VPD_SIZE);
    /* write defaults to EEPROM */
    eeprom_writebuf(VPD_ADDR, (unsigned char *) &defaults, VPD_SIZE);
}

/**********************************
 * vpd_init(void)
 *
 * Initializes vital product data
 *
 * arguments:
 *  nothing
 *
 * returns:
 *  nothing
 *
 * changes:
 *  VPD on EEPROM if corrupted 
 *  or first call.
 */
void vpd_init(void) {
    /* wait for eeprom writes */
    while(eeprom_isbusy());
    /* initialize vpd data with eeprom_readbuf() */
    eeprom_readbuf(VPD_ADDR, (unsigned char *) &vpd, VPD_SIZE);
    /* check data validity */
    if (!vpd_is_data_valid()) { //if data is corrupt
        /* set defaults */
        vpd_write_defaults();
        /* initialize data again */
        eeprom_readbuf(VPD_ADDR, (unsigned char *) &vpd, VPD_SIZE);
    }
}