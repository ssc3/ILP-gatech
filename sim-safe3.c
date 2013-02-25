/* sim-safe.c - sample functional simulator implementation */

/* SimpleScalar(TM) Tool Suite
 * Copyright (C) 1994-2003 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.
 * All Rights Reserved. 
 * 
 * THIS IS A LEGAL DOCUMENT, BY USING SIMPLESCALAR,
 * YOU ARE AGREEING TO THESE TERMS AND CONDITIONS.
 * 
 * No portion of this work may be used by any commercial entity, or for any
 * commercial purpose, without the prior, written permission of SimpleScalar,
 * LLC (info@simplescalar.com). Nonprofit and noncommercial use is permitted
 * as described below.
 * 
 * 1. SimpleScalar is provided AS IS, with no warranty of any kind, express
 * or implied. The user of the program accepts full responsibility for the
 * application of the program and the use of any results.
 * 
 * 2. Nonprofit and noncommercial use is encouraged. SimpleScalar may be
 * downloaded, compiled, executed, copied, and modified solely for nonprofit,
 * educational, noncommercial research, and noncommercial scholarship
 * purposes provided that this notice in its entirety accompanies all copies.
 * Copies of the modified software can be delivered to persons who use it
 * solely for nonprofit, educational, noncommercial research, and
 * noncommercial scholarship purposes provided that this notice in its
 * entirety accompanies all copies.
 * 
 * 3. ALL COMMERCIAL USE, AND ALL USE BY FOR PROFIT ENTITIES, IS EXPRESSLY
 * PROHIBITED WITHOUT A LICENSE FROM SIMPLESCALAR, LLC (info@simplescalar.com).
 * 
 * 4. No nonprofit user may place any restrictions on the use of this software,
 * including as modified by the user, by any other authorized user.
 * 
 * 5. Noncommercial and nonprofit users may distribute copies of SimpleScalar
 * in compiled or executable form as set forth in Section 2, provided that
 * either: (A) it is accompanied by the corresponding machine-readable source
 * code, or (B) it is accompanied by a written offer, with no time limit, to
 * give anyone a machine-readable copy of the corresponding source code in
 * return for reimbursement of the cost of distribution. This written offer
 * must permit verbatim duplication by anyone, or (C) it is distributed by
 * someone who received only the executable form, and is accompanied by a
 * copy of the written offer of source code.
 * 
 * 6. SimpleScalar was developed by Todd M. Austin, Ph.D. The tool suite is
 * currently maintained by SimpleScalar LLC (info@simplescalar.com). US Mail:
 * 2395 Timbercrest Court, Ann Arbor, MI 48105.
 * 
 * Copyright (C) 1994-2003 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"
#include "loader.h"
#include "syscall.h"
#include "dlite.h"
#include "options.h"
#include "stats.h"
#include "sim.h"

/*
 * This file implements a functional simulator.  This functional simulator is
 * the simplest, most user-friendly simulator in the simplescalar tool set.
 * Unlike sim-fast, this functional simulator checks for all instruction
 * errors, and the implementation is crafted for clarity rather than speed.
 */

#define DNA 	0
#define DUNIQ 	(1+32+32)
#define DFPCR 	(0+32+32)
#define DFPR_L(N)		(((N)+32)&~1)
#define DFPR_F(N)		(((N)+32)&~1)
#define DFPR_D(N)		(((N)+32)&~1)
#define DFPR(N)			(((N) == 31) ? DNA : ((N)+32))
#define DGPR(N)			(N)
#define DGPR_D(N)		((N) &~1)
//Initial definitions

max_issue_cycle=0;
ILP=0.0;
count=0;


// A register def and use values structure is created
struct reg_value {

int reg_def;
int reg_use;
};


//The binary search tree node structure
struct tree_node{

md_addr_t main_addr; 
int mem_def;
int mem_use;
int count;
struct tree_node *left_branch;
struct tree_node *right_branch;
}; 

typedef struct tree_node node;

node *root_node = NULL , *new_node = NULL , *node1 = NULL, *node3 = NULL; 

node* insert(md_addr_t key, node *leaf)
{
  node* node2;
  if(key == leaf->main_addr)
  node2 = leaf;
  if(key < leaf->main_addr)
  {
    if(leaf->left_branch!=NULL)
{
	leaf->left_branch->count++;
	node2 = insert(key, leaf->left_branch);
}
    else
    {
	new_node = malloc(sizeof(node));
	leaf->left_branch=new_node;
	node2 = leaf->left_branch;
        node2->main_addr=key;
	node2->left_branch = NULL;    
        node2->right_branch = NULL;   
	node2->mem_def=1;	
	node2->mem_use=0;
    } 
  }
  if(key > leaf->main_addr)
  {
    if(leaf->right_branch!=NULL)
      node2 = insert(key, leaf->right_branch);

    else
    { 
	new_node = malloc(sizeof(node));
	leaf->right_branch=new_node;
	node2 = leaf->right_branch;
	node2->main_addr=key;
      	node2->left_branch=NULL;  
      	node2->right_branch=NULL; 
	node2->mem_def=1;	
	node2->mem_use=0;
    }
  }
  return node2;  
}


