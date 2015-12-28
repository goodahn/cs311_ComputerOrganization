/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   cs311.c                                                   */
/*                                                             */
/***************************************************************/
#include <stdio.h>
#include "pipeline.h"
#include "util.h"
#include "parse.h"
#include "run.h"

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
  FILE *prog;
  int ii = 0;
  char buffer[33];

  /* To notifying data & text segment size */
  int flag = 0;
  int text_index = 0;
  int data_index = 0;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  //read 32bits + '\0' = 33
  while (fgets(buffer,33,prog) != NULL) {
    if(flag == 0) {
      //check text segment size
      text_size = fromBinary(buffer);
      NUM_INST = text_size/4;
      //initial memory allocation of text segment
      INST_INFO = malloc(sizeof(instruction)*(text_size/4));
      init_inst_info();
    }
    else if(flag == 1) {
      //check data segment size
      data_size = fromBinary(buffer);
    }
    else{
      if(ii < text_size)
        INST_INFO[text_index++] = parsing_instr(buffer, ii);
      else if(ii < text_size + data_size)
        parsing_data(buffer, ii-text_size);
      else{
        //Do not enter this case
        //assert(0);
        //However, there is a newline in the input file
      }
      ii += 4;
    }
    flag++;
  }
  CURRENT_STATE.PC = MEM_TEXT_START + 4;
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename) {
  int i;
/*  if_id_latch.act = false;
  if_id_latch.full = false;
  id_ex_latch.act = false;
  id_ex_latch.full = false;
  ex_mem_latch.act = false ;
  ex_mem_latch.full = false ;
  mem_wb_latch.act = false;
  mem_wb_latch.full = false;*/
//  printf("%d %d %d %d %d\n",id_ex_latch.opcode, id_ex_latch.funcode, id_ex_latch.rd, id_ex_latch.rs, id_ex_latch.rt );
  init_memory();
  load_program(program_filename);
  if_register.act = TRUE;
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
  char** tokens;
  int count = 1;
  int addr1 = 0;
  int addr2 = 0;
  int num_inst = 0;

  /* Default option: run 100 cycle */
  int i = 100;

  /* Flags for option */
  int mem_dump_set = 0;
  pipeline_dump_set = 0;
  int debug_set = 0;
  int num_inst_set = 0;
  INSTRUCTION_COUNT = 0;
  branch_prediction = 1;
  data_forwarding = 0;
  CYCLE = 1;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s [-nobp] [-f] [-m addr1:addr2] [-d] [-p] [-n num_instr] inputBinary\n", argv[0]);
    exit(1);
  }

  initialize(argv[argc-1]);

  /* Main argument parsing */
  while(count != argc-1) {
    if(strcmp(argv[count], "-m") == 0) {
      tokens = str_split(argv[++count],':');

      addr1 = (int)strtol(*(tokens), NULL, 16);
      addr2 = (int)strtol(*(tokens+1), NULL, 16);
      mem_dump_set = 1;
    }
    else if(strcmp(argv[count], "-p") == 0)
      pipeline_dump_set = 1;
    else if(strcmp(argv[count], "-f") == 0)
      data_forwarding = 1;
    else if(strcmp(argv[count], "-nobp") == 0)
      branch_prediction = 0;
    else if(strcmp(argv[count], "-d") == 0)
      debug_set = 1;
    else if(strcmp(argv[count], "-n") == 0) {
      num_inst = (int)strtol(argv[++count], NULL, 10);
      num_inst_set = 1;
    }
    else {
      printf("Error: usage: %s [-nobp] [-f] [-m addr1:addr2] [-d] [-p] [-n num_instr] inputBinary\n", argv[0]);
      exit(1);
    }
    count++;
  }

  /* Execute */
  if(num_inst_set) i = num_inst;

  if(debug_set) {
    printf("Simulating for %d insturctions...\n\n", i);

    while( INSTRUCTION_COUNT <i ) {
      if (RUN_BIT==FALSE){
        printf("Simulator halted\n");
        break;
      }
      cycle();
      if( pipeline_dump_set ) pdump();
      rdump();

      if(mem_dump_set) mdump(addr1, addr2);
      update();
    }
  }
  else{
    run(i);
    rdump();

    if(mem_dump_set) mdump(addr1, addr2);
    printf("Simulator halted\n");
  }
  return 0;
}
