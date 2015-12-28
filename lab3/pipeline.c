#include<stdio.h>
#include "pipeline.h"
#include "util.h"

void instruction_fetch() {
  int index =  (CURRENT_STATE.PC - MEM_TEXT_START - 4)/4 ;
  short opcode = INST_INFO[ index ].opcode;
  short func_code = INST_INFO[ index ].func_code;

  if( index == NUM_INST ) {
    if_register.opcode =0;
    if_register.func_code = 0;
  }
  if_register.PC = CURRENT_STATE.PC-4;
  if_register.opcode = opcode;
  if_register.func_code = func_code;
  // r type
  if( opcode == 0 && func_code != 8 ) {
    if_register.register_write = TRUE;
    if_register.access_type = none;
    // sll, srl
    if( func_code == 0 || func_code == 2  ) {
      if_register.rt = INST_INFO[ index ].r_t.r_i.rt;
      if_register.rd = INST_INFO[ index ].r_t.r_i.r_i.r.rd;
      if_register.rs = 0;
      if_register.shamt = INST_INFO[ index ].r_t.r_i.r_i.r.shamt;
    } else {
      // other
      if_register.rs = INST_INFO[ index ].r_t.r_i.rs;
      if_register.rt = INST_INFO[ index ].r_t.r_i.rt;
      if_register.rd = INST_INFO[ index ].r_t.r_i.r_i.r.rd;      
    }
  }
  // i type
  else if( opcode == 4 || opcode == 5 || opcode == 9 || opcode == 11
      || opcode == 12 || opcode == 13 || opcode == 15 || opcode == 35
      || opcode == 43 ) {
    if( opcode == 4 || opcode == 5 ) {
      if_register.register_write = FALSE;
      if_register.access_type = none;
      if_register.rs =  INST_INFO[ index ].r_t.r_i.rs;
      if_register.rt =  INST_INFO[ index ].r_t.r_i.rt;
      if_register.imm = INST_INFO[ index ].r_t.r_i.r_i.imm;
      if_register.rd = 0;
    } else if( opcode == 35 || opcode == 43 ){
      if( opcode == 35 ) {
        if_register.register_write = TRUE;
        if_register.access_type = memory_read;
        if_register.rs =  INST_INFO[ index ].r_t.r_i.rs;
        if_register.rd =  INST_INFO[ index ].r_t.r_i.rt;
        if_register.rt = 0;
        if_register.imm = INST_INFO[ index ].r_t.r_i.r_i.imm;
      } else {
        if_register.register_write = FALSE;
        if_register.access_type = memory_write;
        if_register.rs =  INST_INFO[ index ].r_t.r_i.rs;
        if_register.rd =  INST_INFO[ index ].r_t.r_i.rt;
        if_register.rt = 0;
        if_register.imm = INST_INFO[ index ].r_t.r_i.r_i.imm;
      }
    } else {
      if_register.register_write = TRUE;
      if_register.access_type = none;
      if_register.rs =  INST_INFO[ index ].r_t.r_i.rs;
      if_register.rt = 0;
      if_register.rd =  INST_INFO[ index ].r_t.r_i.rt;
      if_register.imm = INST_INFO[ index ].r_t.r_i.r_i.imm;
    }
  }
  // j type
  else if( ( opcode == 0 && func_code == 8 ) || opcode == 2 || opcode == 3 ) {
    if_register.register_write = FALSE;
    if_register.access_type = none;
    // for jr
    if( opcode == 0 ) {
      if_register.register_write = TRUE;
      if_register.rs = INST_INFO[index ].r_t.r_i.rs ;
      if_register.rt = 0;
      if_register.rd = 0;
    } else {
      //other
      if_register.target = INST_INFO[ index ].r_t.target;
      if_register.rt = 0;
      if_register.rd = 0;
      if_register.rs = 0;
    }
  } else {
    // unknown instruction
    printf("Unknown Instruction Error\n");
    printf("index : %d opcode : %d func_code : %d\n",index,opcode,func_code);
    exit(-1);
  }
  if( index == NUM_INST ){
    flush_if();
  }
}