//Max function calculating the maximum values of 3 variables
int max(int reg_in1, int reg_in2, int reg_in3)
{
if(reg_in1 > reg_in2)
	if(reg_in1 > reg_in3)
	return reg_in1;
	else
	return reg_in3;
else if (reg_in2 > reg_in3)
return reg_in2;
else
return reg_in3;
};


/* simulated registers */
static struct regs_t regs;

/* simulated memory */
static struct mem_t *mem = NULL;

/* track number of refs */
static counter_t sim_num_refs = 0;

/* maximum number of inst's to execute */
static unsigned int max_insts;

/* register simulator-specific options */
void
sim_reg_options(struct opt_odb_t *odb)
{
  opt_reg_header(odb, 
"sim-safe: This simulator implements a functional simulator.  This\n"
"functional simulator is the simplest, most user-friendly simulator in the\n"
"simplescalar tool set.  Unlike sim-fast, this functional simulator checks\n"
"for all instruction errors, and the implementation is crafted for clarity\n"
"rather than speed.\n"
		 );

  /* instruction limit */
  opt_reg_uint(odb, "-max:inst", "maximum number of inst's to execute",
	       &max_insts, /* default */0,
	       /* print */TRUE, /* format */NULL);

}

/* check simulator-specific option values */
void
sim_check_options(struct opt_odb_t *odb, int argc, char **argv)
{
  /* nada */
}

/* register simulator-specific statistics */
void
sim_reg_stats(struct stat_sdb_t *sdb)
{
  stat_reg_counter(sdb, "sim_num_insn",
		   "total number of instructions executed",
		   &sim_num_insn, sim_num_insn, NULL);
  stat_reg_counter(sdb, "sim_num_refs",
		   "total number of loads and stores executed",
		   &sim_num_refs, 0, NULL);
  stat_reg_int(sdb, "sim_elapsed_time",
	       "total simulation time in seconds",
	       &sim_elapsed_time, 0, NULL);
  stat_reg_formula(sdb, "sim_inst_rate",
		   "simulation speed (in insts/sec)",
		   "sim_num_insn / sim_elapsed_time", NULL);
  ld_reg_stats(sdb);
  mem_reg_stats(mem, sdb);
}

/* initialize the simulator */
void
sim_init(void)
{
  sim_num_refs = 0;

  /* allocate and initialize register file */
  regs_init(&regs);

  /* allocate and initialize memory space */
  mem = mem_create("mem");
  mem_init(mem);
}

/* load program into simulated state */
void
sim_load_prog(char *fname,		/* program to load */
	      int argc, char **argv,	/* program arguments */
	      char **envp)		/* program environment */
{
  /* load program text and data, set up environment, memory, and regs */
  ld_load_prog(fname, argc, argv, envp, &regs, mem, TRUE);

  /* initialize the DLite debugger */
  dlite_init(md_reg_obj, dlite_mem_obj, dlite_mstate_obj);
}

/* print simulator-specific configuration information */
void
sim_aux_config(FILE *stream)		/* output stream */
{
  /* nothing currently */

}

/* dump simulator-specific auxiliary simulator statistics */
void
sim_aux_stats(FILE *stream)		/* output stream */
{
  /* nada */
  printf("\n Max Issue Cycle = %d", max_issue_cycle);
  printf("\n Final ILP = %f \n", (float) sim_num_insn/max_issue_cycle);
  
}

/* un-initialize simulator-specific state */
void
sim_uninit(void)
{
  /* nada */
}


/*
 * configure the execution engine
 */

/*
 * precise architected register accessors
 */

/* next program counter */
#define SET_NPC(EXPR)		(regs.regs_NPC = (EXPR))

/* current program counter */
#define CPC			(regs.regs_PC)

/* general purpose registers */
#define GPR(N)			(regs.regs_R[N])
#define SET_GPR(N,EXPR)		(regs.regs_R[N] = (EXPR))

#if defined(TARGET_PISA)

/* floating point registers, L->word, F->single-prec, D->double-prec */
#define FPR_L(N)		(regs.regs_F.l[(N)])
#define SET_FPR_L(N,EXPR)	(regs.regs_F.l[(N)] = (EXPR))
#define FPR_F(N)		(regs.regs_F.f[(N)])
#define SET_FPR_F(N,EXPR)	(regs.regs_F.f[(N)] = (EXPR))
#define FPR_D(N)		(regs.regs_F.d[(N) >> 1])
#define SET_FPR_D(N,EXPR)	(regs.regs_F.d[(N) >> 1] = (EXPR))

