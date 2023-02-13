#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_trace(void) {
  int n;
  argint(0, &n);

  struct proc *p = myproc();
  p->tracenum = n;

  return 0;
}

uint64 sys_sysinfo(void) {
  struct sysinfo info;
  uint64 addr; // user pointer to struct stat
  struct proc *p = myproc();

  argaddr(0, &addr);

  info.freemem = free_mem();
  info.nproc = num_proc();

  if(copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0)
    return -1;

  return 0;
}

uint64 sys_pgaccess(void) {
  uint64 usrpge_ptr;//待检测页表起始指针
  int npage;//待检测页表个数
  uint64 useraddr;//稍后写入用户内存
  int i;
  int res = 0;

  pte_t *pte_addr;
  pte_t pte;

  argaddr(0, &usrpge_ptr);
  argint(1, &npage);
  argaddr(2, &useraddr);
  if (npage > 64) {
    return -1;
  }

  pagetable_t pgtable = myproc()->pagetable;

  for (i = 0; i < npage; i++) {
    pte_addr = walk(pgtable, usrpge_ptr, 0);
    pte = (*pte_addr);

    if (pte & PTE_A) {
      res = res | (1<<i);
      (*pte_addr) = pte & (~PTE_A);
    }

    usrpge_ptr += PGSIZE;
  }

  if(copyout(pgtable, useraddr, (char *)&res, sizeof(res)) < 0) {
    return -1;
  }

  return 0;
}