void instruction_decode() {


  short opcode = if_id_latch.opcode;
  short func_code = if_id_latch.func_code;

  id_register.PC = if_id_latch.PC;
  id_register.opcode = opcode;
  id_register.forward_to = if_id_latch.forward_to;
  id_register.forward_from = if_id_latch.forward_from;
  id_register.func_code = func_code;
  id_register.rs = if_id_latch.rs; 
  id_register.rt = if_id_latch.rt;
  id_register.rd = if_id_latch.rd;
  id_register.shamt = if_id_latch.shamt;
  id_register.imm  = if_id_latch.imm;
  id_register.target = if_id_latch.target;
  id_register.register_write = if_id_latch.register_write;
  id_register.access_type = if_id_latch.access_type;


  // arithmetic part
  if( (opcode == 0 && func_code != 8 ) || opcode == 9 || opcode == 11 
      || opcode == 12 || opcode == 13 || opcode == 15) {
    // r type
    if( opcode == 0 ) {
      // not using shamt
      if( func_code != 0 && func_code !=2 ) {
        id_register.rs_data = CURRENT_STATE.REGS[ id_register.rs ];
        id_register.rt_data = CURRENT_STATE.REGS[ id_register.rt ];
      }
      id_register.rt_data = CURRENT_STATE.REGS[ id_register.rt ];
    }
    // i type
    else{
      if( opcode == 12 || opcode == 13 || opcode == 15 ) {
        // andi, ori, lui
        id_register.rs_data = CURRENT_STATE.REGS[ id_register.rs ];
        id_register.imm = (unsigned short) id_register.imm;
      }
      else {
        // other
        id_register.rs_data = CURRENT_STATE.REGS[ id_register.rs ];
        id_register.imm = if_id_latch.imm;
      }
    }
  }
  // memory read/write
  else if( opcode == 35 || opcode == 43 ) {
    if( opcode == 35) {
      // lw
      id_register.rs_data = CURRENT_STATE.REGS[ id_register.rs ];
      id_register.imm = if_id_latch.imm;
    } else{
      // sw
      id_register.rs_data = CURRENT_STATE.REGS[ id_register.rs ];
      id_register.rt_data = CURRENT_STATE.REGS[ id_register.rt ];
      id_register.imm = if_id_latch.imm;
    }
  } else if( (opcode == 0 && func_code == 8) || opcode ==2 || opcode == 3) {
    // jump
    if( opcode == 0 ) {
      // for jr
      id_register.rs_data = CURRENT_STATE.REGS[ id_register.rs ];
//      CURRENT_STATE.PC = id_register.rs_data;
    } else {
      // other
      if( opcode == 2 ) {
        CURRENT_STATE.PC = ( id_register.PC & 0xf0000000 ) | id_register.target << 2;
      } else if( opcode == 3 ) {
        CURRENT_STATE.PC = ( id_register.PC & 0xf0000000 ) | id_register.target << 2;
      }
    }
    if_register.flush_bit = 1;
  } else if( opcode == 4 || opcode == 5 ) {
    // branch
    id_register.rs_data = CURRENT_STATE.REGS[ id_register.rs ];
    id_register.rt_data = CURRENT_STATE.REGS[ id_register.rt ];
    id_register.imm = if_id_latch.imm;
    if_register.flush_bit = 1;
    if( branch_prediction ) {
      branch_target_buffer[0] = CURRENT_STATE.PC;
      CURRENT_STATE.PC = id_register.PC + (id_register.imm<<2) + 4;
    }
  }
}