/* miscellaneous register accessors */
#define SET_HI(EXPR)		(regs.regs_C.hi = (EXPR))
#define HI			(regs.regs_C.hi)
#define SET_LO(EXPR)		(regs.regs_C.lo = (EXPR))
#define LO			(regs.regs_C.lo)
#define FCC			(regs.regs_C.fcc)
#define SET_FCC(EXPR)		(regs.regs_C.fcc = (EXPR))

#elif defined(TARGET_ALPHA)

/* floating point registers, L->word, F->single-prec, D->double-prec */
#define FPR_Q(N)		(regs.regs_F.q[N])
#define SET_FPR_Q(N,EXPR)	(regs.regs_F.q[N] = (EXPR))
#define FPR(N)			(regs.regs_F.d[(N)])
#define SET_FPR(N,EXPR)		(regs.regs_F.d[(N)] = (EXPR))

/* miscellaneous register accessors */
#define FPCR			(regs.regs_C.fpcr)
#define SET_FPCR(EXPR)		(regs.regs_C.fpcr = (EXPR))
#define UNIQ			(regs.regs_C.uniq)
#define SET_UNIQ(EXPR)		(regs.regs_C.uniq = (EXPR))

#else
#error No ISA target defined...
#endif

/* precise architected memory state accessor macros */
#define READ_BYTE(SRC, FAULT)						\
  ((FAULT) = md_fault_none, addr = (SRC), MEM_READ_BYTE(mem, addr))
#define READ_HALF(SRC, FAULT)						\
  ((FAULT) = md_fault_none, addr = (SRC), MEM_READ_HALF(mem, addr))
#define READ_WORD(SRC, FAULT)						\
  ((FAULT) = md_fault_none, addr = (SRC), MEM_READ_WORD(mem, addr))
#ifdef HOST_HAS_QWORD
#define READ_QWORD(SRC, FAULT)						\
  ((FAULT) = md_fault_none, addr = (SRC), MEM_READ_QWORD(mem, addr))
#endif /* HOST_HAS_QWORD */

#define WRITE_BYTE(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, addr = (DST), MEM_WRITE_BYTE(mem, addr, (SRC)))
#define WRITE_HALF(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, addr = (DST), MEM_WRITE_HALF(mem, addr, (SRC)))
#define WRITE_WORD(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, addr = (DST), MEM_WRITE_WORD(mem, addr, (SRC)))
#ifdef HOST_HAS_QWORD
#define WRITE_QWORD(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, addr = (DST), MEM_WRITE_QWORD(mem, addr, (SRC)))
#endif /* HOST_HAS_QWORD */

/* system call handler macro */
#define SYSCALL(INST)	sys_syscall(&regs, mem_access, mem, INST, TRUE)

