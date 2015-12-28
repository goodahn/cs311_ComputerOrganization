/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   run.c                                                     */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include "pipeline.h"
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
  // update stage
  write_back();
  if( if_register.stall == 0 ) {
    instruction_fetch();
  } else {
    if_register.stall += -1;
  }
  instruction_decode();
  execute_instruction();
  memory_access();
  btb_move_right();
  int index =  (CURRENT_STATE.PC - MEM_TEXT_START - 4)/4 ;
//  printf(" index %d, NUM_INST %d \n",index,NUM_INST);
  if( index == NUM_INST ) {
    CURRENT_STATE.PC += -4;
//    printf("Run bit unset pc: %x\n",CURRENT_STATE.PC);
  }
}

void update(){
  // update stage register's information to latch

  if( if_register.flush_bit == 1 ) flush_if();
  if( id_register.flush_bit == 1 ) flush_id();
  if( ex_register.flush_bit == 1 ) flush_ex();

  //check data dependency
  if( ex_mem_latch.register_write ) {
    if( ex_mem_latch.rd ) {
      if( ( ex_mem_latch.rd == id_register.rs && id_ex_latch.rd != id_register.rs )
          || ( ex_mem_latch.rd == id_register.rt && id_ex_latch.rd != id_register.rt ) ) {
        if( data_forwarding ) {
          id_register.forward_from = mem_wb;
          if( ex_mem_latch.rd == id_register.rs ) {
            id_register.forward_to = mem_wb_rs;
          }

          if( ex_mem_latch.rd == id_register.rt ) {
            id_register.forward_to = id_register.forward_to | mem_wb_rt;
          }
        } else {
          if_id_latch.stall = max( if_id_latch.stall, 1 );
          flush_id_ex();
          id_ex_latch.stall = max( id_ex_latch.stall, 1 );
        }
      }
    }
  }
  if( id_ex_latch.register_write ) {
    if( id_ex_latch.rd ) {
      if( id_ex_latch.rd == id_register.rs || id_ex_latch.rd == id_register.rt ) {
        if( data_forwarding ) {
          if( id_ex_latch.access_type == memory_read ) {
            if_id_latch.stall = max( if_id_latch.stall, 1 );
            flush_id_ex();
            id_ex_latch.stall = max( id_ex_latch.stall, 1 );
          } else {
            id_register.forward_from = id_register.forward_from | ex_mem;
          }
          if( id_ex_latch.rd == id_register.rs ) id_register.forward_to |= ex_mem_rs;
          if( id_ex_latch.rd == id_register.rt ) id_register.forward_to |= ex_mem_rt;
        } else {
          if_id_latch.stall = max( if_id_latch.stall, 2 );
          flush_id_ex();
          id_ex_latch.stall = max( id_ex_latch.stall, 2 );
        }
      }
    }
  }
  short if_opcode = if_register.opcode;
  short if_func_code = if_register.func_code;
  short id_opcode = id_register.opcode;
  short id_func_code = id_register.func_code;
  short ex_opcode = ex_register.opcode;
  short ex_func_code = ex_register.func_code;
  short mem_opcode = mem_register.opcode;
  short mem_func_code = mem_register.func_code;

  // if_id_latch
  if( if_id_latch.stall == 0 ) {
    if_id_latch.act = TRUE;
    if_id_latch.PC = if_register.PC;
    if_id_latch.opcode = if_opcode;
    if_id_latch.func_code = if_func_code;
    if_id_latch.forward_to = if_register.forward_to;
    if_id_latch.forward_from = if_register.forward_from;
    if_id_latch.rs = if_register.rs;
    if_id_latch.rt = if_register.rt;
    if_id_latch.rd = if_register.rd;
    if_id_latch.imm = if_register.imm;
    if_id_latch.shamt = if_register.shamt;
    if_id_latch.target = if_register.target;
    if_id_latch.access_type = if_register.access_type;
    if_id_latch.register_write = if_register.register_write;
    CURRENT_STATE.PC += 4;
  } else {
    //if_id_latch.act = FALSE;
    //if_id_latch.PC = 0;
    if_id_latch.stall += -1;
  }

  // id_ex_latch
  if( id_ex_latch.stall == 0 ) {
    id_ex_latch.act = TRUE;
    id_ex_latch.PC = id_register.PC;
    id_ex_latch.opcode = id_opcode;
    id_ex_latch.func_code = id_func_code;
    id_ex_latch.forward_to = id_register.forward_to;
    id_ex_latch.forward_from = id_register.forward_from;
    id_ex_latch.rs = id_register.rs;
    id_ex_latch.rt = id_register.rt;
    id_ex_latch.rd = id_register.rd;
    id_ex_latch.imm = id_register.imm;
    id_ex_latch.shamt = id_register.shamt;
    id_ex_latch.target = id_register.target;
    id_ex_latch.rs_data = id_register.rs_data;
    id_ex_latch.rt_data = id_register.rt_data;
    id_ex_latch.access_type = id_register.access_type;
    id_ex_latch.register_write = id_register.register_write;
    // jump
    if( ( id_opcode == 0 && id_func_code == 8 ) || id_opcode == 2 || id_opcode == 3 ) {
      if( id_opcode == 0 ) {
        CURRENT_STATE.PC = id_register.rs_data + 4;
      }
    }
    // branch
    else if( id_opcode == 4 || id_opcode == 5 ) {
      // branch prediction option is on
      if( branch_prediction == 1 ) {
        //       printf(" id_register imm : %d \n",id_register.imm);
        //       printf(" PC: %p\n",CURRENT_STATE.PC);
      } else {
        // branch prediction option is off
        if_register.stall = max( if_register.stall, 2 );
        if_id_latch.stall = max( if_id_latch.stall, 1 );
        CURRENT_STATE.PC+=-4;
      }
    }
  } else {
    //id_ex_latch.act = FALSE;
    //id_ex_latch.PC = 0;
    id_ex_latch.stall += -1;
  }

  // ex_mem_latch
  if( ex_mem_latch.stall == 0 ) {
    ex_mem_latch.act = TRUE;
    ex_mem_latch.PC = ex_register.PC;
    ex_mem_latch.opcode = ex_opcode;
    ex_mem_latch.func_code = ex_func_code;
    ex_mem_latch.forward_to = ex_register.forward_to;
    ex_mem_latch.forward_from = ex_register.forward_from;
    ex_mem_latch.flush_bit = ex_register.flush_bit;
    ex_mem_latch.rs = ex_register.rs;
    ex_mem_latch.rt = ex_register.rt;
    ex_mem_latch.rd = ex_register.rd;
    ex_mem_latch.imm = ex_register.imm;
    ex_mem_latch.shamt = ex_register.shamt;
    ex_mem_latch.target = ex_register.target;
    ex_mem_latch.rs_data = ex_register.rs_data;
    ex_mem_latch.rt_data = ex_register.rt_data;
    ex_mem_latch.alu_result = ex_register.alu_result;
    ex_mem_latch.access_type = ex_register.access_type;
    ex_mem_latch.register_write = ex_register.register_write;
    ex_mem_latch.act = TRUE;
  } else {
    ex_mem_latch.act = FALSE;
    //ex_mem_latch.PC = 0;
    ex_mem_latch.stall += -1;
  }
/*  if( ex_mem_latch.flush_bit == 1) {
    CURRENT_STATE.PC = branch_target_buffer[2];
    flush_if_id();
  }*/

  // mem_wb_latch
  if( mem_wb_latch.stall == 0 ) {
    mem_wb_latch.act = TRUE;
    mem_wb_latch.PC = mem_register.PC;
    mem_wb_latch.opcode = mem_opcode;
    mem_wb_latch.func_code = mem_func_code;
    mem_wb_latch.forward_to = mem_register.forward_to;
    mem_wb_latch.forward_from = mem_register.forward_from;
    mem_wb_latch.rs = mem_register.rs;
    mem_wb_latch.rt = mem_register.rt;
    mem_wb_latch.rd = mem_register.rd;
    mem_wb_latch.imm = mem_register.imm;
    mem_wb_latch.shamt = mem_register.shamt;
    mem_wb_latch.target = mem_register.target;
    mem_wb_latch.rs_data = mem_register.rs_data;
    mem_wb_latch.rt_data = mem_register.rt_data;
    mem_wb_latch.alu_result = mem_register.alu_result;
    mem_wb_latch.read_data = mem_register.read_data;
    mem_wb_latch.access_type = mem_register.access_type;
    mem_wb_latch.register_write = mem_register.register_write;
    mem_wb_latch.act = TRUE;
  } else {
    //mem_wb_latch.act = FALSE;
    //mem_wb_latch.PC = 0;
    mem_wb_latch.stall += -1;
  }

  init_stage_register();
/*  CURRENT_STATE.PC+=4;
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
  // categorize arithmetic, memory read/write, jump, branch

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

*/
}
