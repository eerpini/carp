#include "carp_lexer.h"

void carp_lex_lex (carp_machine_state *m, carp_tok *tokens) {
  assert(m != NULL);
  assert(tokens != NULL);

  long long length = -1;
  carp_tok *tmp = tokens;

  carp_ht_init(&m->labels);

  while (tmp != NULL) {
    switch (tmp->type) {
    case ct(UNDEF): {
      fprintf(stderr, "Unknown token <%s>\n", tmp->lexeme);
      carp_lex_exit(tokens, &m->labels, 1);
      break; }
      
    case ct(NUM): {
      long long num = atoi(tmp->lexeme);
      tmp->value = num;
      break; }

    case ct(REG): {
      carp_reg reg = carp_reg_lookup(tmp->lexeme);
      tmp->value = reg;
      break; }

    case ct(LBL): {
      carp_ht *res = carp_ht_set(&m->labels, tmp->lexeme, tmp->pos);
      if (res == NULL) {
	fprintf(stderr, "Could not make label <%s>\n", tmp->lexeme);
	carp_lex_exit(tokens, &m->labels, 1);
      }

      long long instr = CARP_INSTR_NOP;
      tmp->value = instr;
      break; }

    case ct(FUNC): {
      carp_ht *res = carp_ht_get(&m->labels, tmp->lexeme);
      if (res == NULL) {
	fprintf(stderr, "Unknown label <%s>\n", tmp->lexeme);
	carp_lex_exit(tokens, &m->labels, 1);
      }

      tmp->value = res->value;
      break; }

    case ct(VAR): {
      
      break; }

    case ct(INSTR): {
      long long instr = carp_instr_lookup(tmp->lexeme);
      tmp->value = instr;
      break; }
    }

    /*printf("[%04lld] %5s (%5s) = %4lld\n",
      tmp->pos, tmp->lexeme, carp_reverse_type[tmp->type], tmp->value);
    // */
    tmp = tmp->next;
    length++;
  }

  long long code[length];
  tmp = tokens;

  while (tmp != NULL) {
    code[tmp->pos] = tmp->value;
    tmp = tmp->next;
  }

  carp_lex_cleanup(tokens);
  carp_vm_make(m);
  carp_vm_load(m, code, length);
}

void carp_lex_exit (carp_tok *tokens, carp_ht *labels, int code) {
  assert(tokens != NULL);
  assert(labels != NULL);

  carp_lex_cleanup(tokens);
  carp_ht_cleanup(labels);
  exit(code);
}

carp_reg carp_reg_lookup (char *s) {
  assert(s != NULL);

  for (int i = 0; i < CARP_NUM_REGS; i++)
    if (!strcmp(carp_reverse_reg[i], s))
      return i;

  return -1;
}

carp_instr carp_instr_lookup (char *s) {
  assert(s != NULL);

  for (int i = 0; i < CARP_NUM_INSTRS; i++)
    if (!strcmp(carp_reverse_instr[i], s))
      return i;

  return -1;
}
