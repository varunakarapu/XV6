#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

// Return the address of the PTE in page table pgdir
// that corresponds to virtual address va.  If alloc!=0,
// create any required page table pages.
static pte_t *
walkpgdir(pde_t *pgdir, const void *va, int alloc)
{
  pde_t *pde;
  pte_t *pgtab;

  pde = &pgdir[PDX(va)];
  if(*pde & PTE_P){
    pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
  } else {
    if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
      return 0;
    // Make sure all those PTE_P bits are zero.
    memset(pgtab, 0, PGSIZE);
    // The permissions here are overly generous, but they can
    // be further restricted by the permissions in the page table
    // entries, if necessary.
    *pde = V2P(pgtab) | PTE_P | PTE_W | PTE_U;
  }
  return &pgtab[PTX(va)];
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not
// be page-aligned.
static int
mappages(pde_t *pgdir, void *va, uint size, uint pa, int perm)
{
  char *a, *last;
  pte_t *pte;

  a = (char*)PGROUNDDOWN((uint)va);
  last = (char*)PGROUNDDOWN(((uint)va) + size - 1);
  for(;;){
    if((pte = walkpgdir(pgdir, a, 1)) == 0)
      return -1;
    if(*pte & PTE_P)
      panic("remap");
    *pte = pa | perm | PTE_P;
    if(a == last)
      break;
    a += PGSIZE;
    pa += PGSIZE;
  }
  return 0;
}

int
lazy_alloc(uint addr) {
  struct proc *proc = myproc();
  // page-faults on a virtual memory address higher than any allocated with sbrk()
  // this should be >= not > !!!

   if(addr >= KERNBASE)
    return 0;
  // if(newsz < oldsz)
  //   return oldsz;

  // if (addr >= p->sz) {
  //   // printf("lazy_alloc: access invalid address");
  //   return -1;
  // }

  // if (addr < p->trapframe->sp) {
  //   // printf("lazy_alloc: access address below stack");
  //   return -2;
  // }
  
  // uint pa = PGROUNDDOWN(addr);
  // char* mem = kalloc();
  // if (mem == 0) {
  //   // printf("lazy_alloc: kalloc failed");
  //   return -3;
  // }
  
  // memset(mem, 0, PGSIZE);
  // // if(mappages(p->pagetable, pa, PGSIZE, (uint64)mem, PTE_W|PTE_X|PTE_R|PTE_U) != 0){
  // //   kfree(mem);
  // //   return -4;
  // // }



  // if(mappages(p->pgdir, (char*)pa, PGSIZE, V2P(mem), PTE_W|PTE_U) != 0)
  // {
  //   kfree(mem);
  //   return -4;
  // }

  char *mem;
  uint a = PGROUNDDOWN(rcr2());
  // uint a = addr;
  uint newsz = proc->sz;
  for(; a < newsz; a += PGSIZE){
      mem = kalloc();
      memset(mem, 0, PGSIZE);
      mappages(proc->pgdir, (char*)a, PGSIZE, V2P(mem), PTE_W|PTE_U);
  }


  return 0;
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed)
      exit();
    myproc()->tf = tf;
    syscall();
    if(myproc()->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpuid() == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;

  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpuid(), tf->eip, rcr2());
      panic("trap");
    }

    //缺页的情况
	if (tf->trapno == T_PGFLT)
	{
		char* pa;
		uint addr =  PGROUNDDOWN(rcr2());
		pa = kalloc();

		if (pa == 0)
		{
			panic("in functio trap:out of memory\n");
			return;
		}
		
		memset(pa,0,PGSIZE);
		
		if (mappages(myproc()->pgdir,(void*)addr,PGSIZE,V2P(pa),PTE_W|PTE_U)<0)
		{
			panic("error occurs while mapping:in function trap\n");
			return ;
		}

		break;

	}
	//缺页情况结束

    // uint addr = rcr2();
    // // int ret = lazy_alloc(tf->eip+addr);
    // int ret = lazy_alloc(addr);
    // cprintf("pid %d %s: trap %d err %d on cpu %d "
    //         "eip 0x%x addr 0x%x--kill proc\n",
    //         myproc()->pid, myproc()->name, tf->trapno,
    //         tf->err, cpuid(), tf->eip, rcr2());
    // cprintf("ret11=%d\n", ret);
    // if (ret < 0) 
    // {
    //    cprintf("pid %d %s: trap %d err %d on cpu %d "
    //         "eip 0x%x addr 0x%x--kill proc\n",
    //         myproc()->pid, myproc()->name, tf->trapno,
    //         tf->err, cpuid(), tf->eip, rcr2());
    // myproc()->killed = 1;

    //   // myproc()->killed = 1;
    //   // p->killed = 1;
    // }
    // else
    // {

    // }
    // In user space, assume process misbehaved.
   
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER) {
    yield();
  }

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();
}
