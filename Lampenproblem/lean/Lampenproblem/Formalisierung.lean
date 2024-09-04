import Mathlib.Tactic.Find

-- n := Lampenanzahl
-- m := Anzahl Schritte
-- k := Rundenzahl


--def flip_at_helper: (n: Nat) → (m: Nat) → BitVec n :=
--  BitVec.ofNatLt m sorry

-- definiere eine hilfsfunktion, um ein einzelnes bit zu flippen. WTF WARUM IST DAS NICHT IN DER STANDARDBIBLOTHEK???????
def flip_at_helper {n: Nat} (m: Nat): (BitVec n) :=
  BitVec.ofNat n (2^m)
  --BitVec.ofNatLt m sorry

--inductive Vec (α : Type) (n : Nat) where
def all_on: BitVec n := BitVec.allOnes n
def all_off: BitVec n := BitVec.zero n
def flip_at {v: BitVec n} (m: Nat): (BitVec n) := BitVec.xor v (flip_at_helper m)
--def flip_at: BitVec n → m → BitVec n := sorry
inductive LampState: BitVec n  → (m: Nat) → (k: Nat) → Type where
  | start: LampState all_off 1 1
  | step:  LampState v m k → LampState (flip_at v m) (m + k) (m / n)

def step_n {n: Nat} {v: BitVec n} (NumSteps: Nat) (ls: LampState v m k) : Σ (v': BitVec n) (m' k' : Nat), LampState v' m' k' :=
  match NumSteps with
  | 0 => ⟨_, _, _, ls⟩
  | Nat.succ x => match step_n x ls with
    | ⟨_, m', _, ls'⟩ => ⟨_, _, m'/n, ls'.step⟩
    --| ⟨v', m', k', ls'⟩ => ⟨flip_at v' m', m' + k', m'/n, ls'.step⟩

#print LampState.start
#check BitVec
#check BitVec.zero 3
#check BitVec.allOnes 3

--#print BitVec.zero 3
--#print BitVec.allOnes 3

def positiveRunde  (_ : LampState v n m) : Prop := v = all_on ∨ v = all_off


--