void execute_instruction() {
  short opcode = id_ex_latch.opcode;
  short func_code = id_ex_latch.func_code;

  ex_register.PC = id_ex_latch.PC;
  ex_register.opcode = opcode;
  ex_register.func_code = func_code;
  ex_register.rs = id_ex_latch.rs; 
  ex_register.rt = id_ex_latch.rt;
  ex_register.rd = id_ex_latch.rd;
  ex_register.imm = id_ex_latch.imm;
  ex_register.shamt = id_ex_latch.shamt;
  ex_register.rs_data = id_ex_latch.rs_data;
  ex_register.rt_data = id_ex_latch.rt_data;
  ex_register.target = id_ex_latch.target;
  ex_register.forward_from = id_ex_latch.forward_from;
  ex_register.forward_to = id_ex_latch.forward_to;
  ex_register.access_type = id_ex_latch.access_type;
  ex_register.register_write = id_ex_latch.register_write;

  // data forwarding
  if( data_forwarding ) {
    if( ( ex_register.forward_from & ex_mem ) == ex_mem ) {
      if( ( ex_register.forward_to & ex_mem_rs ) == ex_mem_rs ) {
        ex_register.rs_data = ex_mem_latch.alu_result;
      } 
      if( ( ex_register.forward_to & ex_mem_rt ) == ex_mem_rt ) {
        ex_register.rt_data = ex_mem_latch.alu_result;
      }
    }
    if( ( ex_register.forward_from & mem_wb ) == mem_wb ) {
      if( mem_wb_latch.access_type == memory_read ) {
        if( ( ex_register.forward_to & mem_wb_rs ) == mem_wb_rs ) {
          ex_register.rs_data = mem_wb_latch.read_data;
        } 
        if( ( ex_register.forward_to & mem_wb_rt ) == mem_wb_rt ) {
          ex_register.rt_data = mem_wb_latch.read_data;
        }
      } else {
        if( ( ex_register.forward_to & mem_wb_rs ) == mem_wb_rs ) {
          ex_register.rs_data = mem_wb_latch.alu_result;
        }
        if( ( ex_register.forward_to & mem_wb_rt ) == mem_wb_rt ) {
          ex_register.rt_data = mem_wb_latch.alu_result;
        }

      }
    }
  }
  // arithmetic part
  if( (opcode == 0 && func_code != 8 ) || opcode == 9 || opcode == 11 
      || opcode == 12 || opcode == 13 || opcode == 15) {
    if( opcode == 0) {
      switch(func_code) {
        case 0:   // sll
          ex_register.alu_result = ex_register.rt_data << ex_register.shamt;
          break;
        case 2:   //  srl
          ex_register.alu_result = ex_register.rt_data >> ex_register.shamt;
          break;
        case 33: //  addu
          ex_register.alu_result = ex_register.rs_data + ex_register.rt_data;
          break;
        case 35: //  subu
          ex_register.alu_result = ex_register.rs_data - ex_register.rt_data;
          break;
        case 36: //  and
          ex_register.alu_result = ex_register.rs_data & ex_register.rt_data;
          break;
        case 37: //  or
          ex_register.alu_result = ( ex_register.rs_data | ex_register.rt_data );
          break;
        case 39: //  nor
          ex_register.alu_result = ~( ex_register.rs_data | ex_register.rt_data );
          break;
        case 43: //  sltu
          if( ex_register.rs_data < ex_register.rt_data ) {
            ex_register.alu_result = 1;
          } else {
            ex_register.alu_result = 0;
          }
          break;
      }
    } else if( opcode == 9 ) { //addiu
      ex_register.alu_result = ex_register.rs_data + ex_register.imm ;
    } else if( opcode == 11 ) { // sltiu
      if( ex_register.rs_data < ex_register.imm ) {
        ex_register.alu_result = 1;
      } else {
        ex_register.alu_result = 0;
      }
    } else if( opcode == 12 ) { // andi
      ex_register.alu_result = ex_register.rs_data & ex_register.imm;
    } else if( opcode == 13 ) { // ori
      ex_register.alu_result = ex_register.rs_data | ex_register.imm;
    } else if( opcode == 15 ) { // lui
      ex_register.alu_result = ex_register.imm << 16;
    }
  }
  // memory read/write
  else if( opcode == 35 || opcode == 43 ) {
    ex_register.alu_result = ex_register.rs_data + ex_register.imm;
  }
  // jump
  else if( (opcode == 0 && func_code == 8) || opcode == 2 || opcode == 3) {
    // jr
    if( opcode == 0 ) {
      ex_register.alu_result = ex_register.rs_data;
    } // other 
    else {
      ex_register.alu_result = ( ex_register.PC & 0xf0000000 ) | ex_register.target<<2;
    }
  }
  // branch
  else if( opcode == 4 || opcode == 5 ) {
    // beq
    if( opcode == 4 ) {
      if( branch_prediction ) {
        if( ex_register.rs_data != ex_register.rt_data ) {
          ex_register.alu_result = -1;
        } else {
          ex_register.alu_result = 0;
        }
      } else {
        if( ex_register.rs_data == ex_register.rt_data ) {
          ex_register.alu_result = ex_register.imm << 2;
        } else {
          ex_register.alu_result = -1;
        }
      }
    } // bne 
    else {
      if( branch_prediction ) {
        if( ex_register.rs_data == ex_register.rt_data ) {
          ex_register.alu_result = -1;
        } else {
          ex_register.alu_result = 0;
        }
      } else {
        if( ex_register.rs_data != ex_register.rt_data ) {
          ex_register.alu_result = ex_register.imm << 2;
        } else {
          ex_register.alu_result = -1;
        }
      }
    }
  }
}

