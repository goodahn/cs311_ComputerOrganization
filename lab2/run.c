/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   run.c                                                     */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction(){

    /* Your code */
  CURRENT_STATE.PC+=4;
  int index =  (CURRENT_STATE.PC - MEM_TEXT_START - 4)/4 ;
//  printf(" index %d, NUM_INST %d \n",index,NUM_INST);
  if( index+1 >= NUM_INST ) {
    RUN_BIT=FALSE;
//    printf("Run bit unset pc: %x\n",CURRENT_STATE.PC);
  }
  short opcode = INST_INFO[ index ].opcode;
  short func_code = INST_INFO[ index ].func_code;
  char rs;
  char rt;
  char rd;
  unsigned int rs_data;
  unsigned int rt_data;
  unsigned int rd_data;
  char shamt;
  unsigned int imm;

  switch(opcode) {
    case 0:  // 000000 ~

      switch(func_code) {
        case 0:   // sll
          rt = INST_INFO[ index ].r_t.r_i.rt;
          rd = INST_INFO[ index ].r_t.r_i.r_i.r.rd;
          shamt = INST_INFO[ index ].r_t.r_i.r_i.r.shamt;
          CURRENT_STATE.REGS[ rd ] = CURRENT_STATE.REGS[ rt ] << shamt;
          break;
        case 2:   //  srl
          rt = INST_INFO[ index ].r_t.r_i.rt;
          rd = INST_INFO[ index ].r_t.r_i.r_i.r.rd;
          shamt = INST_INFO[ index ].r_t.r_i.r_i.r.shamt;
          CURRENT_STATE.REGS[ rd ] = CURRENT_STATE.REGS[ rt ] >> shamt;
          break;
        case 8:  //  jr
          CURRENT_STATE.PC = CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rs ];
          break;
        case 33: //  addu
          rs = INST_INFO[ index ].r_t.r_i.rs;
          rt = INST_INFO[ index ].r_t.r_i.rt;
          rd = INST_INFO[ index ].r_t.r_i.r_i.r.rd;
          CURRENT_STATE.REGS[ rd ] = CURRENT_STATE.REGS[ rs ]+ CURRENT_STATE.REGS[ rt ];
          break;
        case 35: //  subu
          rs = INST_INFO[ index ].r_t.r_i.rs;
          rt = INST_INFO[ index ].r_t.r_i.rt;
          rd = INST_INFO[ index ].r_t.r_i.r_i.r.rd;
          CURRENT_STATE.REGS[ rd ] = CURRENT_STATE.REGS[ rs ] - CURRENT_STATE.REGS[ rt ];
          break;
        case 36: //  and
          rs = INST_INFO[ index ].r_t.r_i.rs;
          rt = INST_INFO[ index ].r_t.r_i.rt;
          rd = INST_INFO[ index ].r_t.r_i.r_i.r.rd;
          CURRENT_STATE.REGS[ rd ] = CURRENT_STATE.REGS[ rs ] & CURRENT_STATE.REGS[ rt ];
          break;
        case 37: //  or
          rs = INST_INFO[ index ].r_t.r_i.rs;
          rt = INST_INFO[ index ].r_t.r_i.rt;
          rd = INST_INFO[ index ].r_t.r_i.r_i.r.rd;
          CURRENT_STATE.REGS[ rd ] = (CURRENT_STATE.REGS[ rs ] | CURRENT_STATE.REGS[ rt ]);
          break;
        case 39: //  nor
          rs = INST_INFO[ index ].r_t.r_i.rs;
          rt = INST_INFO[ index ].r_t.r_i.rt;
          rd = INST_INFO[ index ].r_t.r_i.r_i.r.rd;
          CURRENT_STATE.REGS[ rd ] = ~(CURRENT_STATE.REGS[ rs ] | CURRENT_STATE.REGS[ rt ]);
          break;
        case 43: //  sltu
          rs = INST_INFO[ index ].r_t.r_i.rs;
          rt = INST_INFO[ index ].r_t.r_i.rt;
          rd = INST_INFO[ index ].r_t.r_i.r_i.r.rd;
          if( CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt] ) {
            CURRENT_STATE.REGS[rd] = 1;
          }
          else {
            CURRENT_STATE.REGS[rd] = 0;
          }

          break;
      }
      break;
    case 2: // j
      CURRENT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) | INST_INFO[ index ].r_t.target<<2;
      break;
    case 3: // jal
      CURRENT_STATE.REGS[31] = CURRENT_STATE.PC;
      CURRENT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) | INST_INFO[ index ].r_t.target<<2;
      break;
    case 4: // beq
      rs_data = CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rs ];
      rt_data = CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rt ];
      imm = INST_INFO[ index ].r_t.r_i.r_i.imm;
      if( rs_data == rt_data  ) {
        CURRENT_STATE.PC += imm<<2;
      }
      break;
    case 5: // bne
      rs_data = CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rs ];
      rt_data = CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rt ];
      imm = INST_INFO[ index ].r_t.r_i.r_i.imm;
      if( rs_data != rt_data  ) {
        CURRENT_STATE.PC += imm<<2;
      }
      break;
    case 9: // addiu
      rs_data = CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rs ];
      imm =   INST_INFO[ index ].r_t.r_i.r_i.imm;
//      printf("rs_data is %d imm is %hd \n",rs_data,imm);
      CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rt ] = rs_data+imm;
      break;
    case 11:// sltiu
      rs_data = CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rs ];
      imm =   INST_INFO[ index ].r_t.r_i.r_i.imm;
//      printf("imm is %x\n", imm);
      if( rs_data< imm) {
        CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rt ] = 1;
      } else {
        CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rt ] = 0;
      }
      break;
    case 12:// andi
      rs_data = CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rs ];
      imm =  (unsigned short) INST_INFO[ index ].r_t.r_i.r_i.imm;
//      printf(" rs_data %p imm %p",rs_data,imm);
      CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rt ] = rs_data&imm;
      break;
    case 13:// ori
//      printf(" reg num is %hd\n",INST_INFO[ index ].r_t.r_i.rs );
      rs_data = CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rs ];
//      printf("rs is %p \n",rs);
      imm =  (unsigned short) INST_INFO[ index ].r_t.r_i.r_i.imm;
      CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rt ] = rs_data|imm;
      break;
    case 15:// lui
      imm =  (unsigned short) INST_INFO[ index ].r_t.r_i.r_i.imm;
//      printf("rt is %hd\n",INST_INFO[ index ].r_t.r_i.rt); 
      CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rt ] = imm<<16;
//      printf("imm<<16 is p\n",imm<<16);
      break;
    case 35:// lw
      rt = INST_INFO[ index ].r_t.r_i.rt;
      rs_data = CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rs ];
      imm = INST_INFO[ index ].r_t.r_i.r_i.imm;
      CURRENT_STATE.REGS[ rt ] = mem_read_32( rs_data+imm );
      break;
    case 43:// sw
      rt_data = CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rt ];
      rs_data = CURRENT_STATE.REGS[ INST_INFO[ index ].r_t.r_i.rs ];
      imm = INST_INFO[ index ].r_t.r_i.r_i.imm;
      mem_write_32( imm+rs_data, rt_data );
      break;
  }


}
