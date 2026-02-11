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

end
