#ifndef CT2_CT2ASM_H
#define CT2_CT2ASM_H

extern int (* des_arm_instructions_set[4096])(UINT adr, UINT nOpCode, char * txt);
extern int (* des_thumb_instructions_set[1024])(UINT adr, UINT nOpCode, char * txt);

#endif // CT2_CT2ASM_H
