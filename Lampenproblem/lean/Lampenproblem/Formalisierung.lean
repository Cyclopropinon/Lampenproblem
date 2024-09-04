import Mathlib.Tactic.Find

-- n := Lampenanzahl
-- m := Anzahl Schritte
-- k := Rundenzahl

inductive Vec (α : Type) (n : Nat) where
def all_on : Vec Bool n := sorry
def all_off : Vec Bool n := sorry
def flip_at : Vec Bool n → m → Vec Bool n := sorry
inductive LampState : Vec Bool n  → (m : Nat) → (k : Nat) → Type where
  | start : LampState all_off 1 1
  | step :  LampState v m k → LampState (flip_at v m) (m + k) (m / n)
def positive  (_ : LampState v n m) : Prop := v = all_on ∨ v = all_off

--