void memory_access() {
  short opcode = ex_mem_latch.opcode;
  short func_code = ex_mem_latch.func_code;
  mem_register.PC = ex_mem_latch.PC;
  mem_register.opcode = opcode;
  mem_register.func_code = func_code;
  mem_register.rs = ex_mem_latch.rs; 
  mem_register.rt = ex_mem_latch.rt;
  mem_register.rd = ex_mem_latch.rd;
  mem_register.shamt = ex_mem_latch.shamt;
  mem_register.rs_data = ex_mem_latch.rs_data;
  mem_register.rt_data = ex_mem_latch.rt_data;
  mem_register.target = ex_mem_latch.target;
  mem_register.access_type = ex_mem_latch.access_type;
  mem_register.register_write = ex_mem_latch.register_write;
  mem_register.alu_result = ex_mem_latch.alu_result;
  mem_register.forward_from = ex_mem_latch.forward_from;
  mem_register.forward_to = ex_mem_latch.forward_to;

  // arithmetic part
  if( (opcode == 0 && func_code != 8 ) || opcode == 9 || opcode == 11 
      || opcode == 12 || opcode == 13 || opcode == 15) {
  }
  // memory read/write
  else if( opcode == 35 || opcode == 43 ) {
    if( opcode == 35 ) {
      mem_register.read_data = mem_read_32( mem_register.alu_result );
    } else {
      mem_write_32( mem_register.alu_result, mem_register.rt_data );
    }
  }
  // jump
  else if( (opcode == 0 && func_code == 8) || opcode ==2 || opcode == 3) {
  }
  // branch
  else if( opcode == 4 || opcode == 5 ) {
    if( opcode == 4) {
      if( mem_register.alu_result != -1 ) {
        if( branch_prediction ) {
        } else {
          CURRENT_STATE.PC = mem_register.PC + 4 + mem_register.alu_result;
        }
      } else {
        if( branch_prediction ) {
          if_register.flush_bit = 1;
          id_register.flush_bit = 1;
          CURRENT_STATE.PC = mem_register.PC + 4;
        } else {
          CURRENT_STATE.PC = mem_register.PC + 4;
        }
      }
    } else {
      if( mem_register.alu_result != -1 ) {
        if( branch_prediction ) {
        } else {
          CURRENT_STATE.PC = mem_register.PC + 4 + mem_register.alu_result;
        }
      } else {
        if( branch_prediction ) {
          if_register.flush_bit = 1;
          id_register.flush_bit = 1;
          CURRENT_STATE.PC = mem_register.PC + 4;
        } else {
          CURRENT_STATE.PC = mem_register.PC + 4;
        }
      }
    }
  }
}

