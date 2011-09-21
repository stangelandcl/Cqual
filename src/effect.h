/* This file is part of cqual.
   Copyright (C) 2000-2002 The Regents of the University of California.

cqual is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

cqual is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cqual; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA. */

#ifndef EFFECT_H
#define EFFECT_H

#include "linkage.h"
#include "quals.h"

EXTERN_C_BEGIN

void init_effects(void);
void init_store(void);

/**************************************************************************
 *                                                                        *
 * Abstract locations                                                     *
 *                                                                        *
 **************************************************************************/

/* Make a new, unique abstract location */
aloc make_aloc(const char *name, qtype points_to);

/* Return al's name */
const char *name_aloc(aloc al);

/* Hash function on alocs */
int hash_aloc(aloc al);

/* Return the qtype al points to */
qtype points_to_aloc(aloc al);

/* Return TRUE iff al1 and al2 are equal */
bool eq_aloc(aloc al1, aloc al2);

/* Unify al1 and al2 */
void unify_aloc(location loc, aloc al1, aloc al2);

/* Print the abstract location al */
int print_aloc(printf_func pf, aloc al);

/**************************************************************************
 *                                                                        *
 * Effects                                                                *
 *                                                                        *
 **************************************************************************/

/* The \emptyset of effects */
extern effect effect_empty;

/* returns aloc from effect */
aloc aloc_effect(effect e);

/* mark this aloc as interesting (optimization help for store) */
void mark_aloc_interesting(aloc al);

/* The effect {al} */
effect effect_single(aloc al);

/* The effect alloc(al), which is different than al itself */
effect effect_alloc(aloc al);

/* The effect rwr(al) */
effect effect_rwr(aloc al);

/* The effect r(al) */
effect effect_r(aloc al);

/* The effect wr(al) */
effect effect_wr(aloc al);

/* The effect k(al) */
effect effect_constr(aloc al, eff_kind k);

/* Create a fresh effect variable */
effect effect_var(const char *name);

/* Create a fresh effect variable with a unique name */
effect effect_fresh(void);

/* The effect e1 \cup e2 */
effect effect_union(effect e1, effect e2);

/* The effect e1 \cap e2.  e2 cannot be the empty effect. */
effect effect_inter(effect e1, effect e2);

/* Mark this effect as interesting (optimization for caching
   reachability queries) */
void mark_effect_interesting(effect e);

/* Return FALSE if e1 may not be equal to e2.  Does not generate a
   constraint. */
bool eq_effect(effect e1, effect e2);

/* Add constraint that effect set e1 is a subset of effect set e2. */
void mkleq_effect(effect e1, effect e2);

/* Add constraint that effect set e1 is equal to the effect set e2. */
void mkeq_effect(effect e1, effect e2);

/* Add constraint that effect sets e1 and e2 are equal and
   indistinguishable.  e1 and e2 must be effect variables. */
void unify_effect(effect e1, effect e2);

/* Remember the constraint al !<= e2.  The constraint will be checked
   when check_nin is called. */
void mknin_aloc_effect(location loc, aloc al, eff_kind kind, effect e);

/* Remember the constraint e1 !<= e2.  The constraint will be checked
  when check_nin is called.  e1 must not contain any effect variables.  */
void mknin_effect(location loc, effect e1, effect e2);

/* Forall rho.kind(rho) \notin e */
void forall_aloc_mknin_effect(location loc, eff_kind kind, effect e);

/* Forall rho s.t. kind(rho) \in e1.kind(rho) \notin e2 */
void forall_aloc_in_effect_mknin_effect(location loc, 
					eff_kind kind1, effect e1, 
					eff_kind kind2, effect e2);

/* Test whether the constraint al \not\in e is satisfiable, i.e.,
   whether anything in al is in e, which must be an effect variable.
   Return TRUE if the constraint is unsatisfiable, FALSE if the
   constraint is satisfiable. */
bool check_nin_aloc_effect(aloc al, effect e);

/* Call check_nin_aloc_effect on all the constraints generated by
   mknin_aloc_effect and mknin_effect.  Generates error messages with
   calls to report_qerror. */
void check_nin(void);

int print_effect(printf_func pf, effect e);

/**************************************************************************
 *                                                                        *
 * Stores                                                                 *
 *                                                                        *
 **************************************************************************/

/* Return TRUE iff s1 and s2 are the same store */
bool eq_store(store s1, store s2);

/* Make a fresh store variable */
store make_store_var(location loc, const char *name);

/* Make the store s|_e, s restricted to e */
store make_store_filter(location loc, store s, effect e);

/* Make the store ref(al, s) = s + {al : fresh} */
store make_store_ref(location loc, store s, aloc al);

/* Make the store ref(al1, ref(al2, ..., ref(aln, s))) for ali in e.  e
   must be a singleton effect or a union */
store make_store_ref_effect(location loc, store s, effect e);

/* Make the store store_ow(s1, s2, e) = s1|_e + s2|_{!e} */
store make_store_ow(location loc, store s1, store s2, effect e);

/* Make the store assign(s, al, qt) = (s - al) + {al : qt}, plus
   the constraint that if al is \omega, then qt = s(al) .  If strong
   is TRUE then this must be a strong update. */
store make_store_assign(location loc, const char *err_msg, store s, aloc al,
			qtype qt, bool strong);

/* Add constraint left <= right */
void mkleq_store(location loc, const char *err_msg, store left, store right);

/* Add constraint left = right */
void mkeq_store(location loc, const char *err_msg, store s1, store s2);

/* Return a fresh store s such that s1 <= s and s2 <= s */
store lub_store(location loc, const char *err_msg, store s1, store s2);

/* Add constraint left == right, and can't distinguish left from right
   afterward. */
void unify_store(location loc, const char *err_msg, store s1, store s2);

/* Mark the al cell of s as propagating, and propagate it to
   neighboring stores */
void propagate_store_cell_forward(store s, aloc al);
void propagate_store_cell_backward(store s, aloc al);

/* Compute linearities */
void compute_lins(void);

/* Return the linearity of al in s, either 0, 1, or 2 (= omega).
  compute_lins() must be called first, and this function only works on
  assign stores */
int lin_from_store(store s, aloc al);

/* Return al's qtype in s */
qtype qtype_from_store(store s, aloc al);

int print_store(printf_func pf, store s);
int print_store_cells(printf_func pf, pr_qual_fn pr_qual, store s,
                      bool print_alocs);
int print_store_raw(printf_func pf, store s);

EXTERN_C_END

#endif
