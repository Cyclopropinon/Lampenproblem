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

def step_n {n: Nat} {v: Vec Bool n} (NumSteps: Nat) (ls: LampState v m k) : Σ (v': Vec Bool n) (m' k' : Nat), LampState v' m' k' :=
  match NumSteps with
  | 0 => ⟨_, _, _, ls⟩
  | Nat.succ x => match step_n x ls with
    | ⟨_, m', _, ls'⟩ => ⟨_, _, m'/n, ls'.step⟩
    --| ⟨v', m', k', ls'⟩ => ⟨flip_at v' m', m' + k', m'/n, ls'.step⟩

#print LampState.start

def positiveRunde  (_ : LampState v n m) : Prop := v = all_on ∨ v = all_off


--
