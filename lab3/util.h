/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   util.h                                                    */
/*                                                             */
/***************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "pipeline.h"
#include <string.h>
#include <stdint.h>

#define FALSE 0
#define TRUE  1

/* Basic information */
#define MEM_TEXT_START	0x00400000
#define MEM_TEXT_SIZE	0x00100000
#define MEM_DATA_START	0x10000000
#define MEM_DATA_SIZE	0x00100000
#define MIPS_REGS	32
#define BYTES_PER_WORD	4

typedef enum memory_ac{
  none = 0,
  memory_write = 1,
  memory_read = 2,
} memory_type;

typedef enum forward_loc{
  id_ex = 0,
  ex_mem = 1,
  mem_wb = 2,
  ex_mem_rs = 4,
  ex_mem_rt = 8,
  mem_wb_rs = 16,
  mem_wb_rt = 32,
} forward_location;

struct if_id_latch_struct {
  int act;
  int stall;

  forward_location forward_to;
  forward_location forward_from;

  int flush_bit;

  memory_type access_type;
  int register_write;

  uint32_t PC;

  short opcode;
  short func_code;
  unsigned char shamt;
  unsigned char rd;
  unsigned char rs;
  unsigned char rt;
  unsigned int imm;
  int target;
};

struct id_ex_latch_struct {
  int act;
  int stall;

  forward_location forward_to;
  forward_location forward_from;

  int flush_bit;
 
  memory_type access_type;
  int register_write;

  uint32_t PC;

  short opcode;
  short func_code;
  unsigned char shamt;
  unsigned char rd;
  unsigned char rs;
  unsigned char rt;
  unsigned int imm;
  int target;
  
  unsigned  int rs_data;
  unsigned int rt_data;
};

struct ex_mem_latch_struct {
  int act;
  int stall;
  
  forward_location forward_to;
  forward_location forward_from;

  int flush_bit;

  memory_type access_type;
  int register_write;

  uint32_t PC;

  short opcode;
  short func_code;
  unsigned char shamt;
  unsigned char rd;
  unsigned char rs;
  unsigned char rt;
  unsigned int imm;
  int target;
  
  unsigned  int rs_data;
  unsigned int rt_data;
  
  unsigned int alu_result;
};

struct mem_wb_latch_struct {
  int act;
  int stall;
  
  forward_location forward_to;
  forward_location forward_from;

  int flush_bit;
  
  memory_type access_type;
  int register_write;
  
  uint32_t PC;

  short opcode;
  short func_code;
  unsigned char shamt;
  unsigned char rd;
  unsigned char rs;
  unsigned char rt;
  unsigned int imm;
  int target;
  
  unsigned  int rs_data;
  unsigned int rt_data;
  
  int read_data;
  unsigned int alu_result;

  int write_complete;
};

typedef struct CPU_State_Struct {
    uint32_t PC;		/* program counter */
    uint32_t REGS[MIPS_REGS];	/* register file. */
} CPU_State;

/* To store instruction information */
typedef struct inst_s {
    short opcode;
    
    /*R-type*/
    short func_code;

    union {
        /* R-type or I-type: */
        struct {
	    unsigned char rs;
	    unsigned char rt;

	    union {
	        short imm;

	        struct {
		    unsigned char rd;
		    unsigned char shamt;
		} r;
	    } r_i;
	} r_i;
        /* J-type: */
        uint32_t target;
    } r_t;

    uint32_t value;
} instruction;

/* Memory regions */
typedef struct {
    uint32_t start, size;
    uint8_t *mem;
} mem_region_t;

/* For if/id latch */
extern struct if_id_latch_struct if_register;
extern struct if_id_latch_struct if_id_latch;

/* For id/ex latch */
extern struct id_ex_latch_struct id_register;
extern struct id_ex_latch_struct id_ex_latch;

/* For ex/mem latch */
extern struct ex_mem_latch_struct ex_register;
extern struct ex_mem_latch_struct ex_mem_latch;

/* For mem/wb latch */
extern struct mem_wb_latch_struct mem_register;
extern struct mem_wb_latch_struct mem_wb_latch;
extern struct mem_wb_latch_struct wb_register;

/* For PC and registers */
extern CPU_State CURRENT_STATE;

/* For instructions */
extern instruction *INST_INFO;
extern int NUM_INST;

/* For memory regions */
extern mem_region_t MEM_REGIONS[2];

/* For execution */
extern int RUN_BIT;		/* run bit */
extern int INSTRUCTION_COUNT;
extern int CYCLE;

/* For branch prediction */
extern int branch_prediction;
extern int branch_target_buffer[4];

/* For data forwarding */
extern int data_forwarding;

/* For pdump */
extern int pipeline_dump_set;
/* Functions */
int max( int a, int b );
int min( int a, int b );
void btb_move_right();
char**		str_split(char *a_str, const char a_delim);
int		fromBinary(char *s);
uint32_t	mem_read_32(uint32_t address);
void		mem_write_32(uint32_t address, uint32_t value);
void		cycle();
void		run(int num_cycles);
void		go();
void		mdump(int start, int stop);
void    pdump();
void		rdump();
void		init_memory();
void		init_inst_info();

#endif
