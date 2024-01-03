struct proc_q {
  int start;
  int end;
  struct proc *buf[NPROC];
};

void create_q(struct proc_q *q) {
  int i;
  q->start = 0;
  q->end = 0;
  for (i = 0; i < NPROC; i++) {
    q->buf[i] = 0;
  }
}

void enqueue_p(struct proc_q *q , struct proc *p) {
  q->buf[q->end++] = p;
  q->end %= NPROC;
}

struct proc *dequeue_p(struct proc_q *q) {
  struct proc* p = q->buf[q->start];
  q->buf[q->start++] = 0;
  q->start %= NPROC;
  return p;
}

int not_in_q(struct proc_q *q, struct proc *p) {
  int i = 0;
  for (i = 0; i < NPROC; i++) {
    if (q->buf[i] == p) {
      return 0;
    }
  }
  return 1;
}

struct proc *first_p(struct proc_q *q) {
  return q->buf[q->start];
}

int q_is_empty(struct proc_q *q) {
  int i = 0;
  for (i = 0; i < NPROC; i++) {
    if (q->buf[i] != 0) {
      return 0;
    }
  }
  return 1;
}