void write_back() {
  short opcode = mem_wb_latch.opcode;
  short func_code = mem_wb_latch.func_code;
  wb_register.PC = mem_wb_latch.PC;
  wb_register.opcode = opcode;
  wb_register.func_code = func_code;
  wb_register.rs = mem_wb_latch.rs; 
  wb_register.rt = mem_wb_latch.rt;
  wb_register.rd = mem_wb_latch.rd;
  wb_register.shamt = mem_wb_latch.shamt;
  wb_register.rs_data = mem_wb_latch.rs_data;
  wb_register.rt_data = mem_wb_latch.rt_data;
  wb_register.target = mem_wb_latch.target;
  wb_register.access_type = mem_wb_latch.access_type;
  wb_register.register_write = mem_wb_latch.register_write;
  wb_register.alu_result = mem_wb_latch.alu_result;
  wb_register.read_data = mem_wb_latch.read_data;
//  printf(" wb rd: %d rs: %p  shamt : %p\n",wb_register.rd,wb_register.rs_data,wb_register.shamt);
//  printf(" wb alu result %p \n",wb_register.alu_result);
  // arithmetic part
  if( opcode == 0 && func_code != 8 ) {
    if( wb_register.rd != 0 ) {
      CURRENT_STATE.REGS[ wb_register.rd ] = wb_register.alu_result;
    }
  }
  // memory read/write
  else if( opcode == 35 || opcode == 43 ) {
    if( opcode == 35 ) {
      if( wb_register.rd != 0 ) {
        CURRENT_STATE.REGS[ wb_register.rd ] = wb_register.read_data;
      }
    }
  }
  // jump
  else if( (opcode == 0 && func_code == 8) || opcode ==2 || opcode == 3) {
    if( opcode == 3 ) {
      CURRENT_STATE.REGS[ 31 ] = wb_register.PC + 4;
    }
  }
  // branch
  else if( opcode == 4 || opcode == 5 ) {
  }else {
    if( wb_register.rd != 0 ) {
      CURRENT_STATE.REGS[ wb_register.rd ] = wb_register.alu_result;
    }
  }
  if( wb_register.PC != 0 ) {
    INSTRUCTION_COUNT++;
  }
  int index = ( wb_register.PC - MEM_TEXT_START )/4;
  if( index ==( NUM_INST - 1 )  ){
    RUN_BIT = FALSE;
  }
}

void flush_if() {
  if_register.act = 0;
  if_register.stall = 0;
  if_register.PC = 0;
  if_register.opcode = 0;
  if_register.func_code = 0;
  if_register.shamt = 0;
  if_register.rs = 0;
  if_register.rt = 0;
  if_register.rd = 0;
  if_register.imm = 0;
  if_register.target = 0;
  if_register.access_type = none;
  if_register.register_write = 0;
  if_register.flush_bit = 0;
}

void flush_id() {
  id_register.act = 0;
  id_register.stall = 0;
  id_register.PC = 0;
  id_register.opcode = 0;
  id_register.func_code = 0;
  id_register.shamt = 0;
  id_register.rs = 0;
  id_register.rt = 0;
  id_register.rd = 0;
  id_register.imm = 0;
  id_register.target = 0;
  id_register.access_type = none;
  id_register.register_write = 0;
  id_register.rs_data = 0;
  id_register.rt_data = 0;
  id_register.flush_bit = 0;
  id_register.forward_from = 0;
  id_register.forward_to = 0;
}

