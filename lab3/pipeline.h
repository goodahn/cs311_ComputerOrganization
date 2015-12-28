#ifndef _PIPE_
#define _PIPE_
#include<stdio.h>
#include"util.h"

void instruction_fetch();
void instruction_decode();
void execute_instruction();
void memory_access();
void write_back();
void flush_if();
void flush_id();
void flush_ex();
void flush_if_id();
void flush_id_ex();
void init_stage_register();
#endif
