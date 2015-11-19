/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   parse.c                                                   */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

/**************************************************************/
/*                                                            */
/* Procedure : parsing_instr                                  */
/*                                                            */
/* Purpose   : parse binary format instrction and return the  */
/*             instrcution data                               */
/*                                                            */
/* Argument  : buffer - binary string for the instruction     */
/*             index  - order of the instruction              */
/*                                                            */
/**************************************************************/
instruction parsing_instr(const char *buffer, const int index) {
	instruction instr;
	char tmp[7];
	strncpy(tmp, buffer, 6);
	tmp[6] = '\0';
	instr.value = fromBinary(buffer);

	if( !strcmp(tmp,"000000")) {
		instr.opcode = 0xffff & fromBinary(tmp);
		char tmp2[7];
		strncpy(tmp2, buffer+26, 7);
		instr.func_code = 0xffff & fromBinary(tmp2);
		if(!strcmp(tmp2, "001000")) {
			instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
		} else if( !strcmp(tmp2,"100001")) {
			instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
			instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
			instr.r_t.r_i.r_i.r.rd = (fromBinary("00000000000000001111100000000000") & fromBinary(buffer)) >> 11;
		} else if( !strcmp(tmp2,"100100")) {
			instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
			instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
			instr.r_t.r_i.r_i.r.rd = (fromBinary("00000000000000001111100000000000") & fromBinary(buffer)) >> 11;
		} else if( !strcmp(tmp2,"100111")) {
			instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
			instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
			instr.r_t.r_i.r_i.r.rd = (fromBinary("00000000000000001111100000000000") & fromBinary(buffer)) >> 11;
		} else if( !strcmp(tmp2,"100101")) {
			instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
			instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
			instr.r_t.r_i.r_i.r.rd = (fromBinary("00000000000000001111100000000000") & fromBinary(buffer)) >> 11;
		} else if( !strcmp(tmp2,"101011")) {
			instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
			instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
			instr.r_t.r_i.r_i.r.rd = (fromBinary("00000000000000001111100000000000") & fromBinary(buffer)) >> 11;
		} else if( !strcmp(tmp2,"000000")) {
			instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
			instr.r_t.r_i.r_i.r.rd = (fromBinary("00000000000000001111100000000000") & fromBinary(buffer)) >> 11;
			instr.r_t.r_i.r_i.r.shamt = (fromBinary("00000000000000000000011111000000") & fromBinary(buffer)) >> 6;
		} else if( !strcmp(tmp2,"000010")) {
			instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
			instr.r_t.r_i.r_i.r.rd = (fromBinary("00000000000000001111100000000000") & fromBinary(buffer)) >> 11;
			instr.r_t.r_i.r_i.r.shamt = (fromBinary("00000000000000000000011111000000") & fromBinary(buffer)) >> 6;
		}else if( !strcmp(tmp2,"100011")) {
			instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
			instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
			instr.r_t.r_i.r_i.r.rd = (fromBinary("00000000000000001111100000000000") & fromBinary(buffer)) >> 11;
		}
	} else if( !strcmp(tmp,"000010")) {
		instr.opcode = 0xffff & fromBinary(tmp);
		instr.r_t.target = 0x3ffffff & fromBinary(buffer);	
	} else if( !strcmp(tmp,"000011")){
		instr.opcode = 0xffff & fromBinary(tmp);
		instr.r_t.target = 0x3ffffff & fromBinary(buffer);		
	} else if( !strcmp(tmp,"000100")) {
		instr.opcode = 0xffff & fromBinary(tmp);
		instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
		instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
		instr.r_t.r_i.r_i.imm = fromBinary("00000000000000001111111111111111") & fromBinary(buffer);
	} else if ( !strcmp(tmp,"000101")) {
		instr.opcode = 0xffff & fromBinary(tmp);
		instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
		instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
		instr.r_t.r_i.r_i.imm = fromBinary("00000000000000001111111111111111") & fromBinary(buffer);
	} else if ( !strcmp(tmp,"001111")) {
		instr.opcode = 0xffff & fromBinary(tmp);
		instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
		instr.r_t.r_i.r_i.imm = fromBinary("00000000000000001111111111111111") & fromBinary(buffer);
	} else if ( !strcmp(tmp,"100011")) {
		instr.opcode = 0xffff & fromBinary(tmp);
		instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
		instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
		instr.r_t.r_i.r_i.imm = fromBinary("00000000000000001111111111111111") & fromBinary(buffer);
	} else if ( !strcmp(tmp,"101011")) {
		instr.opcode = 0xffff & fromBinary(tmp);
		instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
		instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
		instr.r_t.r_i.r_i.imm = fromBinary("00000000000000001111111111111111") & fromBinary(buffer);
	} else if ( !strcmp(tmp,"001001")) {
		instr.opcode = 0xffff & fromBinary(tmp);
		instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
		instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
		instr.r_t.r_i.r_i.imm = fromBinary("00000000000000001111111111111111") & fromBinary(buffer);
	} else if ( !strcmp(tmp,"001100")) {
		instr.opcode = 0xffff & fromBinary(tmp);
		instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
		instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
		instr.r_t.r_i.r_i.imm = fromBinary("00000000000000001111111111111111") & fromBinary(buffer);
	} else if ( !strcmp(tmp,"001101")) {
		instr.opcode = 0xffff & fromBinary(tmp);
//    printf("buffer is %s\n",buffer);
		instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
//    printf(" rs is %hd\n",instr.r_t.r_i.rs);
		instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
		instr.r_t.r_i.r_i.imm = fromBinary("00000000000000001111111111111111") & fromBinary(buffer);
	} else if ( !strcmp(tmp,"001011")) {
		instr.opcode = 0xffff & fromBinary(tmp);
		instr.r_t.r_i.rs = (fromBinary("00000011111000000000000000000000") & fromBinary(buffer)) >> 21;
		instr.r_t.r_i.rt = (fromBinary("00000000000111110000000000000000") & fromBinary(buffer)) >> 16;
		instr.r_t.r_i.r_i.imm = fromBinary("00000000000000001111111111111111") & fromBinary(buffer);
	}


	return instr;
}

/**************************************************************/
/*                                                            */
/* Procedure : parsing_data                                   */
/*                                                            */
/* Purpose   : parse binary format data and store data into   */
/*             the data region                                */
/*                                                            */
/* Argument  : buffer - binary string for data                */
/*             index  - order of data                         */
/*                                                            */
/**************************************************************/
void parsing_data(const char *buffer, const int index) {

	mem_write_32(MEM_DATA_START+index, fromBinary(buffer));
}