void flush_ex() {
  ex_register.act = 0;
  ex_register.stall = 0;
  ex_register.PC = 0;
  ex_register.opcode = 0;
  ex_register.func_code = 0;
  ex_register.shamt = 0;
  ex_register.rs = 0;
  ex_register.rt = 0;
  ex_register.rd = 0;
  ex_register.imm = 0;
  ex_register.target = 0;
  ex_register.access_type = none;
  ex_register.register_write = 0;
  ex_register.rs_data = 0;
  ex_register.rt_data = 0;
  ex_register.alu_result = 0;
  ex_register.flush_bit = 0;
}

void flush_id_ex() {
  id_ex_latch.act = 0;
  id_ex_latch.stall = 0;
  id_ex_latch.PC = 0;
  id_ex_latch.opcode = 0;
  id_ex_latch.func_code = 0;
  id_ex_latch.shamt = 0;
  id_ex_latch.rs = 0;
  id_ex_latch.rt = 0;
  id_ex_latch.rd = 0;
  id_ex_latch.imm = 0;
  id_ex_latch.target = 0;
  id_ex_latch.access_type = none;
  id_ex_latch.register_write = 0;
  id_ex_latch.rs_data = 0;
  id_ex_latch.rt_data = 0;
  id_ex_latch.flush_bit = 0;
}
void init_stage_register() {
/*  if_register.act = 0;
  if_register.stall = 0;
  if_register.PC = 0;
  if_register.opcode = 0;
  if_register.func_code = 0;
  if_register.shamt = 0;
  if_register.rs = 0;
  if_register.rt = 0;
  if_register.imm = 0;
  if_register.target = 0;
  if_register.access_type = none;
  if_register.register_write = 0;

  id_register.act = 0;
  id_register.stall = 0;
  id_register.PC = 0;
  id_register.opcode = 0;
  id_register.func_code = 0;
  id_register.shamt = 0;
  id_register.rs = 0;
  id_register.rt = 0;
  id_register.imm = 0;
  id_register.target = 0;
  id_register.access_type = none;
  id_register.register_write = 0;
  id_register.rs_data = 0;
  id_register.rt_data = 0;

  ex_register.act = 0;
  ex_register.stall = 0;
  ex_register.PC = 0;
  ex_register.opcode = 0;
  ex_register.func_code = 0;
  ex_register.shamt = 0;
  ex_register.rs = 0;
  ex_register.rt = 0;
  ex_register.imm = 0;
  ex_register.target = 0;
  ex_register.access_type = none;
  ex_register.register_write = 0;
  ex_register.rs_data = 0;
  ex_register.rt_data = 0;
  ex_register.alu_result = 0;*/

  mem_register.act = 0;
  mem_register.stall = 0;
  mem_register.PC = 0;
  mem_register.opcode = 0;
  mem_register.func_code = 0;
  mem_register.shamt = 0;
  mem_register.rs = 0;
  mem_register.rt = 0;
  mem_register.rd = 0;
  mem_register.imm = 0;
  mem_register.target = 0;
  mem_register.access_type = none;
  mem_register.register_write = 0;
  mem_register.rs_data = 0;
  mem_register.rt_data = 0;
  mem_register.alu_result = 0;
  mem_register.read_data = 0;

  wb_register.act = 0;
  wb_register.stall = 0;
  wb_register.PC = 0;
  wb_register.opcode = 0;
  wb_register.func_code = 0;
  wb_register.shamt = 0;
  wb_register.rs = 0;
  wb_register.rt = 0;
  wb_register.rd = 0;
  wb_register.imm = 0;
  wb_register.target = 0;
  wb_register.access_type = none;
  wb_register.register_write = 0;
  wb_register.rs_data = 0;
  wb_register.rt_data = 0;
  wb_register.alu_result = 0;
  wb_register.read_data = 0;
}