/* start simulation, program loaded, processor precise state initialized */
void
sim_main(void)
{
struct reg_value regist[102];
int i,p;
for (i=0; i<102; i++)
{
	regist[i].reg_def=1;
	regist[i].reg_use=0;
}
i=0;
int reg_in0, reg_in1, reg_in2, reg_out0, reg_out1, RAW=0, WAR=0, WAW=0, MRAW=0, MWAR=0, MWAW=0, r_issue_cycle=0, m_issue_cycle=0, issue_cycle=0, old_issue_cycle=0;  //Initialization of all variables
float ILP = 0.0;
md_inst_t inst;
  register md_addr_t addr;
  md_addr_t tempaddr;
  enum md_opcode op;
  register int is_write;
  enum md_fault_type fault;

  fprintf(stderr, "sim: ** starting functional simulation **\n");

  /* set up initial default next PC */
  regs.regs_NPC = regs.regs_PC + sizeof(md_inst_t);

  /* check for DLite debugger entry condition */
  if (dlite_check_break(regs.regs_PC, /* !access */0, /* addr */0, 0, 0))
    dlite_main(regs.regs_PC - sizeof(md_inst_t),
	       regs.regs_PC, sim_num_insn, &regs, mem);


  while (TRUE)
    {
      /* maintain $r0 semantics */
      regs.regs_R[MD_REG_ZERO] = 0;
#ifdef TARGET_ALPHA
      regs.regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

      /* get the next instruction to execute */
      MD_FETCH_INST(inst, mem, regs.regs_PC);

      /* keep an instruction count */
      sim_num_insn++;

      /* set default reference address and access mode */
      addr = 0; is_write = FALSE;

      /* set default fault - none */
      fault = md_fault_none;

      /* decode the instruction */
      MD_SET_OPCODE(op, inst);

      /* execute the instruction */
      switch (op)
	{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)		\
	case OP:							\
        reg_out0=O1;							\
	reg_out1=O2;							\
	reg_in0=I1;							\
	reg_in1=I2;							\
	reg_in2=I3;							\
	SYMCAT(OP,_IMPL);						\
          break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
        case OP:							\
          panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#define DECLARE_FAULT(FAULT)						\
	  { fault = (FAULT); break; }
#include "machine.def"
	default:
	  panic("attempted to execute a bogus opcode");
      }


old_issue_cycle=max_issue_cycle;

i=0;


      if (fault != md_fault_none)
	fatal("fault (%d) detected @ 0x%08p", fault, regs.regs_PC);

      if (verbose)
	{
	  myfprintf(stderr, "%10n [xor: 0x%08x] @ 0x%08p: ",
		    sim_num_insn, md_xor_regs(&regs), regs.regs_PC);
	  md_print_insn(inst, regs.regs_PC, stderr);
	  if (MD_OP_FLAGS(op) & F_MEM)
	    myfprintf(stderr, "  mem: 0x%08p", addr);
	  fprintf(stderr, "\n");
	  /* fflush(stderr); */
	}

 	
//Checking for Memory access flag and creating a root node for binary tree
      if (MD_OP_FLAGS(op) & F_MEM)
	sim_num_refs++;
	if(sim_num_refs == 1)
	  {
					
		root_node = malloc(sizeof(node));
		root_node->left_branch = root_node->right_branch = NULL;
		root_node->main_addr = addr;
		root_node->mem_def=1;
		root_node->mem_use=0;
		if ((MD_OP_FLAGS(op)) & F_LOAD)
		MRAW = root_node->mem_def;
		if ((MD_OP_FLAGS(op)) & F_STORE)
		{
			MWAR = root_node->mem_use;
			MWAW = root_node->mem_def;
		}  
	  }
	  else
	  {
		 
		node3 = insert(addr, root_node);
		if((MD_OP_FLAGS(op)) & F_LOAD)
		  {	
		  	MRAW = node3->mem_def;
		  }
		  if ((MD_OP_FLAGS(op)) & F_STORE)
		  {
			MWAR = node3->mem_use;
			MWAW = node3->mem_def;	
		 }
		
	  }		
		
}	


RAW=max(regist[reg_in0].reg_def,regist[reg_in1].reg_def,regist[reg_in2].reg_def);
WAR=max(regist[reg_out0].reg_use,regist[reg_out1].reg_use,0);
WAW=max(regist[reg_out0].reg_def,regist[reg_out1].reg_def,0);

r_issue_cycle=max(RAW, WAR, WAW);
m_issue_cycle=max(MRAW, MWAR, MWAW);
issue_cycle=max(r_issue_cycle, m_issue_cycle, 0); //Calculation of Issue Cycle




if(reg_out0 != DNA)

regist[reg_out0].reg_def=issue_cycle+1;
if(reg_out1 != DNA)
regist[reg_out1].reg_def=issue_cycle+1;


if(reg_in0 != DNA)
regist[reg_in0].reg_use=issue_cycle;
if(reg_in1 != DNA)
regist[reg_in1].reg_use=issue_cycle;
if(reg_in2 != DNA)
regist[reg_in2].reg_use=issue_cycle;


//Updating the def and use timestamps of memory locations after each instruction
if (MD_OP_FLAGS(op) & F_MEM)
{

if (sim_num_refs==1)
{
	if ((MD_OP_FLAGS(op)) & F_LOAD)
		{
			
			root_node->mem_use=issue_cycle;
		}
		if ((MD_OP_FLAGS(op)) & F_STORE)
		{
			
			root_node->mem_def = issue_cycle + 1;
		}  
	  }
	  else
	  {
		 if((MD_OP_FLAGS(op)) & F_LOAD)
		  {	
		  	node3->mem_use = issue_cycle;
		   }
		  if ((MD_OP_FLAGS(op)) & F_STORE)
		  {
			is_write = TRUE;			
			node3->mem_def = issue_cycle + 1;		
			
		  }
		}

}



max_issue_cycle=max(old_issue_cycle, issue_cycle,0);







ILP = (float) sim_num_insn/max_issue_cycle;
fprintf(stderr,"ILP = %f", ILP);
     /* check for DLite debugger entry condition */
      if (dlite_check_break(regs.regs_NPC,
			    is_write ? ACCESS_WRITE : ACCESS_READ,
			    addr, sim_num_insn, sim_num_insn))
	dlite_main(regs.regs_PC, regs.regs_NPC, sim_num_insn, &regs, mem);

      /* go to the next instruction */
      regs.regs_PC = regs.regs_NPC;
      regs.regs_NPC += sizeof(md_inst_t);

	

      /* finish early? */
      if (max_insts && sim_num_insn >= max_insts)
	return;

	
    }
}
