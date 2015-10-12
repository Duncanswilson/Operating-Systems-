#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

TAILQ_HEAD(tailhead, entry) head;

struct entry {
  char c;
  TAILQ_ENTRY(entry) entries;
};

void add_to_queue(char ch) {
  struct entry *elem;
  elem = malloc(sizeof(struct entry));
  if (elem) {
    elem->c = ch;
  }
  TAILQ_INSERT_HEAD(&head, elem, entries);
}

int main(int argc, char *argv[]) {
  char ch = 'A';
  int i;
  struct entry *elem;

  TAILQ_INIT(&head);
  for (i=0; i<4; i++) {
    add_to_queue(ch);
    ch++;
    add_to_queue(ch);

    elem = head.tqh_first;
    TAILQ_REMOVE(&head, head.tqh_first, entries);
    free(elem);
  }
  exit(0);
}