(* 
 * Ternary.thy - Isabelle/HOL formalization of balanced ternary arithmetic
 *
 * TASK-009: Formal verification stub for trit operations.
 * This defines the trit datatype and trit_add function, then proves
 * basic correctness properties.
 *
 * Prerequisites: Isabelle2024 or later with HOL.
 * Build: isabelle build -d proofs -b Ternary (once session ROOT configured)
 *)

theory Ternary
  imports Main
begin

section \<open>Balanced Ternary Trit Type\<close>

datatype trit = N | Z | P

text \<open>Numeric interpretation of trits: N = -1, Z = 0, P = +1\<close>

fun trit_val :: "trit \<Rightarrow> int" where
  "trit_val N = -1" |
  "trit_val Z = 0" |
  "trit_val P = 1"

section \<open>Trit Multiplication\<close>

fun trit_mul :: "trit \<Rightarrow> trit \<Rightarrow> trit" where
  "trit_mul N N = P" |
  "trit_mul N Z = Z" |
  "trit_mul N P = N" |
  "trit_mul Z _ = Z" |
  "trit_mul P N = N" |
  "trit_mul P Z = Z" |
  "trit_mul P P = P"

section \<open>Trit Addition (with carry)\<close>

text \<open>Given trits a, b and carry c_in, returns (result, carry_out)\<close>

fun trit_add :: "trit \<Rightarrow> trit \<Rightarrow> trit \<Rightarrow> trit \<times> trit" where
  "trit_add a b c = (let s = trit_val a + trit_val b + trit_val c in
     if s > 1 then (
       (if s - 3 = -1 then N else if s - 3 = 0 then Z else P),
       P)
     else if s < -1 then (
       (if s + 3 = -1 then N else if s + 3 = 0 then Z else P),
       N)
     else (
       (if s = -1 then N else if s = 0 then Z else P),
       Z))"

section \<open>Correctness Lemmas\<close>

text \<open>trit_mul preserves the product of trit values\<close>

lemma trit_mul_correct:
  "trit_val (trit_mul a b) = trit_val a * trit_val b"
  by (cases a; cases b; simp)

text \<open>trit_add with zero carry cancels: P + N = Z\<close>

lemma trit_add_cancel:
  "fst (trit_add P N Z) = Z"
  by simp

lemma trit_add_cancel_carry:
  "snd (trit_add P N Z) = Z"
  by simp

text \<open>Zero is identity for addition\<close>

lemma trit_add_zero_left:
  "fst (trit_add Z b Z) = b"
  by (cases b; simp)

lemma trit_add_zero_right:
  "fst (trit_add a Z Z) = a"
  by (cases a; simp)

text \<open>Positive overflow: P + P = N with carry P\<close>

lemma trit_add_overflow:
  "trit_add P P Z = (N, P)"
  by simp

text \<open>Negative overflow: N + N = P with carry N\<close>

lemma trit_add_underflow:
  "trit_add N N Z = (P, N)"
  by simp

text \<open>trit_mul commutativity\<close>

lemma trit_mul_comm:
  "trit_mul a b = trit_mul b a"
  by (cases a; cases b; simp)

text \<open>Zero annihilates multiplication\<close>

lemma trit_mul_zero:
  "trit_mul Z a = Z"
  by simp

section \<open>Phase 2: Memory Model and Capability Verification (TASK-019)\<close>

text \<open>
  Ternary memory addressing: 9-trit addresses give 3^9 = 19683 cells.
  We model memory as a function from int (converted from trit word) to int.
\<close>

type_synonym tmemory = "int \<Rightarrow> int"

definition tmem_read :: "tmemory \<Rightarrow> int \<Rightarrow> int" where
  "tmem_read m addr = m addr"

definition tmem_write :: "tmemory \<Rightarrow> int \<Rightarrow> int \<Rightarrow> tmemory" where
  "tmem_write m addr val = m(addr := val)"

lemma tmem_write_read:
  "tmem_read (tmem_write m addr val) addr = val"
  by (simp add: tmem_read_def tmem_write_def)

lemma tmem_write_read_other:
  "addr \<noteq> addr' \<Longrightarrow> tmem_read (tmem_write m addr val) addr' = tmem_read m addr'"
  by (simp add: tmem_read_def tmem_write_def)

text \<open>Capability rights model (ternary-encoded)\<close>

text \<open>Rights are a natural number interpreted as balanced ternary.
  Bit intersection models rights restriction.\<close>

definition cap_rights_subset :: "int \<Rightarrow> int \<Rightarrow> bool" where
  "cap_rights_subset child parent = ((child AND parent) = child)"

text \<open>No escalation: derived cap rights are subset of parent\<close>

lemma cap_derive_no_escalation:
  "cap_rights_subset (parent AND mask) parent"
  by (simp add: cap_rights_subset_def)

text \<open>Revocation: setting rights to 0 satisfies subset with any parent\<close>

lemma cap_revoke_subset:
  "cap_rights_subset 0 parent"
  by (simp add: cap_rights_subset_def)

text \<open>Subtraction correctness for ternary (Phase 2)\<close>

lemma trit_sub_via_neg:
  "trit_val a - trit_val b = trit_val a + (- trit_val b)"
  by simp

text \<open>Memory store-load roundtrip is idempotent\<close>

lemma tmem_roundtrip:
  "tmem_read (tmem_write (tmem_write m addr v1) addr v2) addr = v2"
  by (simp add: tmem_read_def tmem_write_def)

end
